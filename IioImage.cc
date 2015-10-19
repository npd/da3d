/*
 * IioImage.cpp
 *
 *  Created on: 10/feb/2015
 *      Author: nicola
 */

#include "IioImage.h"

extern "C" {
#include "iio.h"
}

namespace da3d {

IioImage::IioImage(char const* filename) {
  data_ = iio_read_image_float_vec(const_cast<char*>(filename), &columns_,
                                   &rows_, &channels_);
}

void IioImage::Save(char const* filename) {
  iio_save_image_float_vec(const_cast<char*>(filename), data_, columns_, rows_,
                           channels_);
}

} /* namespace da3d */
