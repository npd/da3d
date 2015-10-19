/*
 * IioWeightMap.cc
 *
 *  Created on: 25/mar/2015
 *      Author: nicola
 */

#include "IioWeightMap.h"

extern "C" {
#include "iio.h"
}

namespace da3d {

void IioWeightMap::Save(const char* filename) {
  iio_save_image_float_vec(const_cast<char*>(filename), data_[0], columns_[0],
                           rows_[0], 1);
}

} /* namespace da3d */
