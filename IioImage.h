/*
 * IioImage.h
 *
 *  Created on: 10/feb/2015
 *      Author: nicola
 */

#ifndef IIOIMAGE_H_
#define IIOIMAGE_H_

#include "Image.h"

namespace da3d {

class IioImage : public Image {
 public:
  IioImage() : Image() {}
  IioImage(int rows, int columns, int channels = 1) : Image(rows, columns, channels) {}
  IioImage(char const* filename);
  void Save(char const* filename);
};

} /* namespace da3d */

#endif /* IIOIMAGE_H_ */
