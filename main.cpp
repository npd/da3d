/*
 * main.cc
 *
 *  Created on: 24/mar/2015
 *      Author: nicola
 */

#include <iostream>
#include "Image.hpp"
#include "Utils.hpp"
#include "DA3D.hpp"

using std::cerr;
using std::endl;

using utils::pick_option;
using utils::read_image;
using utils::save_image;

using da3d::Image;
using da3d::DA3D;

int main(int argc, char **argv) {
  int nthreads = atoi(pick_option(&argc, argv, "nt", "0"));
  int r = atoi(pick_option(&argc, argv, "r", "31"));
  float sigma_s = atof(pick_option(&argc, argv, "sigma_s", "14"));
  float gamma_r = atof(pick_option(&argc, argv, "gamma_r", ".7"));
  float gamma_f = atof(pick_option(&argc, argv, "gamma_f", ".8"));
  float threshold = atof(pick_option(&argc, argv, "threshold", "2"));
  bool usage = pick_option(&argc, argv, "h", nullptr);
  if (usage || argc < 4) {
    cerr << "usage: " << argv[0] << " input guide sigma [output]" << endl;
    return EXIT_FAILURE;
  }
  Image input = read_image(argv[1]);
  Image guide = read_image(argv[2]);
  Image output;
  float sigma = atof(argv[3]);
  DA3D(input, guide, &output, sigma, nthreads, r, sigma_s, gamma_r, gamma_f,
       threshold);
  save_image(output, argc > 4 ? argv[4] : "-");
}
