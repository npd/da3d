/*
 * Utils.h
 *
 *  Created on: 23/mar/2015
 *      Author: nicola
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <cstring>
#include <cassert>

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

} /* namespace utils */

#endif /* SRC_UTILS_H_ */
