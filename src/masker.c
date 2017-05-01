#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#include "masker.h"

void init_mask(Mask *mask, int size, double value) {
  mask->size = size;
  mask->values = calloc(size, sizeof(double*));
  for (int x = 0; x < size; x++) {
    mask->values[x] = calloc(size, sizeof(double));
    if (value != 0) {
      for (int y = 0; y < size; y++) {
        mask->values[x][y] = value;
      }
    }
  }
}

void load_mask(char const *path, Mask *mask) {
  FILE *file;
  file = fopen(path, "r");
  int size;
  if (!fscanf(file, "%d", &size)) exit(1);
  
  init_mask(mask, size, 0);

  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      if (!fscanf(file, "%lf", &mask->values[x][y])) {
        fclose(file);
        exit(0);
      }
    }
  }
  fclose(file);
}

void apply_mask(Image *image, Image *result, Mask *mask) {
  /*
   * This variable will store the current sum for the mask.
   * Notice that this variable is necessary as the sum will
   * probably be bigger than 255, and we need a double in 
   * order to divide it later and get a higher precision.
   */
  double *current;

  int half = mask->size / 2;
  int size_squared = mask->size * mask->size;
  // Iterate through all the pixels.
  #pragma omp parallel private(current)
  {
    current = calloc(3, sizeof(double));
    #pragma omp for collapse(2)
    for (int x = 0; x < image->width; x++) {
      for (int y = 0; y < image->height; y++) {
        // Clear the current sum from last iteration.
        current[0] = 0.0;
        current[1] = 0.0;
        current[2] = 0.0;

        // Apply mask.
        for (int mx = -half; mx <= half; mx++) {
          for (int my = -half; my <= half; my++) {
            // Get the pixel.
            uint8_t *pixel = get_pixel(image, x + mx, y + my);
            // Get the multiplier.
            double maskval = mask->values[mx + half][my + half];
            // For each channel...
            for (int channel = 0; channel < 3; channel++) {
              current[channel] += pixel[channel] * maskval;
            }
          }
        }

        for (int channel = 0; channel < 3; channel++) {
          result->pixels[x][y][channel] = (uint8_t) round(current[channel] / size_squared);
        }
      }
    }
    free(current);
  }
}

void free_mask(Mask *mask) {
  for (int x = 0; x < mask->size; x++) {
    free(mask->values[x]);
  }
  free(mask->values);
  free(mask);
}
