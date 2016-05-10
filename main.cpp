/*
 * main.cc
 *
 *  Created on: 24/mar/2015
 *      Author: nicola
 */

#include <iostream>
#include <iterator>
#include "Image.hpp"
#include "Utils.hpp"
#include "DA3D.hpp"

using std::cerr;
using std::endl;
using std::min;
using std::max;
using std::copy;
using std::ostream_iterator;

using utils::pick_option;
using utils::read_image;
using utils::save_image;

using da3d::Image;
using da3d::DA3D;

double mse(const Image &im1, const Image &im2) {
  double ans = 0.;
  for (auto it1 = im1.begin(), it2 = im2.begin(); it1 != im1.end(); ++it1, ++it2) {
    float v1 = min(max(*it1, 0.f), 255.f);
    float v2 = min(max(*it2, 0.f), 255.f);
    ans += (v1 - v2) * (v1 - v2);
  }
  ans /= im1.samples();
  return ans;
}

int main(int argc, char **argv) {
  int nthreads = atoi(pick_option(&argc, argv, "nt", "0"));
  int r = atoi(pick_option(&argc, argv, "r", "31"));
  float sigma_s = static_cast<float>(atof(pick_option(&argc, argv, "sigma_s", "14")));
  float gamma_r = static_cast<float>(atof(pick_option(&argc, argv, "gamma_r", ".7")));
  float threshold = static_cast<float>(atof(pick_option(&argc, argv, "threshold", "2")));
  const char* optimize = pick_option(&argc, argv, "optimize", "");
  bool usage = static_cast<bool>(pick_option(&argc, argv, "h", nullptr));
  if (usage || argc < 4) {
    cerr << "usage: " << argv[0] << " input guide sigma [output]" << endl;
    return EXIT_FAILURE;
  }

#ifndef _OPENMP
  cerr << "Warning: OpenMP not available. The algorithm will run in a single thread." << endl;
#endif

  Image input = read_image(argv[1]);
  Image guide = read_image(argv[2]);
  float sigma = static_cast<float>(atof(argv[3]));
  std::vector<float> K_high = {0.f,  // 0
                               0.f,  // 0.25
                               0.25f,  // 0.5
                               0.5f,  // 0.75
                               0.75f,  // 1
                               0.1f,  // 1.25
                               1.f,  // 1.5
                               1.f,  // 1.75
                               1.f,  // 2
  };
  std::vector<float> K_low = {0.f,  // 0
                              0.f,  // 0.25
                              0.25f,  // 0.5
                              0.5f,  // 0.75
                              0.75f,  // 1
                              0.1f,  // 1.25
                              1.f,  // 1.5
                              1.f,  // 1.75
                              1.f,  // 2
  };
  if (strlen(optimize)) {
    Image reference = read_image(optimize);
    for (int j = 0; j < 10; ++j) {
      for (float &v : K_high) {
        float a = 0.f, b = 1.f, c;
        v = a;
        double msea = mse(reference, DA3D(input, guide, sigma, K_high, K_low));
        v = b;
        double mseb = mse(reference, DA3D(input, guide, sigma, K_high, K_low));
        double msec;
        for (int i = 0; i < 5 + j; ++i) {
          c = 0.5f * (a + b);
          v = c;
          msec = mse(reference, DA3D(input, guide, sigma, K_high, K_low));
          if (msea < mseb) {
            b = c;
            mseb = msec;
          } else {
            a = c;
            msea = msec;
          }
        }
        if (msea < msec) v = a;
        if (mseb <= msec) v = b;
        cerr << "High: ";
        copy(K_high.begin(), K_high.end(), ostream_iterator<float>(cerr, " "));
        cerr << " MSE " << msec << endl;
      }
      for (float &v : K_low) {
        float a = 0.f, b = 1.f, c;
        v = a;
        double msea = mse(reference, DA3D(input, guide, sigma, K_high, K_low));
        v = b;
        double mseb = mse(reference, DA3D(input, guide, sigma, K_high, K_low));
        double msec;
        for (int i = 0; i < 5 + j; ++i) {
          c = 0.5f * (a + b);
          v = c;
          msec = mse(reference, DA3D(input, guide, sigma, K_high, K_low));
          if (msea < mseb) {
            b = c;
            mseb = msec;
          } else {
            a = c;
            msea = msec;
          }
        }
        if (msea < msec) v = a;
        if (mseb <= msec) v = b;
        cerr << "Low: ";
        copy(K_low.begin(), K_low.end(), ostream_iterator<float>(cerr, " "));
        cerr << " MSE " << msec << endl;
      }
    }
  }
  Image output = DA3D(input, guide, sigma, K_high, K_low, nthreads, r, sigma_s,
                      gamma_r, threshold);
  save_image(output, argc > 4 ? argv[4] : "-");
}
