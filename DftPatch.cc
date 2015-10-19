/*
 * DftPatch.cc
 *
 *  Created on: 11/feb/2015
 *      Author: nicola
 */

#include "DftPatch.h"

namespace da3d {

DftPatch::DftPatch(int rows, int columns, int channels)
: rows_(rows), columns_(columns), channels_(channels) {
  int N = rows * columns * channels;
  space_ = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N);
  freq_ = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N);
  int n[] = {columns, rows};
#pragma omp critical
  {
    plan_forward_ = fftwf_plan_many_dft(2, n, channels, space_, NULL, channels,
                                        1, freq_, NULL, channels, 1,
                                        FFTW_FORWARD, FFTW_ESTIMATE);
    plan_backward_ = fftwf_plan_many_dft(2, n, channels, freq_, NULL, channels,
                                         1, space_, NULL, channels, 1,
                                         FFTW_BACKWARD, FFTW_ESTIMATE);
  }
}

DftPatch::~DftPatch() {
  fftwf_free(space_);
  fftwf_free(freq_);
  fftwf_destroy_plan(plan_forward_);
  fftwf_destroy_plan(plan_backward_);
}

void DftPatch::ToFreq() {
  fftwf_execute(plan_forward_);
}

void DftPatch::ToSpace() {
  fftwf_execute(plan_backward_);
  for (int i = 0; i < rows_ * columns_ * channels_; ++i) {
    space_[i][0] /= rows_ * columns_;
  }
}

} /* namespace da3d */
