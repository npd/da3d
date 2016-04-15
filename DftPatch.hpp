/*
 * DftPatch.h
 *
 *  Created on: 11/feb/2015
 *      Author: nicola
 */

#ifndef DFTPATCH_HPP_
#define DFTPATCH_HPP_

#include <fftw3.h>

namespace da3d {

class DftPatch {
 public:
  DftPatch(int rows, int columns, int channels = 1);
  ~DftPatch();
  void ToFreq();
  void ToSpace();
  int rows() const { return rows_; }
  int columns() const { return columns_; }
  int channels() const { return channels_; }
  fftwf_complex& space(int col, int row, int chan = 0);
  fftwf_complex& freq(int col, int row, int chan = 0);

 private:
  fftwf_complex *space_;
  fftwf_complex *freq_;
  fftwf_plan plan_forward_;
  fftwf_plan plan_backward_;
  int rows_, columns_, channels_;
};

inline fftwf_complex& DftPatch::space(int col, int row, int chan) {
  return space_[row * columns_ * channels_ + col * channels_ + chan];
}

inline fftwf_complex& DftPatch::freq(int col, int row, int chan) {
  return freq_[row * columns_ * channels_ + col * channels_ + chan];
}

inline DftPatch::DftPatch(int rows, int columns, int channels)
    : rows_(rows), columns_(columns), channels_(channels) {
  int N = rows * columns * channels;
  space_ = reinterpret_cast<fftwf_complex *>(fftwf_malloc(
      sizeof(fftwf_complex) * N));
  freq_ = reinterpret_cast<fftwf_complex *>(fftwf_malloc(
      sizeof(fftwf_complex) * N));
  int n[] = {columns, rows};
#pragma omp critical
  {
    plan_forward_ = fftwf_plan_many_dft(2, n, channels, space_, NULL, channels,
                                        1, freq_, NULL, channels, 1,
                                        FFTW_FORWARD, FFTW_MEASURE);
    plan_backward_ = fftwf_plan_many_dft(2, n, channels, freq_, NULL, channels,
                                         1, space_, NULL, channels, 1,
                                         FFTW_BACKWARD, FFTW_MEASURE);
  }
}

inline DftPatch::~DftPatch() {
  fftwf_free(space_);
  fftwf_free(freq_);
  fftwf_destroy_plan(plan_forward_);
  fftwf_destroy_plan(plan_backward_);
}

inline void DftPatch::ToFreq() {
  fftwf_execute(plan_forward_);
}

inline void DftPatch::ToSpace() {
  fftwf_execute(plan_backward_);
  for (int i = 0; i < rows_ * columns_ * channels_; ++i) {
    space_[i][0] /= rows_ * columns_;
  }
}

} /* namespace da3d */

#endif /* DFTPATCH_HPP_ */
