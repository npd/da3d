/*
 * DftPatch.h
 *
 *  Created on: 11/feb/2015
 *      Author: nicola
 */

#ifndef DFTPATCH_H_
#define DFTPATCH_H_

#include <fftw3.h>
#include "Image.h"

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

} /* namespace da3d */

#endif /* DFTPATCH_H_ */
