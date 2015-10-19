/*
 * WeightMap.h
 *
 *  Created on: 12/feb/2015
 *      Author: nicola
 */

#ifndef WEIGHTMAP_H_
#define WEIGHTMAP_H_

#include "Image.h"

namespace da3d {

class WeightMap {
 public:
  WeightMap();
  WeightMap(int rows, int columns);
  ~WeightMap();
  void Init(int rows, int columns);
  float Minimum() const;
  void FindMinimum(int *row, int *col) const;
  void IncreaseWeights(const Image &weights, int row0, int col0);
  int width() const { return width_; }
  int height() const { return height_; }
  int num_levels() const { return num_levels_; }
  float val(int col, int row, int level = 0) const;
  float &val(int col, int row, int level = 0);
 protected:
  int num_levels_, width_, height_;
  int *rows_, *columns_;
  float **data_;
};

inline float WeightMap::val(int col, int row, int level) const {
  return data_[level][columns_[level] * row + col];
}

inline float &WeightMap::val(int col, int row, int level) {
  return data_[level][columns_[level] * row + col];
}

} /* namespace da3d */

#endif /* WEIGHTMAP_H_ */
