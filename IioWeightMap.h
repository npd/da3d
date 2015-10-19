/*
 * IioWeightMap.h
 *
 *  Created on: 25/mar/2015
 *      Author: nicola
 */

#ifndef SRC_IIOWEIGHTMAP_H_
#define SRC_IIOWEIGHTMAP_H_

#include "WeightMap.h"

namespace da3d {

class IioWeightMap : public WeightMap {
 public:
  IioWeightMap() : WeightMap() {}
  IioWeightMap(int rows, int columns) : WeightMap(rows, columns) {}
  void Save(char const* filename);
};

} /* namespace da3d */

#endif /* SRC_IIOWEIGHTMAP_H_ */
