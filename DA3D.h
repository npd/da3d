/*
 * da3d.h
 *
 *  Created on: 24/mar/2015
 *      Author: nicola
 */

#ifndef SRC_DA3D_H_
#define SRC_DA3D_H_

#include "Image.h"
#include "DftPatch.h"

namespace da3d {

void ColorTransform(const Image &src, Image *dst);
void ColorTransformInverse(const Image &src, Image *dst);
void PadSymmetric(const Image &src, int pad_before, int pad_after, Image *dst);
void ExtractPatch(const Image &src, int pr, int pc, Image *dst);
void BilateralWeight(const Image &g, Image *k, int r, float gamma_r_sigma2,
                     float sigma_s2);
void ComputeRegressionPlane(const Image &y, const Image &k, int r,
                            float reg_plane[][2]);
void SubtractPlane(int r, float reg_plane[][2], Image *y);
void AddPlane(int r, float reg_plane[][2], Image *y);
void ModifyPatch(const Image &patch, const Image &k, DftPatch *modified,
                 float *average = NULL);
void DA3D(const Image &noisy, const Image &guide, Image *output, float sigma,
          int nthreads = 0, int r = 31, float sigma_s = 14.f,
          float gamma_r = .7f, float gamma_f = .8f, float threshold = 2.f);
void DA3D_strip(const Image &noisy_pad, const Image &guide_pad,
                const Image &guide, Image *output, Image *weights, float sigma,
                int row_start, int row_end, int r, float sigma_s, float gamma_r,
                float gamma_f, float threshold);
}  // namespace da3d

#endif /* SRC_DA3D_H_ */
