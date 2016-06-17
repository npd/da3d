/*
 * main.cc
 *
 *  Created on: 24/mar/2015
 *      Author: nicola
 */

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <tuple>
#include <algorithm>
#include <utility>
#include "Image.hpp"
#include "Utils.hpp"
#include "DA3D.hpp"

using std::cerr;
using std::endl;
using std::min;
using std::max;
using std::copy;
using std::ostream_iterator;
using std::istream_iterator;
using std::vector;
using std::string;
using std::ofstream;
using std::ifstream;
using std::istringstream;
using std::ostringstream;
using std::getline;
using std::move;
using std::pair;
using std::tie;

using utils::pick_option;
using utils::read_image;
using utils::save_image;

using da3d::Image;
using da3d::DA3D;

double l2(const Image &im1, const Image &im2) {
  double ans = 0.;
  for (auto it1 = im1.begin(), it2 = im2.begin(); it1 != im1.end(); ++it1, ++it2) {
    float v1 = min(max(*it1, 0.f), 255.f);
    float v2 = min(max(*it2, 0.f), 255.f);
    ans += (v1 - v2) * (v1 - v2);
  }
  return ans;
}

void save_lut(string filename, vector<float> h, vector<float> l) {
  ostringstream lut_h, lut_l;
  copy(h.begin(), h.end(), ostream_iterator<float>(lut_h, " "));
  copy(l.begin(), l.end(), ostream_iterator<float>(lut_l, " "));
  ofstream file(filename);
  file << lut_h.str() << endl << lut_l.str() << endl;
  file.close();
}

pair<vector<float>, vector<float>> load_lut(string filename) {
  string s_h, s_l;
  ifstream file(filename);
  getline(file, s_h);
  getline(file, s_l);
  file.close();
  istringstream lut_h(s_h), lut_l(s_l);
  vector<float> h(istream_iterator<float>(lut_h), {});
  vector<float> l(istream_iterator<float>(lut_l), {});
  return {move(h), move(l)};
}

int main(int argc, char **argv) {
  bool usage = static_cast<bool>(pick_option(&argc, argv, "h", nullptr));
  bool only_psnr = static_cast<bool>(pick_option(&argc, argv, "m", nullptr));
  const char* lut = pick_option(&argc, argv, "lut", "");
  bool optimize = static_cast<bool>(pick_option(&argc, argv, "o", nullptr));
  if (usage || argc < 3 || (!optimize && argc < 4) || (optimize && !strlen(lut))) {
    cerr << "usage: " << argv[0] << " noisy guide sigma output [-lut file]" <<
        endl;
    cerr << "       " << argv[0] <<
        " algorithm sigma files -o -lut file [-m]" << endl;
    return EXIT_FAILURE;
  }

#ifndef _OPENMP
  cerr << "Warning: OpenMP not available. The algorithm will run in a single" <<
      " thread." << endl;
#endif

  vector<float> K_high, K_low;
  if (strlen(lut)) tie(K_high, K_low) = load_lut(lut);

  if (optimize) {
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

    if (only_psnr) {
      double m = 0.;
      for (unsigned k = 0; k < input.size(); ++k)
        m += l2(reference[k], DA3D(input[k], guide[k], sigma, K_high, K_low));
      cerr << "Error " << m << endl;
    } else {
      for (int j = 0; j < 2; ++j) {
        for (float &v : K_high) {
          float a = 0.f, b = 1.f, c;
          v = a;
          double msea = 0.;
          for (unsigned k = 0; k < input.size(); ++k)
            msea += l2(reference[k], DA3D(input[k], guide[k], sigma, K_high,
                                          K_low));
          v = b;
          double mseb = 0.;
          for (unsigned k = 0; k < input.size(); ++k)
            mseb += l2(reference[k], DA3D(input[k], guide[k], sigma, K_high,
                                          K_low));
          double msec;
          for (int i = 0; i < 5 + j; ++i) {
            c = 0.5f * (a + b);
            v = c;
            msec = 0.;
            for (unsigned k = 0; k < input.size(); ++k)
              msec += l2(reference[k], DA3D(input[k], guide[k], sigma, K_high,
                                            K_low));
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
          cerr << "High: ";
          copy(K_high.begin(), K_high.end(), ostream_iterator<float>(cerr, " "));
          cerr << " Error " << min({msea, mseb, msec}) << endl;
        }
        save_lut(lut, K_high, K_low);
        for (float &v : K_low) {
          float a = 0.f, b = 1.f, c;
          v = a;
          double msea = 0.;
          for (unsigned k = 0; k < input.size(); ++k)
            msea +=
                l2(reference[k], DA3D(input[k], guide[k], sigma, K_high, K_low));
          v = b;
          double mseb = 0.;
          for (unsigned k = 0; k < input.size(); ++k)
            mseb +=
                l2(reference[k], DA3D(input[k], guide[k], sigma, K_high, K_low));
          double msec;
          for (int i = 0; i < 10; ++i) {
            c = 0.5f * (a + b);
            v = c;
            msec = 0.;
            for (unsigned k = 0; k < input.size(); ++k)
              msec += l2(reference[k],
                         DA3D(input[k], guide[k], sigma, K_high, K_low));
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
          cerr << " Error " << min({msea, mseb, msec}) << endl;
        }
        save_lut(lut, K_high, K_low);
      }
    }
  } else {
    Image input = read_image(argv[1]);
    Image guide = read_image(argv[2]);
    float sigma = atof(argv[3]);
    Image output = DA3D(input, guide, sigma, K_high, K_low, strlen(lut));
    save_image(output, argc > 4 ? argv[4] : "-");
  }
}
