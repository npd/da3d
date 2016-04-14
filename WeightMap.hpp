/*
 * WeightMap.h
 *
 *  Created on: 12/feb/2015
 *      Author: nicola
 */

#ifndef WEIGHTMAP_HPP_
#define WEIGHTMAP_HPP_

#include <cassert>

namespace da3d {

class Image;

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
  const float* data() const { return data_[0]; }
 private:
  int num_levels_, width_, height_;
  int *rows_, *columns_;
  float **data_;
};

inline float WeightMap::val(int col, int row, int level) const {
  assert (0 <= level && level < num_levels_);
  assert (0 <= col && col < columns_[level]);
  assert (0 <= row && row < rows_[level]);
  return data_[level][columns_[level] * row + col];
}

inline float &WeightMap::val(int col, int row, int level) {
  assert (0 <= level && level < num_levels_);
  assert (0 <= col && col < columns_[level]);
  assert (0 <= row && row < rows_[level]);
  return data_[level][columns_[level] * row + col];
}

}  // namespace da3d

#endif  // WEIGHTMAP_HPP_
