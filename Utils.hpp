/*
 * Utils.h
 *
 *  Created on: 23/mar/2015
 *      Author: nicola
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <cstring>
#include <string>
#include "Image.hpp"
#include "WeightMap.hpp"

namespace utils {

// number of bits used to represent n
inline int NumberOfBits(int n) {
  int ans = 0;
  while (n) {
    ans += 1;
    n >>= 1;
  }
  return ans;
}

// http://graphics.stanford.edu/~seander/bithacks.html
inline int NextPowerOf2(int n) {
  --n;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  ++n;
  return n;
}

inline float fastexp(float x) {
  int result = static_cast<int>(12102203 * x) + 1065353216;
  result *= result > 0;
  std::memcpy(&x, &result, sizeof(result));
  return x;
}

const char *pick_option(int *c, char **v, const char *o, const char *d);
da3d::Image read_image(const std::string &filename);
void save_image(const da3d::Image &image, const std::string &filename);

}  // namespace utils

#endif  // UTILS_HPP_
