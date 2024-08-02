#ifndef A_BITMAP_H
#define A_BITMAP_H

#include <stdint.h>

typedef struct {
  unsigned char *data;
  size_t size;
  uint32_t width;
  uint32_t height;
  uint32_t bpp;
  uint32_t stride;
} bitmap_t;

bitmap_t *A_Bitmap_ReadFile(const char *path);
void A_Bitmap_Free(bitmap_t *bitmap);

#endif
