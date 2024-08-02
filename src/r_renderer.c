#include <stdarg.h>
#include <stdbool.h>

#include "r_renderer.h"
#include "m_message.h"
#include "a_bitmap.h"
#include "a_font.h"
#include "vec2.h"

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

font_t *font = NULL;

font_glyph_t font_glyphs[256];
#define FONT_GLYPHS_SIZE sizeof(font_glyphs) / sizeof(font_glyph_t)

#define FONT_GLYPH_WIDTH 4
#define FONT_GLYPH_HEIGHT 6
#define FONT_GLYPH_RANGE_START 0
#define FONT_GLYPH_RANGE_END 128

// #define FONT_GLYPH_WIDTH 6
// #define FONT_GLYPH_HEIGHT 12
// #define FONT_GLYPH_RANGE_START 32
// #define FONT_GLYPH_RANGE_END 128

#define FONT_GLYPHS_PER_ROW 16
#define FONT_GLYPHS_PER_COL 8
#define FONT_PIXEL_DATA_SIZE FONT_GLYPH_WIDTH * FONT_GLYPHS_PER_ROW * FONT_GLYPH_HEIGHT * FONT_GLYPHS_PER_COL
unsigned char font_pixel_data[FONT_PIXEL_DATA_SIZE];

bool R_LoadFont(const char *path) {
  font = A_Font_ReadConfig("assets/monogram_6x10.conf");
  if (font == NULL) {
    M_Log("[Renderer] Failed to load font\n");
    return false;
  }

  bitmap_t *font_bitmap = A_Bitmap_ReadFile(path);
  if (font_bitmap == NULL) {
    M_Log("[Renderer] Failed to create font\n");
    return false;
  }
  // TODO: Revisit this.
  SDL_assert(font_bitmap->bpp == 1);

  memset(font_pixel_data, 0, FONT_PIXEL_DATA_SIZE);
  uint32_t bstride = (FONT_GLYPH_WIDTH * FONT_GLYPHS_PER_ROW);

  for (uint32_t ch = FONT_GLYPH_RANGE_START; ch < FONT_GLYPH_RANGE_END; ++ch) {
    switch (font_bitmap->bpp) {
      case 1: {
        uint32_t stride = font_bitmap->stride;
        // p = 112
        // gx = 112 % 16 = 0
        // gy = 112 / 16 = 7
        uint32_t gx = (ch - FONT_GLYPH_RANGE_START) % FONT_GLYPHS_PER_ROW;
        uint32_t gy = (ch - FONT_GLYPH_RANGE_START) / FONT_GLYPHS_PER_ROW;
        // uint32_t bx = gx * FONT_GLYPH_WIDTH;
        // uint32_t by = (FONT_GLYPHS_PER_COL - gy) * FONT_GLYPH_HEIGHT;

        // size_t glyph_idx = gy * FONT_GLYPHS_PER_ROW + gx;
        font_glyphs[ch].x = gx * FONT_GLYPH_WIDTH;
        font_glyphs[ch].y = gy * FONT_GLYPH_HEIGHT;
        // font_glyphs[ch].index = (gy * FONT_GLYPH_HEIGHT) * bstride + (gx * FONT_GLYPH_WIDTH);
        // font_glyphs[ch].ch = ch;

        //uint32_t bidx = font_glyphs[ch].index;

        // uint8_t shift = gx % 2 == 0 ? 4 : 0;
        for (uint32_t oy = 0; oy < FONT_GLYPH_HEIGHT; ++oy) {
          //uint32_t idx = ((FONT_GLYPHS_PER_COL - 1 - gy - oy) * FONT_GLYPH_HEIGHT * stride) + (gx / 2);
          //uint32_t idx = (((FONT_GLYPHS_PER_COL - gy) * FONT_GLYPH_HEIGHT) - oy) * stride + (gx / 2);
          //uint32_t idx = (((FONT_GLYPHS_PER_COL - gy - 1) * FONT_GLYPH_HEIGHT) + (FONT_GLYPH_HEIGHT - oy)) * stride + (gx / 2);
          // uint32_t idx = (((FONT_GLYPHS_PER_COL - gy) * FONT_GLYPH_HEIGHT - oy) * stride) + (gx / 2);
          uint32_t y = gy * FONT_GLYPH_HEIGHT + oy;
          uint32_t inverted_y = (font_bitmap->height - 1 - y);
          // uint32_t idx = (font_bitmap->height - 1 - (gy * FONT_GLYPH_HEIGHT + oy)) * stride + (gx / 2);
          uint32_t idx = inverted_y * stride + (gx / 2);
          SDL_assert(idx < font_bitmap->size);

          for (uint32_t ox = 0; ox < FONT_GLYPH_WIDTH; ++ox) {
            uint32_t x = (gx * FONT_GLYPH_WIDTH + ox);
            uint32_t byte_index = (inverted_y * stride) + (x / 8);
            uint32_t bit_index = 7 - (x % 8);
            unsigned char pixel = (font_bitmap->data[byte_index] >> bit_index) & 1;
            if (!pixel) {
              uint32_t bidx = y * bstride + x;
              font_pixel_data[bidx] = 0xff;
            }
          }



          // uint8_t mask = 8;
          // // FIXME: This doesn't work for cases where more than 8 pixels need to be read for a single glyph.
          // unsigned char b = font_bitmap->data[idx] >> shift;
          // if (b) {
          //   for (uint32_t ox = 0; ox < FONT_GLYPH_WIDTH; ++ox) {
          //     uint32_t bidx = ((gy * FONT_GLYPH_HEIGHT) + oy) * bstride + (gx * FONT_GLYPH_WIDTH) + ox;
          //     if (b & mask) {
          //       font_pixel_data[bidx] = 0xff;
          //     }
          //     mask >>= 1;
          //   }
          // }

          //bidx += bstride;
        }
        break;
      }
      default:
        SDL_assert(false);
        break;
    }
  }

  A_Bitmap_Free(font_bitmap);

  return true;
}

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

  if (!R_LoadFont("assets/font_4x6.bmp")) {
    // if (!R_LoadFont("assets/monogram_6x10.bmp")) {
    return false;
  }

  M_Log("[Renderer] Initialized %dx%d\n", render_width, render_height);

  return true;
}

void R_Shutdown() {
  M_Log("[Renderer] Destroying\n");
  if (font != NULL) {
    A_Font_Free(font);
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
  SDL_UpdateTexture(framebuffer_texture, NULL, framebuffer, (int)(screen_width * sizeof(uint32_t)));
  SDL_RenderCopy(sdl_renderer, framebuffer_texture, NULL, NULL);
  SDL_RenderPresent(sdl_renderer);
}

void R_ClearFrameBuffer() {
  memset(framebuffer, 0, framebuffer_size);
}

void R_DrawPoint(int32_t x, int32_t y, uint32_t color) {
  if (x < 0) x = (x + screen_width) % screen_width;
  if (x >= screen_width) x = (x - screen_width) % screen_width;
  if (y < 0) y = (y + screen_height) % screen_height;
  if (y >= screen_height) y = (y - screen_height) % screen_height;

  SDL_assert(x >= 0 && x < screen_width);
  SDL_assert(y >= 0 && y < screen_height);
  framebuffer[(screen_width * y) + x] = color;
}

void R_DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color) {
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
    R_DrawLine(v[i - 1].x, v[i - 1].y, v[i].x, v[i].y, color);
  }
  R_DrawLine(v[v_count - 1].x, v[v_count - 1].y, v[0].x, v[0].y, color);
}

void R_DrawText(int32_t x, int32_t y, const char *format, ...) {
  static char buf[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(buf, 1024, format, args);
  va_end(args);

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

void R_DrawPlayer(const player_t *player) {
  // Draw the thruster.
  if (vec2_length(&player->accel) > 0) {
    vec2_t vt[] = {
      {-2.5f,  5.5f},
      { 0.0f,  10.0f},
      { 2.5f,  5.5f}
    };
    size_t vt_count = sizeof(vt) / sizeof(vec2_t);
    for (size_t i = 0; i < vt_count; ++i) {
      vec2_irotate(&vt[i], player->dir_angle);
      vec2_iadd(&vt[i], &player->pos);
    }
    R_DrawWireframe(vt, vt_count, 0xFF00FFFF);
  }

  // Draw the ship.
  vec2_t vs[] = {
    {0.0f, -10.0f},
    {5.0f, 5.0f},
    {-5.f, 5.0f}
  };
  size_t vs_count = sizeof(vs) / sizeof(vec2_t);
  for (size_t i = 0; i < vs_count; ++i) {
    vec2_irotate(&vs[i], player->dir_angle);
    vec2_iadd(&vs[i], &player->pos);
  }
  R_DrawWireframe(vs, vs_count, 0xFF0000FF);

  R_DrawText(player->pos.x - 11, player->pos.y + 10, "Emma");
}

// void R_DrawTextXXX(int32_t x, int32_t y, const char *format, ...) {
//   SDL_assert(font_bitmap != NULL);

//   uint32_t cx = x;
//   uint32_t cy = y;
//   uint32_t glyph_width = 4;
//   uint32_t glyph_height = 6;
//   uint32_t glyphs_per_row = 16;
//   uint32_t stride = font_bitmap->width / 8;

//   static char buf[1024];
//   va_list args;
//   va_start(args, format);
//   vsnprintf(buf, 1024, format, args);
//   va_end(args);

//   char *text = buf;

//   char ch;
//   while ((ch = *text++) != '\0') {
//     if (ch == ' ') {
//       cx += glyph_width;
//     } if (ch == '\n') {
//       cx = x;
//       cy += glyph_height;
//     } else {
//       uint32_t gx = (uint32_t)ch % glyphs_per_row;
//       uint32_t by = font_bitmap->height - (((uint32_t)ch / glyphs_per_row) * glyph_height);
//       uint8_t shift = gx % 2 == 0 ? 4 : 0;
//       for (uint32_t oy = 1; oy <= glyph_height; ++oy) {
//         uint32_t idx = (by - oy) * stride + (gx / 2);
//         uint8_t mask = 8;
//         unsigned char b = font_bitmap->data[idx] >> shift;
//         if (b) {
//           for (uint32_t ox = 1; ox <= glyph_width; ++ox) {
//             if (b & mask) {
//               R_DrawPoint(cx + ox, cy + oy, 0xFFFFFFFF);
//             }
//             mask >>= 1;
//           }
//         }
//       }

//       cx += glyph_width;
//     }
//   }


//   // R_DrawPoint(x + 1, y, 0xFFFFFFFF);
//   // R_DrawPoint(x + 2, y, 0xFFFFFFFF);
//   // R_DrawPoint(x + 3, y, 0xFFFFFFFF);
//   // R_DrawPoint(x, y + 1, 0xFFFFFFFF);
//   // R_DrawPoint(x + 4, y + 1, 0xFFFFFFFF);
//   // R_DrawPoint(x, y + 2, 0xFFFFFFFF);
//   // R_DrawPoint(x + 4, y + 2, 0xFFFFFFFF);
//   // R_DrawPoint(x, y + 3, 0xFFFFFFFF);
//   // R_DrawPoint(x + 4, y + 3, 0xFFFFFFFF);
//   // R_DrawPoint(x, y + 4, 0xFFFFFFFF);
//   // R_DrawPoint(x + 4, y + 4, 0xFFFFFFFF);
//   // R_DrawPoint(x, y + 5, 0xFFFFFFFF);
//   // R_DrawPoint(x + 4, y + 5, 0xFFFFFFFF);
//   // R_DrawPoint(x + 1, y + 6, 0xFFFFFFFF);
//   // R_DrawPoint(x + 2, y + 6, 0xFFFFFFFF);
//   // R_DrawPoint(x + 3, y + 6, 0xFFFFFFFF);
// }

// void R_Debugging(const player_t *player) {
//   // TODO: Figure out text rendering
//   //SDL_Text
// }

void R_Render(game_state_t *game_state __attribute__((unused)), player_t *player) {
  R_ClearFrameBuffer();

  P_EntityManager_Render();
  R_DrawPlayer(player);
  // R_DrawText(100, 100, "Hello world!"); 
  float speed = vec2_length(&player->velocity);
  R_DrawText(4, 384 - 16, "pos: %.0f,%.0f   vel: %.0f,%.0f  spd: %.0f", player->pos.x, player->pos.y, player->velocity.x, player->velocity.y, speed);


  // R_Debugging(player);

  R_UpdateScreen();
}
