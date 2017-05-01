#pragma once
#include "imagelib/imagelib.h"

typedef struct {
  int size;
  double **values;
} Mask;

void init_mask(Mask *mask, int size, double value);
void load_mask(char const *path, Mask *mask);
void apply_mask(Image *image, Image *result, Mask *mask);
void free_mask(Mask *mask);
