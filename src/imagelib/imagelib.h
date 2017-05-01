#pragma once
#include <stdint.h>

typedef struct {
  int width; // The width of the image in pixels
  int height; // The height of the image in pixels
  uint8_t ***pixels; // Pixel data
} Image;

void init_image(Image *image, int width, int height);

/* 
 * Loads an image located in "path". Each pixel contains red, green and blue components (RGB).
 */
void load_image(char const *path, Image *image);

/*
 * Receives a struct of type Image, and writes it in "path".
 * "path" should end with .png or .bmp (i.e. "../hello.png").
 */
void write_image(char const *path, Image *image);

void cp_image(Image *dest, Image *src);

uint8_t *get_pixel(Image *image, int x, int y);

void free_image(Image *image);
