#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "imagelib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const char *get_filetype(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}

void init_image(Image *image, int width, int height) {
  image->width = width;
  image->height = height;
  image->pixels = calloc(width, sizeof(uint8_t**));
  for (int x = 0; x < width; x++) {
    image->pixels[x] = calloc(height, sizeof(uint8_t*));
    for (int y = 0; y < height; y++) {
      image->pixels[x][y] = calloc(3, sizeof(uint8_t));
    }
  }
}

void load_image(char const *path, Image *image) {
  int width, height;
  int n; // number of channels given by the original format
  uint8_t *data = stbi_load(path, &width, &height, &n, 3);

  if (data == NULL) {
    puts("ERROR: Could not read image file");
    exit(EXIT_FAILURE);
  }

  /* printf("%d, %d\n", width, height); */

  init_image(image, width, height);

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      memcpy(
          image->pixels[x][y],
          &data[x * 3 + y * width * 3],
          3 * sizeof(uint8_t)
      );
    }
  }

  stbi_image_free(data);
}

void write_image(char const *path, Image *image) {
  const char *filetype = get_filetype(path);
  if (strcmp(filetype, "png") != 0 && strcmp(filetype, "bmp") != 0) {
    puts("ERROR: output filename should end in .png or .bmp");
    exit(EXIT_FAILURE);
  }

  int size = image->width * image->height * 3;
  uint8_t *data = calloc(size, sizeof(uint8_t));

  unsigned long index = 0;
  for (int y = 0; y < image->height; y++) {
    for (int x = 0; x < image->width; x++) {
      data[index++] = image->pixels[x][y][0];
      data[index++] = image->pixels[x][y][1];
      data[index++] = image->pixels[x][y][2];
    }
  }

  int result = 0;
  if (strcmp(filetype, "png") == 0) {
    result = stbi_write_png(path, image->width, image->height, 3, data, 0);
  } else if (strcmp(filetype, "bmp") == 0) {
    result = stbi_write_bmp(path, image->width, image->height, 3, data);
  }

  if (result == 0) {
    puts("ERROR: could not write data");
    exit(EXIT_FAILURE);
  }

  free(data);
}

void cp_image(Image *dest, Image *src) {
  for (int x = 0; x < dest->width; x++) {
    for (int y = 0; y < dest->height; y++) {
      memcpy(
          dest->pixels[x][y],
          src->pixels[x][y],
          3 * sizeof(uint8_t)
      );
    }
  }
}

// If we ask for a pixel outside of the image, return the pixel from the border
uint8_t *get_pixel(Image *image, int x, int y) {
  int x_ = x;
  int y_ = y;

  if (x < 0)
    x_ = 0;
  else if (x >= image->width)
    x_ = image->width - 1;

  if (y < 0)
    y_ = 0;
  else if (y >= image->height)
    y_ = image->height - 1;

  return image->pixels[x_][y_];
}

void free_image(Image *image) {
  for (int x = 0; x < image->width; x++) {
    for (int y = 0; y < image->height; y++) {
      free(image->pixels[x][y]);
    }
    free(image->pixels[x]);
  }
  free(image->pixels);
  free(image);
}
