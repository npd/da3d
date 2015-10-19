/*
 * Image.h
 *
 *  Created on: 14/gen/2015
 *      Author: nicola
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <cassert>

namespace da3d {

class Image {
 public:
  Image();
  Image(int rows, int columns, int channels = 1);
//  Image(const Image&) = delete;
  ~Image();
  void Init(int rows, int columns, int channels = 1);
  void Reset();
  float val(int col, int row, int chan = 0) const;
  float& val(int col, int row, int chan = 0);
  float val(int pos) const;
  float& val(int pos);
  int channels() const { return channels_; }
  int columns() const { return columns_; }
  int rows() const { return rows_; }

 protected:
  int rows_;
  int columns_;
  int channels_;
  float* data_;
};

inline float Image::val(int col, int row, int chan) const {
  assert(col < columns_);
  assert(row < rows_);
  assert(chan < channels_);
  return data_[row * columns_ * channels_ + col * channels_ + chan];
}

inline float& Image::val(int col, int row, int chan) {
  assert(col < columns_);
  assert(row < rows_);
  assert(chan < channels_);
  return data_[row * columns_ * channels_ + col * channels_ + chan];
}

inline float Image::val(int pos) const {
  return data_[pos];
}

inline float& Image::val(int pos) {
  return data_[pos];
}


} /* namespace da3d */

#endif /* IMAGE_H_ */
