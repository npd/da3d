//
// Created by Nicola Pierazzo on 18/04/16.
//

#ifndef EIGENIMAGE_HPP_
#define EIGENIMAGE_HPP_

#include <Eigen/Core>
#include <utility>
#include "Image.hpp"

namespace da3d {

class EigenImage : public Image {
 public:
  using Image::Image;
  EigenImage(Image&& src) : Image(std::move(src)) {};
  Eigen::Map<Eigen::VectorXf> asvector() const { return vecmap_; };
  Eigen::Map<Eigen::VectorXf> asvector() { return vecmap_; };

 private:
  Eigen::Map<Eigen::VectorXf> vecmap_{data(), samples()};
};

}  // namespace da3d

#endif  // EIGENIMAGE_HPP_
