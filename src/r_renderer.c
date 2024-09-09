#include <stdarg.h>
#include <stdbool.h>
#include <math.h>

#include "r_renderer.h"
#include "r_particles.h"
#include "m_message.h"
#include "m_util.h"
#include "m_noise.h"
#include "a_bitmap.h"
#include "a_font.h"

#include "p_entity.h"

SDL_Renderer *sdl_renderer = NULL;
SDL_Texture *framebuffer_texture = NULL;
uint32_t *framebuffer = NULL;
size_t framebuffer_size = 0;

int32_t screen_width, screen_height;

bool R_InitScreen(int32_t width, int32_t height) {
  screen_width = width;
  screen_height = height;
  framebuffer_size = (uint32_t)screen_width * (uint32_t)screen_height * sizeof(uint32_t);
  framebuffer = malloc(framebuffer_size);
  if (framebuffer == NULL) {
    M_Log("[Renderer] Framebuffer could not be allocated\n");
    return false;
  }

  framebuffer_texture = SDL_CreateTexture(
    sdl_renderer,
    SDL_PIXELFORMAT_RGBA32,
    SDL_TEXTUREACCESS_STREAMING,
    screen_width,
    screen_height
  );
  if (framebuffer_texture == NULL) {
    M_Log("[Renderer] Framebuffer texture could not be created: %s\n", SDL_GetError());
    return false;
  }

  return true;
}

font_t *small_font = NULL;
font_t *large_font = NULL;

bool R_LoadFonts() {
  large_font = A_Font_ReadConfig("assets/monogram_6x10.conf");
  if (large_font == NULL) {
    M_Log("[Renderer] Failed to load large font\n");
    return false;
  }

  small_font = A_Font_ReadConfig("assets/tiny_4x6.conf");
  if (small_font == NULL) {
    M_Log("[Renderer] Failed to load small font\n");
    return false;
  }
  return true;
}

// XXX
void xxx_gen_noise();

bool R_Init(SDL_Window *window, const game_state_t *game_state) {
  sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  if (sdl_renderer == NULL) {
    M_Log("[Renderer] SDL renderer could not be created: %s\n", SDL_GetError());
    return false;
  }

  uint32_t render_width = game_state->screen_width;
  uint32_t render_height = game_state->screen_height;

  if (!R_InitScreen(render_width, render_height)) {
    return false;
  }
  SDL_RenderSetLogicalSize(sdl_renderer, render_width, render_height);
  SDL_SetTextureBlendMode(framebuffer_texture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);

  if (!R_LoadFonts()) {
    return false;
  }

  M_Log("[Renderer] Initialized %dx%d\n", render_width, render_height);

  // XXX:
  initPermutation();
  xxx_gen_noise();

  return true;
}

void R_Shutdown() {
  M_Log("[Renderer] Destroying\n");
  if (large_font != NULL) {
    A_Font_Free(large_font);
  }
  if (small_font != NULL) {
    A_Font_Free(small_font);
  }
  if (framebuffer != NULL) {
    free(framebuffer);
  }
  if (framebuffer_texture != NULL) {
    SDL_DestroyTexture(framebuffer_texture);
  }
  if (sdl_renderer != NULL) {
    SDL_DestroyRenderer(sdl_renderer);
  }
}

void R_UpdateScreen() {
  SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
  SDL_RenderClear(sdl_renderer);
  SDL_UpdateTexture(framebuffer_texture, NULL, framebuffer, (int)(screen_width * sizeof(uint32_t)));
  SDL_RenderCopy(sdl_renderer, framebuffer_texture, NULL, NULL);
  SDL_RenderPresent(sdl_renderer);
}

void R_ClearFrameBuffer() {
  //memset(framebuffer, 0, framebuffer_size);
  size_t count = screen_width * screen_height;
  for (size_t idx = 0; idx < count; ++idx) {
    framebuffer[idx] = 0xFF000000;
  }
}

uint32_t blend_colors(uint32_t srcColor, uint32_t destColor) {
  // Extract RGBA components from the source color
  uint8_t srcA = (srcColor >> 24) & 0xFF;
  uint8_t srcB = (srcColor >> 16) & 0xFF;
  uint8_t srcG = (srcColor >> 8) & 0xFF;
  uint8_t srcR = srcColor & 0xFF;

  // Extract RGBA components from the destination color
  uint8_t destA = (destColor >> 24) & 0xFF;
  uint8_t destB = (destColor >> 16) & 0xFF;
  uint8_t destG = (destColor >> 8) & 0xFF;
  uint8_t destR = destColor & 0xFF;

  // Normalize alpha to the range [0.0, 1.0]
  float alpha = srcA / 255.0f;

  // Compute the blended color components
  uint8_t outR = (uint8_t)((srcR * alpha) + (destR * (1 - alpha)));
  uint8_t outG = (uint8_t)((srcG * alpha) + (destG * (1 - alpha)));
  uint8_t outB = (uint8_t)((srcB * alpha) + (destB * (1 - alpha)));
  uint8_t outA = (uint8_t)(srcA + (destA * (1 - alpha)));  // Optional: you may choose to keep the destination alpha

  // Combine components back into a single uint32_t value
  return (outA << 24) | (outB << 16) | (outG << 8) | outR;
}

void R_DrawPoint(int32_t x, int32_t y, uint32_t color) {
  if (x < 0) x = (x + screen_width) % screen_width;
  if (x >= screen_width) x = (x - screen_width) % screen_width;
  if (y < 0) y = (y + screen_height) % screen_height;
  if (y >= screen_height) y = (y - screen_height) % screen_height;

  SDL_assert(x >= 0 && x < screen_width);
  SDL_assert(y >= 0 && y < screen_height);
  size_t index = (screen_width * y) + x;
  uint32_t existing = framebuffer[index];
  framebuffer[index] = blend_colors(color, existing);
}

// void R_DrawPointAA(int32_t x, int32_t y, uint32_t color) {
//   R_DrawPoint(x, y, color);
//   return;
//   if (x < 0) x = (x + screen_width) % screen_width;
//   if (x >= screen_width) x = (x - screen_width) % screen_width;
//   if (y < 0) y = (y + screen_height) % screen_height;
//   if (y >= screen_height) y = (y - screen_height) % screen_height;

//   SDL_assert(x >= 0 && x < screen_width);
//   SDL_assert(y >= 0 && y < screen_height);
//   uint32_t current = framebuffer[(screen_width * y) + x];
//   uint32_t current_alpha = current & 0xFF000000;
//   uint32_t alpha = SDL_max(current_alpha, color & 0xFF000000);
//   framebuffer[(screen_width * y) + x] = alpha | (color & 0x00FFFFFF);
// }

void R_DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
  int32_t dx = abs(x1 - x0);
  int32_t sx = x0 < x1 ? 1 : -1;
  int32_t dy = -abs(y1 - y0);
  int32_t sy = y0 < y1 ? 1 : -1;
  int32_t error = dx + dy;
  int32_t e2;

  for (;;) {
    R_DrawPoint(x0, y0, color);
    if (x0 == x1 && y0 == y1) {
      break;
    }

    e2 = 2 * error;

    if (e2 >= dy) {
      if (x0 == x1) {
        break;
      }
      error += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      if (y0 == y1) {
        break;
      }
      error += dx;
      y0 += sy;
    }
  }
}

/* draw a black (0) anti-aliased line on white (255) background */
void R_DrawLineAA(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
  int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, x2;
  long dx = abs(x1 - x0), dy = abs(y1 - y0), err = dx * dx + dy * dy;
  long e2 = err == 0 ? 1 : 0xffff7fl / sqrt(err);     /* multiplication factor */
  color = 0x00FFFFFF & color;
  uint32_t alpha = 0x00000000;

  dx *= e2; dy *= e2; err = dx - dy;                       /* error value e_xy */
  for (;;) {                                                 /* pixel loop */
    // R_DrawPoint(x0, y0, abs(err - dx + dy) >> 16);
    alpha = 255 - (abs(err - dx + dy) >> 16);
    R_DrawPoint(x0, y0, (alpha << 24) | color);
    e2 = err; x2 = x0;
    if (2 * e2 >= -dx) {                                            /* x step */
      if (x0 == x1) break;
      if (e2 + dy < 0xff0000l) {
        // R_DrawPoint(x0, y0 + sy, (e2 + dy) >> 16);
        alpha = 255 - ((e2 + dy) >> 16);
        R_DrawPoint(x0, y0 + sy, (alpha << 24) | color);
      }
      err -= dy; x0 += sx;
    }
    if (2 * e2 <= dy) {                                             /* y step */
      if (y0 == y1) break;
      if (dx - e2 < 0xff0000l) {
        // R_DrawPoint(x2 + sx, y0, (dx - e2) >> 16);
        alpha = 255 - ((dx - e2) >> 16);
        R_DrawPoint(x2 + sx, y0, (alpha << 24) | color);
      }
      err += dx; y0 += sy;
    }
  }
}

void R_DrawTriangle(
  int32_t p0x,
  int32_t p0y,
  int32_t p1x,
  int32_t p1y,
  int32_t p2x,
  int32_t p2y,
  uint32_t color
) {
  R_DrawLine(p0x, p0y, p1x, p1y, color);
  R_DrawLine(p1x, p1y, p2x, p2y, color);
  R_DrawLine(p2x, p2y, p0x, p0y, color);
}

void R_DrawCircle(int32_t cx, int32_t cy, int32_t r, uint32_t color) {
  int x = -r;
  int y = 0;
  int err = 2 - 2 * r;
  do {
    R_DrawPoint(cx - x, cy + y, color);
    R_DrawPoint(cx - y, cy - x, color);
    R_DrawPoint(cx + x, cy - y, color);
    R_DrawPoint(cx + y, cy + x, color);
    r = err;
    if (r > x) {
      err += ++x * 2 + 1;
    }
    if (r <= y) {
      err += ++y * 2 + 1;
    }
  } while (x < 0);
}

// https://gist.github.com/bert/1085538#file-raster_circle-c
void R_FillCircle(int32_t cx, int32_t cy, int32_t r, uint32_t color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  R_DrawLine(cx, cy + r, cx, cy - r, color);
  R_DrawLine(cx + r, cy, cx - r, cy, color);

  while (x < y) {
    // ddF_x == 2 * x + 1;
    // ddF_y == -2 * y;
    // f == x*x + y*y - radius*radius + 2*x - y + 1;
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    R_DrawLine(cx + x, cy + y, cx - x, cy + y, color);
    R_DrawLine(cx + x, cy - y, cx - x, cy - y, color);
    R_DrawLine(cx + y, cy + x, cx - y, cy + x, color);
    R_DrawLine(cx + y, cy - x, cx - y, cy - x, color);
  }
}

void R_DrawWireframe(const vec2_t *v, size_t v_count, uint32_t color) {
  SDL_assert(v_count > 1);

  for (size_t i = 1; i < v_count; ++i) {
    R_DrawLineAA(v[i - 1].x, v[i - 1].y, v[i].x, v[i].y, color);
  }
  R_DrawLineAA(v[v_count - 1].x, v[v_count - 1].y, v[0].x, v[0].y, color);
}

void R_DrawTextFont(font_t *font, int32_t x, int32_t y, const char *format, va_list args) {
  static char buf[1024];
  vsnprintf(buf, 1024, format, args);

  char *text = buf;

  char ch;
  uint32_t cx = x;
  uint32_t cy = y;
  while ((ch = *text++) != '\0') {
    if (ch == '\n') {
      cx = x;
      cy += font->glyph_height;
    } else {
      font_glyph_t glyph = font->glyphs[(uint8_t)ch];
      for (uint32_t py = 0; py < font->glyph_height; ++py) {
        for (uint32_t px = 0; px < font->glyph_width; ++px) {
          if (A_Font_Pixel(font, glyph.x + px, glyph.y + py)) {
            R_DrawPoint(cx + px, cy + py, 0xFFFFFFFF);
          }
        }
      }
      cx += font->glyph_width;
    }
  }
}

void R_DrawText(int32_t x, int32_t y, const char *format, ...) {
  va_list args;
  va_start(args, format);
  R_DrawTextFont(large_font, x, y, format, args);
  va_end(args);
}

void R_DrawSmallText(int32_t x, int32_t y, const char *format, ...) {
  va_list args;
  va_start(args, format);
  R_DrawTextFont(small_font, x, y, format, args);
  va_end(args);
}

#define NOISE_WIDTH 512
#define NOISE_HEIGHT 512

double noise[NOISE_WIDTH * NOISE_HEIGHT];

#define STARS_MAX_COUNT 25000
typedef struct {
  uint32_t x;
  uint32_t y;
  uint32_t color;
  double brightness;
} lol_star_t;
static lol_star_t stars[STARS_MAX_COUNT];

void R_Render(game_state_t *game SDL_UNUSED) {
  R_ClearFrameBuffer();

  P_EntityManager_Render();

  // int32_t mouse_x, mouse_y;
  // SDL_GetMouseState(&mouse_x, &mouse_y);
  // R_DrawCircle(mouse_x / 2, mouse_y / 2, 2, 0xFF0000FF);

  // TODO: Don't update this in render?
  R_Particles_Update(game->delta_time);
  R_Particles_Render();

  // for (int y = 0; y < 400; ++y) {
  //   for (int x = 0; x < 400; ++x) {
  //     double n = noise[(y % NOISE_HEIGHT) * NOISE_WIDTH + (x % NOISE_WIDTH)];
  //     //uint32_t c = n * 255;
  //     //uint32_t color = 0xFF000000 | (c << 16) | (c << 8) | c;
  //     uint32_t color = 0xFF000000;

  //     if (n > 0.98) {
  //       color = 0xFFFF0000;
  //     }
  //     R_DrawPoint(x, y, color);
  //   }
  // }

  for (size_t i = 0; i < STARS_MAX_COUNT; ++i) {
    // float brightness = ((stars[i].brightness / 2.0f) * ((SDL_sinf(game->current_tick * 0.00025) + 1.0) / 2.0f)) + 0.25f;
    float brightness = stars[i].brightness;
    //stars[i].brightness = fmod(stars[i].brightness + (0.000025 * game->delta_time) * 255, 0.5);
    uint8_t a = brightness * 255;
    uint32_t color = (a << 24) | stars[i].color;
    R_DrawPoint(stars[i].x, stars[i].y, color);
  }

  R_UpdateScreen();
}

void xxx_gen_noise() {
  size_t lol_idx = 0;
  size_t dist = (1024 * 768 / STARS_MAX_COUNT) * 4;
  double scale = 0.025;
  for (size_t i = 0; i < STARS_MAX_COUNT; ++i) {
    lol_idx += rand_float() * dist;
    if (lol_idx >= 1024 * 768) {
      break;
    }
    uint32_t x = lol_idx % 768;
    uint32_t y = lol_idx / 768;
    stars[i].x = x;
    stars[i].y = y;
    double n = PerlinNoise3D(x * scale, 0, y * scale, 1234, 2, 0.5);
    n = SDL_clamp((n + 1.0) * 0.5, 0, 1);
    uint8_t r, g, b;
    if (n > 0.85) {
      r = n * 127;
      g = n * 127;
      b = n * 255;
    } else if (n > 0.5) {
      r = n * 200;
      g = n * 200;
      b = n * 250;
    } else {
      r = n * 225;
      g = n * 100;
      b = n * 100;
    }
    // uint8_t r = n * 127;
    // uint8_t g = n * 125;
    // uint8_t b = n * 255;
    stars[i].color = 0x00000000 | (b << 16) | (g << 8) | r;
    //stars[i].brightness = SDL_clamp((n + 1.0) * 0.5, 0, 1);
    stars[i].brightness = n * n * n;
    //stars[i].brightness *= stars[i].brightness;
  }

  for (int y = 0; y < NOISE_HEIGHT; ++y) {
    for (int x = 0; x < NOISE_WIDTH; ++x) {
      // double scale = 0.025;

      //double n = SDL_clamp(pnoise2d(x, y, 1.0, 5, 12345), -2, 2);
      //uint32_t c = ((n + 2.0) / 4.0) * 255;
      // float n = perlin2D_octaves(fx * scale, fy * scale, 3, 0.7f);
      // n = SDL_clamp(n * 5.0f, -1, 1);
      // n = n > 0 ? powf(n, 2) : -powf(n, 2);
      //float n = fbm(fx * scale, fy * scale, 5, 0.85f, 1.25f);
      // double n = ken_noise(x * scale, y * scale, 0.0, 5, 0.7);
      //double n = ken_noise_vanilla(x * scale, y * scale, 0.0);
      // double n = fbm2D(x * scale, y * scale, 5, 1.15, 0.25);
      // n = sigmoid(n, 5.0);
      //n = SDL_clamp(n * 1.2, -1, 1);


      // double fx = x * scale;
      // double fy = y * scale;
      // double n = PerlinNoise3D(fx, 0, fy, 1234, 7, 0.8);

      // double n = 0.0;
      // double curPersistence = 1.0;
      // for (int octave = 0; octave < 3; ++octave) {
      //   double signal = GradientCoherentNoise3D(fx, 0, fy, 1234, NOISE_QUALITY_BEST);
      //   signal = 2.0 * fabs(signal) - 1.0;
      //   n += signal * curPersistence;

      //   fx *= 1.3;
      //   fy *= 1.3;
      //   curPersistence *= 0.3;
      // }
      // n += 0.5;

      // noise[y * NOISE_WIDTH + x] = SDL_clamp((n + 1.0) / 2.0, 0, 1);

      noise[y * NOISE_WIDTH + x] = rand_float();
    }
  }
}
