#ifndef A_FONT_H
#define A_FONT_H

#define GLYPHS_COUNT 256

typedef struct {
  uint32_t x;
  uint32_t y;
} font_glyph_t;

typedef struct {
  unsigned char *pixel_data;
  uint32_t pixel_stride;
  uint32_t glyph_width;
  uint32_t glyph_height;
  font_glyph_t glyphs[GLYPHS_COUNT];
} font_t;

font_t *A_Font_ReadConfig(const char *path);
uint8_t A_Font_Pixel(const font_t *font, uint32_t x, uint32_t y);
void A_Font_Free(font_t *font);

#endif
