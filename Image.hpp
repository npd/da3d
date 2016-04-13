/*
 * Image.hpp
 *
 *  Created on: 14/gen/2015
 *      Author: nicola
 */

#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <cassert>
#include <vector>

namespace da3d {

class Image {
 public:
  Image() = default;
  Image(int rows, int columns, int channels = 1, float val = 0.f);
  Image(float *data, int rows, int columns, int channels = 1);  // construct
                                                                // from C array

  Image(const Image&) = delete;  // disable copy constructor
  Image& operator=(const Image&) = delete;

  Image(Image&&) = default;  // default move constructor
  Image& operator=(Image&&) = default;

  void Clear(float val = 0.f) { std::fill(data_.begin(), data_.end(), val); }

  float val(int col, int row, int chan = 0) const;
  float& val(int col, int row, int chan = 0);
  float val(int pos) const;
  float& val(int pos);

  int channels() const { return channels_; }
  int columns() const { return columns_; }
  int rows() const { return rows_; }
  int pixels() const { return columns_ * rows_; }
  int samples() const { return channels_ * columns_ * rows_; }
  float* data() { return data_.data(); }
  const float* data() const { return data_.data(); }
  std::vector<float>::iterator begin() { return data_.begin(); }
  std::vector<float>::const_iterator begin() const { return data_.begin(); }
  std::vector<float>::iterator end() { return data_.end(); }
  std::vector<float>::const_iterator end() const { return data_.end(); }

 protected:
  int rows_{0};
  int columns_{0};
  int channels_{0};
  std::vector<float> data_;
};

inline Image::Image(int rows, int columns, int channels, float val)
    : rows_(rows), columns_(columns), channels_(channels),
      data_(rows * columns * channels, val) {}

inline Image::Image(float *data, int rows, int columns, int channels)
    : rows_(rows), columns_(columns), channels_(channels),
      data_(data, data + rows * columns * channels) {}


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

}  // namespace da3d

#endif  // IMAGE_HPP_
