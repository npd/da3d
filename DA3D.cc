#include "DA3D.h"
#include "Image.h"
#include "WeightMap.h"
#include "Utils.h"
#include "DftPatch.h"
#include <omp.h>
#include <algorithm>
#include <cmath>
#include <cassert>

using std::max;

namespace da3d {

void ColorTransform(const Image &src, Image *dst) {
  for (int row = 0; row < src.rows(); ++row) {
    for (int col = 0; col < src.columns(); ++col) {
      float r, g, b;
      r = src.val(col, row, 0);
      g = src.val(col, row, 1);
      b = src.val(col, row, 2);
      dst->val(col, row, 0) = (r + g + b) / sqrtf(3);
      dst->val(col, row, 1) = (r - b) / sqrtf(2);
      dst->val(col, row, 2) = (r - 2 * g + b) / sqrtf(6);
    }
  }
}

void ColorTransformInverse(const Image &src, Image *dst) {
  for (int row = 0; row < src.rows(); ++row) {
    for (int col = 0; col < src.columns(); ++col) {
      float y, u, v;
      y = src.val(col, row, 0);
      u = src.val(col, row, 1);
      v = src.val(col, row, 2);
      dst->val(col, row, 0) = (sqrtf(2) * y + sqrtf(3) * u + v) / sqrtf(6);
      dst->val(col, row, 1) = (y - sqrtf(2) * v) / sqrtf(3);
      dst->val(col, row, 2) = (sqrtf(2) * y - sqrtf(3) * u + v) / sqrtf(6);
    }
  }
}

inline int SymmetricCoordinate(int pos, int size) {
  if (pos < 0) pos = -pos - 1;
  if (pos >= 2 * size) pos %= 2 * size;
  if (pos >= size) pos = 2 * size - 1 - pos;
  return pos;
}

void PadSymmetric(const Image &src, int pad_before, int pad_after, Image *dst) {
  int pad_total = pad_before + pad_after;
  dst->Init(src.rows() + pad_total, src.columns() + pad_total, src.channels());
  for (int row = 0; row < dst->rows(); ++row) {
    for (int col = 0; col < dst->columns(); ++col) {
      for (int chan = 0; chan < dst->channels(); ++chan) {
        dst->val(col, row, chan) = src.val(SymmetricCoordinate(col - pad_before, src.columns()),
                                           SymmetricCoordinate(row - pad_before, src.rows()),
                                           chan);
      }
    }
  }
}

void ExtractPatch(const Image &src, int pr, int pc, Image *dst) {
  // src is padded, so (pr, pc) becomes the upper left pixel
  for (int row = 0; row < dst->rows(); ++row) {
    for (int col = 0; col < dst->columns(); ++col) {
      for (int chan = 0; chan < dst->channels(); ++chan) {
        dst->val(col, row, chan) = src.val(pc + col, pr + row, chan);
      }
    }
  }
}

void FastExtractPatch(const Image &src, int pr, int pc, Image *dst) {
  // src is padded, so (pr, pc) becomes the upper left pixel
  int i = 0, j = (pr * src.columns() + pc) * src.channels();
  for (int row = 0; row < dst->rows(); ++row, j += (src.columns() - dst->columns()) * src.channels()) {
    for (int el = 0; el < dst->columns() * dst->channels(); ++el, ++i, ++j) {
      dst->val(i) = src.val(j);
    }
  }
}

void BilateralWeight(const Image &g, Image *k, int r, float gamma_r_sigma2,
                     float sigma_s2) {
  for (int row = 0; row < g.rows(); ++row) {
    for (int col = 0; col < g.columns(); ++col) {
      float x = 0.f;
      for (int chan = 0; chan < g.channels(); ++chan) {
        float y = g.val(col, row, chan) - g.val(r, r, chan);
        x += y * y;
      }
      x /= gamma_r_sigma2;
      x += ((row - r) * (row - r) + (col - r) * (col - r)) / (2 * sigma_s2);
      k->val(col, row) = utils::fastexp(-x);
    }
  }
}

void ComputeRegressionPlane(const Image &y, const Image &g, const Image &k,
                            int r, float reg_plane[][2]) {
  float a = 0.f, b = 0.f, c = 0.f;
  for (int row = 0; row < y.rows(); ++row) {
    for (int col = 0; col < y.columns(); ++col) {
      a += (row - r) * (row - r) * k.val(col, row);
      b += (row - r) * (col - r) * k.val(col, row);
      c += (col - r) * (col - r) * k.val(col, row);
    }
  }
  float det = a * c - b * b;
  if (det == 0) {
    for (int chan = 0; chan < y.channels(); ++chan) {
      reg_plane[chan][0] = 0.f;
      reg_plane[chan][1] = 0.f;
    }
  } else {
    for (int chan = 0; chan < y.channels(); ++chan) {
      float d = 0.f, e = 0.f;
      float central = g.val(r, r, chan);
      for (int row = 0; row < y.rows(); ++row) {
        for (int col = 0; col < y.columns(); ++col) {
          d += (row - r) * (y.val(col, row, chan) - central) * k.val(col, row);
          e += (col - r) * (y.val(col, row, chan) - central) * k.val(col, row);
        }
      }
      // Solves the system
      // |a   b| |x1|   |d|
      // |     | |  | = | |
      // |b   c| |x2|   |e|
      reg_plane[chan][0] = (c * d - b * e) / det;
      reg_plane[chan][1] = (a * e - b * d) / det;
    }
  }
}

void SubtractPlane(int r, float reg_plane[][2], Image *y) {
  for (int row = 0; row < y->rows(); ++row) {
    for (int col = 0; col < y->columns(); ++col) {
      for (int chan = 0; chan < y->channels(); ++chan) {
        y->val(col, row, chan) -= reg_plane[chan][0] * (row - r) + reg_plane[chan][1] * (col - r);
      }
    }
  }
}

void AddPlane(int r, float reg_plane[][2], Image *y) {
  for (int row = 0; row < y->rows(); ++row) {
    for (int col = 0; col < y->columns(); ++col) {
      for (int chan = 0; chan < y->channels(); ++chan) {
        y->val(col, row, chan) += reg_plane[chan][0] * (row - r) + reg_plane[chan][1] * (col - r);
      }
    }
  }
}

void ModifyPatch(const Image &patch, const Image &k, DftPatch *modified, float *average) {
  // compute the total weight of the mask
  float weight = 0.;
  for (int row = 0; row < k.rows(); ++row) {
    for (int col = 0; col < k.columns(); ++col) {
      weight += k.val(col, row);
    }
  }

  for (int chan = 0; chan < patch.channels(); ++chan) {
    float avg = 0.f;
    for (int row = 0; row < patch.rows(); ++row) {
      for (int col = 0; col < patch.columns(); ++col) {
        avg += k.val(col, row) * patch.val(col, row, chan);
      }
    }
    avg /= weight;
    for (int row = 0; row < patch.rows(); ++row) {
      for (int col = 0; col < patch.columns(); ++col) {
        modified->space(col, row, chan)[0] = k.val(col, row) * patch.val(col, row, chan)
                                                                               + (1 - k.val(col, row)) * avg;
        modified->space(col, row, chan)[1] = 0.f;
      }
    }
    if (average) {
      average[chan] = avg;
    }
  }
}

void DA3D(const Image &noisy, const Image &guide, Image *output, float sigma,
          int nthreads, int r, float sigma_s, float gamma_r, float gamma_f,
          float threshold) {
  // padding and color transformation
  int s = utils::NextPowerOf2(2 * r + 1);
  Image guide_pad, noisy_pad;
  PadSymmetric(guide, r, s - r - 1, &guide_pad);
  PadSymmetric(noisy, r, s - r - 1, &noisy_pad);
  ColorTransform(guide_pad, &guide_pad);
  ColorTransform(noisy_pad, &noisy_pad);
  output->Init(guide.rows(), guide.columns(), guide.channels());
  output->Reset();  // line 2
  Image agg_weight(guide.rows(), guide.columns());
  agg_weight.Reset();

  if (!nthreads) nthreads = omp_get_max_threads();  // number of threads
#pragma omp parallel for num_threads(nthreads)
  for (int i = 0; i < nthreads; ++i) {
    Image out(guide.rows(), guide.columns(), guide.channels());
    out.Reset();
    Image weight(guide.rows(), guide.columns());
    weight.Reset();
    DA3D_strip(noisy_pad, guide_pad, guide, &out, &weight, sigma,
               (i * guide.rows()) / nthreads, ((i + 1) * guide.rows()) / nthreads,
               r, sigma_s, gamma_r, gamma_f, threshold);
#pragma omp critical
    for (int row = 0; row < guide.rows(); ++row) {
      for (int col = 0; col < guide.columns(); ++col) {
        agg_weight.val(col, row) += weight.val(col, row);
        for (int chan = 0; chan < guide.channels(); ++chan) {
          output->val(col, row, chan) += out.val(col, row, chan);
        }
      }
    }
  }
  for (int row = 0; row < guide.rows(); ++row) {
    for (int col = 0; col < guide.columns(); ++col) {
      for (int chan = 0; chan < guide.channels(); ++chan) {
        output->val(col, row, chan) /= agg_weight.val(col, row);
      }
    }
  }
  ColorTransformInverse(*output, output);
}

void DA3D_strip(const Image &noisy_pad, const Image &guide_pad,
                const Image &guide, Image *output, Image *weights, float sigma,
                int row_start, int row_end, int r, float sigma_s, float gamma_r,
                float gamma_f, float threshold) {
  // useful values
  int s = utils::NextPowerOf2(2 * r + 1);
  float sigma2 = sigma * sigma;
  float gamma_r_sigma2 = gamma_r * sigma2;
  float sigma_s2 = sigma_s * sigma_s;

  // regression parameters
  float gamma_rr_sigma2 = gamma_r_sigma2 * 10.f;
  float sigma_sr2 = sigma_s2 * 2.f;

  // declaration of internal variables
  Image y(s, s, guide.channels());
  Image g(s, s, guide.channels());
  Image k_reg(s, s);
  Image k(s, s);
  DftPatch y_m(s, s, guide.channels());
  DftPatch g_m(s, s, guide.channels());
  int pr, pc;  // coordinates of the central pixel
  float reg_plane[guide.channels()][2];  // parameters of the regression plane
  float yt[guide.channels()];  // weighted average of the patch
  WeightMap agg_weights(row_end - row_start, guide.columns());  // line 1

  // main loop
  while (agg_weights.Minimum() < threshold) {  // line 4
    agg_weights.FindMinimum(&pr, &pc);  // line 5
    pr += row_start;
    FastExtractPatch(noisy_pad, pr, pc, &y);  // line 6
    FastExtractPatch(guide_pad, pr, pc, &g);  // line 7
    BilateralWeight(g, &k_reg, r, gamma_rr_sigma2, sigma_sr2);  // line 8
    ComputeRegressionPlane(y, g, k_reg, r, reg_plane);  // line 9
    SubtractPlane(r, reg_plane, &y);  // line 10
    SubtractPlane(r, reg_plane, &g);  // line 11
    BilateralWeight(g, &k, r, gamma_r_sigma2, sigma_s2);  // line 12
    ModifyPatch(y, k, &y_m, yt);  // line 13
    ModifyPatch(g, k, &g_m);  // line 14
    y_m.ToFreq();  // line 15
    g_m.ToFreq();  // line 16
    float sigma_f2 = 0.f;
    for (int row = 0; row < k.rows(); ++row) {
      for (int col = 0; col < k.columns(); ++col) {
        sigma_f2 += k.val(col, row) * k.val(col, row);
      }
    }
    sigma_f2 *= sigma2;  // line 17
    for (int row = 0; row < y_m.rows(); ++row) {
      for (int col = 0; col < y_m.columns(); ++col) {
        for (int chan = 0; chan < y_m.channels(); ++chan) {
          if (row || col) {
            float G2 = g_m.freq(col, row, chan)[0] * g_m.freq(col, row, chan)[0]
                     + g_m.freq(col, row, chan)[1] * g_m.freq(col, row, chan)[1];
            float K = utils::fastexp(-gamma_f * sigma_f2 / G2);  // line 18
            y_m.freq(col, row, chan)[0] *= K;
            y_m.freq(col, row, chan)[1] *= K;
          }
        }
      }
    }
    y_m.ToSpace();  // line 19

    // lines 20,21,25
    // icol and irow are the "internal" indexes (with respect to the patch).
    // col and row are the indexes with respect to the entire image
    for (int row = max(0, pr - r), irow = row - pr + r; irow < s && row < output->rows(); ++row, ++irow) {
      for (int col = max(0, pc - r), icol = col - pc + r; icol < s && col < output->columns(); ++col, ++icol) {
        for (int chan = 0; chan < output->channels(); ++chan) {
          output->val(col, row, chan) += (y_m.space(icol, irow, chan)[0] +
              (reg_plane[chan][0] * (irow - r) + reg_plane[chan][1] * (icol - r))
              * k.val(icol, irow) -
              (1.f - k.val(icol, irow)) * yt[chan]) * k.val(icol, irow);
        }
        k.val(icol, irow) *= k.val(icol, irow);  // line 22
        weights->val(col, row) += k.val(icol, irow);
      }
    }
    agg_weights.IncreaseWeights(k, pr - row_start - r, pc - r);  // line 24
  }
}

}  // namespace da3d
