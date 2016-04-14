/*
 * WeightMap.cc
 *
 *  Created on: 12/feb/2015
 *      Author: nicola
 */

#include <cassert>
#include <limits>
#include <cstdlib>
#include <algorithm>
#include "WeightMap.hpp"
#include "Image.hpp"
#include "Utils.hpp"

using std::max;
using std::min;

namespace da3d {

WeightMap::WeightMap()
    : num_levels_(0), width_(0), height_(0),
      rows_(NULL), columns_(NULL), data_(NULL) {}

WeightMap::WeightMap(int rows, int columns) : data_(NULL) {
  Init(rows, columns);
}

void WeightMap::Init(int rows, int columns) {
  // memory allocation
  assert(data_ == NULL);
  int height_rows = utils::NumberOfBits(rows - 1) + 1;
  int height_columns = utils::NumberOfBits(columns - 1) + 1;
  num_levels_ = (height_rows > height_columns) ? height_rows : height_columns;
  width_ = columns;
  height_ = rows;
  rows_ = (int*)std::malloc(sizeof(int) * num_levels_);
  columns_ = (int*)std::malloc(sizeof(int) * num_levels_);
  data_ = (float**)std::malloc(sizeof(float*) * num_levels_);

  // initialization of layers
  int rows_rounded = utils::NextPowerOf2(rows);
  int cols_rounded = utils::NextPowerOf2(columns);
  for (int l = 0; l < num_levels_; ++l) {
    rows_[l] = rows_rounded;
    columns_[l] = cols_rounded;
    data_[l] = (float*)std::malloc(sizeof(float) * rows_rounded * cols_rounded);
    // zeros in the good area, MAXFLT elsewhere
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < columns; ++col) {
        val(col, row, l) = 0.f;
      }
      for (int col = columns; col < cols_rounded; ++col) {
        val(col, row, l) = std::numeric_limits<float>::infinity();
      }
    }
    for (int row = rows; row < rows_rounded; ++row) {
      for (int col = 0; col < cols_rounded; ++col) {
        val(col, row, l) = std::numeric_limits<float>::infinity();
      }
    }
    rows = (rows + 1) >> 1;  // it stays at least 1
    columns = (columns + 1) >> 1;
    rows_rounded = ((rows_rounded + 3) >> 2) << 1;  // it stays at least 2
    cols_rounded = ((cols_rounded + 3) >> 2) << 1;
  }
  assert(rows == 1);
  assert(columns == 1);
}

WeightMap::~WeightMap() {
  for (int l = 0; l < num_levels_; ++l) {
    std::free(data_[l]);
  }
  std::free(data_);
  std::free(rows_);
  std::free(columns_);
}

float WeightMap::Minimum() const {
  return val(0, 0, num_levels_ - 1);
}

void WeightMap::FindMinimum(int *row, int *col) const {
  *row = 0;
  *col = 0;
  for (int l = num_levels_ - 2; l >= 0; --l) {
    *row <<= 1;
    *col <<= 1;
    int r = *row, c = *col;
    float min = val(c, r, l);
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        if (val(c + i, r + j, l) < min) {
          min = val(c + i, r + j, l);
          *row = r + j;
          *col = c + i;
        }
      }
    }
  }
}

void WeightMap::IncreaseWeights(const Image &weights, int row0, int col0) {
  assert(weights.channels() == 1);
  for (int row = max(0, -row0); row < min(weights.rows(), height() - row0); ++row) {
    for (int col = max(0, -col0); col < min(weights.columns(), width() - col0); ++col) {
        val(col0 + col, row0 + row) += weights.val(col, row);
    }
  }
  // Updates the tree
  for (int l = 1; l < num_levels_; ++l) {
    // Updates the level l
    for (int row = max(0, row0) >> l; row <= (row0 + weights.rows() - 1) >> l && row <= (height() - 1) >> l; ++row) {
      for (int col = max(0, col0) >> l; col <= (col0 + weights.columns() - 1) >> l && col <= (width() - 1) >> l; ++col) {
        val(col, row, l) = val(col << 1, row << 1, l - 1);
        if (val(col, row, l) > val((col << 1) + 1, row << 1, l - 1))
          (val(col, row, l) = val((col << 1) + 1, row << 1, l - 1));
        if (val(col, row, l) > val((col << 1) + 1, (row << 1) + 1, l - 1))
          (val(col, row, l) = val((col << 1) + 1, (row << 1) + 1, l - 1));
        if (val(col, row, l) > val(col << 1, (row << 1) + 1, l - 1))
          (val(col, row, l) = val(col << 1, (row << 1) + 1, l - 1));
      }
    }
  }
}

}  // namespace da3d
