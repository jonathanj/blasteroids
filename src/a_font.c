#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "a_font.h"
#include "a_bitmap.h"
#include "m_message.h"


#define BUF_MAX_SIZE 256
#define LINE_MAX_SIZE 256

void A_Font_Copy_Glyph_Bitmap1(
  const font_t *font,
  const bitmap_t *bitmap,
  uint8_t *pixel_data,
  const font_glyph_t *glyph
) {
  SDL_assert(bitmap->bpp == 1);

  for (uint32_t offset_y = 0; offset_y < font->glyph_height; ++offset_y) {
    uint32_t y = glyph->y + offset_y;
    uint32_t inverted_y = (bitmap->height - 1 - y);
    for (uint32_t offset_x = 0; offset_x < font->glyph_width; ++offset_x) {
      uint32_t x = glyph->x + offset_x;
      uint32_t byte_index = (inverted_y * bitmap->stride) + (x / 8);
      uint8_t bit_index = 7 - (x % 8);
      uint8_t pixel = (bitmap->data[byte_index] >> bit_index) & 1;
      // TODO: inverted
      if (!pixel) {
        uint32_t pixel_index = y * bitmap->width + x;
        pixel_data[pixel_index] = 0xFF;
      }
    }
  }
}

typedef enum {
  PARSING_MODE_DESCRIPTORS,
  PARSING_MODE_GLYPHS
} config_parsing_mode_t;

font_t *A_Font_ReadConfig(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    M_Log("[Asset/Font] Could not open config file\n");
    return NULL;
  }

  uint32_t glyph_width = 0;
  uint32_t glyph_height = 0;
  font_glyph_t glyphs[GLYPHS_COUNT];
  memset(glyphs, 0, sizeof(glyphs));

  config_parsing_mode_t mode = PARSING_MODE_DESCRIPTORS;

  char key[BUF_MAX_SIZE];
  char value[BUF_MAX_SIZE];
  char line[LINE_MAX_SIZE];
  bitmap_t *bitmap = NULL;
  while (fgets(line, LINE_MAX_SIZE, file)) {
    if (strncmp(line, "\r\n", 2) == 0 || strncmp(line, "\n", 1) == 0) {
      mode = PARSING_MODE_GLYPHS;
      continue;
    }

    switch (mode) {
      case PARSING_MODE_DESCRIPTORS: {
        if (sscanf(line, "%s %s", key, value) == -1) {
          M_Log("[Asset/Font] Could not parse line\n");
          return NULL;
        }

        if (strncmp(key, "bitmap", BUF_MAX_SIZE) == 0) {
          // FIXME: This might be a 1bpp or something, which we want to rather normalize.
          bitmap = A_Bitmap_ReadFile(value);
          if (bitmap == NULL) {
            M_Log("[Asset/Font] Could not load bitmap\n");
            return NULL;
          }
        } else if (strncmp(key, "glyph_width", BUF_MAX_SIZE) == 0) {
          sscanf(value, "%u", &glyph_width);
        } else if (strncmp(key, "glyph_height", BUF_MAX_SIZE) == 0) {
          sscanf(value, "%u", &glyph_height);
        }
        break;
      }
      case PARSING_MODE_GLYPHS: {
        uint32_t ch = -1;
        uint32_t x = -1;
        uint32_t y = -1;

        if (sscanf(line, "%u %u %u", &ch, &x, &y) != -1) {
          SDL_assert(ch < 256);
          SDL_assert(x < bitmap->width);
          SDL_assert(y < bitmap->height);
          glyphs[ch].x = x;
          glyphs[ch].y = y;
        }
        break;
      }
    }
  }

  if (bitmap == NULL || glyph_height == 0 || glyph_width == 0) {
    M_Log("[Asset/Font] Invalid font config\n");
    if (bitmap != NULL) {
      A_Bitmap_Free(bitmap);
    }
    return NULL;
  }

  // Extract glyphs from bitmap and put them into our own array.
  size_t pixel_data_size = bitmap->width * bitmap->height * sizeof(uint8_t);
  unsigned char *pixel_data = malloc(pixel_data_size);
  memset(pixel_data, 0, pixel_data_size);

  if (pixel_data == NULL) {
    M_Log("[Asset/Font] Could not allocate pixel data\n");
    return NULL;
  }

  font_t *font = malloc(sizeof(font_t));
  if (font == NULL) {
    M_Log("[Asset/Font] Could not allocate font_t\n");
    return NULL;
  }
  font->pixel_data = pixel_data;
  font->pixel_stride = bitmap->width;
  font->glyph_width = glyph_width;
  font->glyph_height = glyph_height;
  for (size_t ch = 1; ch < GLYPHS_COUNT; ++ch) {
    font_glyph_t glyph = glyphs[ch];
    font->glyphs[ch].x = glyph.x;
    font->glyphs[ch].y = glyph.y;
    switch (bitmap->bpp) {
      case 1:
        A_Font_Copy_Glyph_Bitmap1(font, bitmap, pixel_data, &glyph);
        break;
      default:
        SDL_assert_always(0);
        break;
    }
  }

  A_Bitmap_Free(bitmap);

  return font;
}

uint8_t A_Font_Pixel(const font_t *font, uint32_t x, uint32_t y) {
  uint32_t index = y * font->pixel_stride + x;
  return font->pixel_data[index];
}

void A_Font_Free(font_t *font) {
  if (font != NULL) {
    if (font->pixel_data != NULL) {
      free(font->pixel_data);
    }

    free(font);
  }
}
