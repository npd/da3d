/*
 * Image.c
 *
 *  Created on: 10/feb/2015
 *      Author: nicola
 */

#include "Image.h"
#include <cstdlib>
#include <cstddef>
#include <cassert>

namespace da3d {

Image::Image() : rows_(0), columns_(0), channels_(0), data_(NULL) {}

Image::Image(int rows, int columns, int channels) : data_(NULL) {
  Init(rows, columns, channels);
}

Image::~Image() {
  std::free(data_);
}

void Image::Init(int rows, int columns, int channels) {
  assert(data_ == NULL);
  rows_ = rows;
  columns_ = columns;
  channels_ = channels;
  data_ = (float*) std::malloc(rows * columns * channels * sizeof(float));
}

void Image::Reset() {
  for (int i = 0; i < rows_ * columns_ * channels_; ++i) {
    data_[i] = 0.f;
  }
}

}  // namespace da3d
