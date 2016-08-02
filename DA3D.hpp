/*
 * da3d.h
 *
 *  Created on: 24/mar/2015
 *      Author: nicola
 */

#ifndef DA3D_DA3D_HPP_
#define DA3D_DA3D_HPP_

#include "Image.hpp"

namespace da3d {

Image DA3D(const Image &noisy, const Image &guide, float sigma,
           const std::vector<float> &K_high, const std::vector<float> &K_low,
           bool use_lut = true, int nthreads = 0, int r = 31,
           float sigma_s = 14.f, float gamma_r = .7f, float threshold = 2.f);

}  // namespace da3d

#endif  // DA3D_DA3D_HPP_
