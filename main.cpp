/*
 * main.cc
 *
 *  Created on: 24/mar/2015
 *      Author: nicola
 */

#include <iostream>
#include <iterator>
#include "Image.hpp"
#include "Utils.hpp"
#include "DA3D.hpp"

using std::cerr;
using std::endl;
using std::min;
using std::max;
using std::copy;
using std::ostream_iterator;
using std::vector;
using std::string;

using utils::pick_option;
using utils::read_image;
using utils::save_image;

using da3d::Image;
using da3d::DA3D;

double mse(const Image &im1, const Image &im2) {
  double ans = 0.;
  for (auto it1 = im1.begin(), it2 = im2.begin(); it1 != im1.end(); ++it1, ++it2) {
    float v1 = min(max(*it1, 0.f), 255.f);
    float v2 = min(max(*it2, 0.f), 255.f);
    ans += (v1 - v2) * (v1 - v2);
  }
  ans /= im1.samples();
  return ans;
}

int main(int argc, char **argv) {
  bool usage = static_cast<bool>(pick_option(&argc, argv, "h", nullptr));
  if (usage || argc < 4) {
    cerr << "usage: " << argv[0] << " guide noise filenames" << endl;
    return EXIT_FAILURE;
  }

#ifndef _OPENMP
  cerr
      << "Warning: OpenMP not available. The algorithm will run in a single thread."
      << endl;
#endif

  float sigma = static_cast<float>(atof(argv[2]));
  vector<Image> input;
  vector<Image> guide;
  vector<Image> reference;
  for (int i = 3; i < argc; ++i) {
    input.push_back(read_image(
        string("noisy/") + argv[2] + "/" + argv[i] + ".tiff"));
    guide.push_back(read_image(
        string(argv[1]) + "/" + argv[2] + "/" + argv[i] + ".tiff"));
    reference.push_back(read_image(string("originals/") + argv[i] + ".png"));
  }
  std::vector<float> K_high = {0.f,  // 0
                               0.f,  // 0.25
                               0.f,  // 0.5
                               0.f,  // 0.75
                               0.f,  // 1
                               1.f,  // 1.25
                               1.f,  // 1.5
                               1.f,  // 1.75
                               1.f,  // 2
  };
  std::vector<float> K_low = {0.f,  // 0
                              0.17f,  // 0.25
                              0.17f,  // 0.5
                              0.27f,  // 0.75
                              0.49f,  // 1
                              0.63f,  // 1.25
                              0.73f,  // 1.5
                              0.83f,  // 1.75
                              1.f,  // 2
  };

  for (int j = 0; j < 5; ++j) {
//    for (float &v : K_high) {
//      float a = 0.f, b = 1.f, c;
//      v = a;
//      double msea = 0.;
//      for (unsigned k = 0; k < input.size(); ++k)
//        msea += mse(reference[k], DA3D(input[k], guide[k], sigma, K_high, K_low));
//      v = b;
//      double mseb = 0.;
//      for (unsigned k = 0; k < input.size(); ++k)
//        mseb += mse(reference[k], DA3D(input[k], guide[k], sigma, K_high, K_low));
//      double msec;
//      for (int i = 0; i < 5 + j; ++i) {
//        c = 0.5f * (a + b);
//        v = c;
//        msec = 0.;
//        for (unsigned k = 0; k < input.size(); ++k)
//          msec += mse(reference[k], DA3D(input[k], guide[k], sigma, K_high, K_low));
//        if (msea < mseb) {
//          b = c;
//          mseb = msec;
//        } else {
//          a = c;
//          msea = msec;
//        }
//      }
//      if (msea < msec) v = a;
//      if (mseb <= msec) v = b;
//      cerr << "High: ";
//      copy(K_high.begin(), K_high.end(), ostream_iterator<float>(cerr, " "));
//      cerr << " MSE " << msec << endl;
//    }
    for (float &v : K_low) {
      float a = 0.f, b = 1.f, c;
      v = a;
      double msea = 0.;
      for (unsigned k = 0; k < input.size(); ++k)
        msea += mse(reference[k], DA3D(input[k], guide[k], sigma, K_high, K_low));
      v = b;
      double mseb = 0.;
      for (unsigned k = 0; k < input.size(); ++k)
        mseb += mse(reference[k], DA3D(input[k], guide[k], sigma, K_high, K_low));
      double msec;
      for (int i = 0; i < 10 + j; ++i) {
        c = 0.5f * (a + b);
        v = c;
        msec = 0.;
        for (unsigned k = 0; k < input.size(); ++k)
          msec += mse(reference[k], DA3D(input[k], guide[k], sigma, K_high, K_low));
        if (msea < mseb) {
          b = c;
          mseb = msec;
        } else {
          a = c;
          msea = msec;
        }
      }
      if (msea < msec) v = a;
      if (mseb <= msec) v = b;
      cerr << "Low: ";
      copy(K_low.begin(), K_low.end(), ostream_iterator<float>(cerr, " "));
      cerr << " MSE " << msec << endl;
    }
  }

  for (unsigned k = 0; k < input.size(); ++k) {
    save_image(DA3D(input[k], guide[k], sigma, K_high, K_low), string(argv[1]) + "_da3d/" + argv[2] + "/" + argv[k + 3] + ".tiff");
  }
}
