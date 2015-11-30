/*
 * main.cc
 *
 *  Created on: 24/mar/2015
 *      Author: nicola
 */

#include "Image.h"
#include "IioImage.h"
#include "DA3D.h"
#include <string.h>
#include <cstdlib>

using da3d::DA3D;
using da3d::IioImage;


// c: pointer to original argc
// v: pointer to original argv
// o: option name after hyphen
// d: default value (if NULL, the option takes no argument)
const char *pick_option(int *c, char **v, const char *o, const char *d) {
  int id = d ? 1 : 0;
  for (int i = 0; i < *c - id; i++) {
    if (v[i][0] == '-' && 0 == strcmp(v[i] + 1, o)) {
      char *r = v[i + id] + 1 - id;
      for (int j = i; j < *c - id; j++)
        v[j] = v[j + id + 1];
      *c -= id + 1;
      return r;
    }
  }
  return d;
}

int main(int argc, char **argv) {
  int nthreads = atoi(pick_option(&argc, argv, "nt", "0"));
  int r = atoi(pick_option(&argc, argv, "r", "31"));
  float sigma_s = atof(pick_option(&argc, argv, "sigma_s", "14"));
  float gamma_r = atof(pick_option(&argc, argv, "gamma_r", ".7"));
  float gamma_f = atof(pick_option(&argc, argv, "gamma_f", ".8"));
  float threshold = atof(pick_option(&argc, argv, "threshold", "2"));
  if (argc < 5) {
    fprintf(stderr, "too few parameters\n");
    fprintf(stderr, "usage: %s input guide sigma output\n", argv[0]);
    return 1;
  }
  IioImage input(argv[1]);
  IioImage guide(argv[2]);
  IioImage output;
  float sigma = atof(argv[3]);
  DA3D(input, guide, &output, sigma, nthreads, r, sigma_s, gamma_r, gamma_f,
       threshold);
  output.Save(argv[4]);
}
