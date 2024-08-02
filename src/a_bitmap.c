#include <stdio.h>
#include <stdlib.h>

#include "a_bitmap.h"
#include "m_message.h"

typedef struct {
  uint16_t type;
  uint32_t size;
  uint16_t _reserved_1;
  uint16_t _reserved_2;
  uint32_t offset;
} __attribute__((packed)) bitmap_file_header_t;

typedef struct {
  uint32_t size;
  uint32_t width;
  uint32_t height;
  uint16_t planes;
  uint16_t bpp;
  uint32_t compression;
  uint32_t data_size;
  int32_t ppm_width;
  int32_t ppm_height;
  uint32_t palette_count;
  uint32_t palette_important_count;
} __attribute__((packed)) bitmap_file_info_header_t;

bitmap_t *A_Bitmap_ReadFile(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    M_Log("[A_Bitmap] Failed to open file: %s\n", path);
    return NULL;
  }

  bitmap_file_header_t file_header;
  fread(&file_header, sizeof(bitmap_file_header_t), 1, file);
  if (ferror(file)) {
    fclose(file);
    M_Log("[A_Bitmap] Read file header failed\n");
    return NULL;
  }

  if (file_header.type != 0x4D42) {
    // Not a BM bitmap.
    fclose(file);
    M_Log("[A_Bitmap] Not a BM bitmap\n");
    return NULL;
  }

  bitmap_file_info_header_t info_header;
  fread(&info_header, sizeof(bitmap_file_info_header_t), 1, file);
  if (ferror(file)) {
    fclose(file);
    M_Log("[A_Bitmap] Read info header failed\n");
    return NULL;
  }

  if (info_header.compression != 0) {
    fclose(file);
    M_Log("[A_Bitmap] Compression is not supported\n");
    return NULL;
  }

  if (fseek(file, file_header.offset, SEEK_SET) != 0) {
    fclose(file);
    M_Log("[A_Bitmap] File seeking failed\n");
    return NULL;
  }

  unsigned char *data = malloc(info_header.data_size);
  if (data == NULL) {
    fclose(file);
    M_Log("[A_Bitmap] Allocating data failed\n");
    return NULL;
  }

  fread(data, 1, info_header.data_size, file);
  if (ferror(file)) {
    free(data);
    fclose(file);
    M_Log("[A_Bitmap] Read bitmap data failed\n");
    return NULL;
  }

  fclose(file);

  bitmap_t *bitmap = malloc(sizeof(bitmap_t));
  if (bitmap == NULL) {
    free(data);
    M_Log("[A_Bitmap] Allocating result failed\n");
    return NULL;
  }

  bitmap->data = data;
  bitmap->size = info_header.data_size;
  bitmap->width = info_header.width;
  bitmap->height = info_header.height;
  bitmap->bpp = info_header.bpp;
  uint32_t bytes_per_row = (info_header.width + 7) / 8;
  bitmap->stride = (bytes_per_row + 3) & ~3;

  return bitmap;
}

void A_Bitmap_Free(bitmap_t *bitmap) {
  if (bitmap != NULL) {
    if (bitmap->data) {
      free(bitmap->data);
      bitmap->data = NULL;
    }
  }
}
