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

  if (!R_LoadFonts()) {
    return false;
  }

  M_Log("[Renderer] Initialized %dx%d\n", render_width, render_height);

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

void R_Render(game_state_t *game_state __attribute__((unused)), player_t *player SDL_UNUSED) {
  R_ClearFrameBuffer();

  P_EntityManager_Render();
  // R_DrawPlayer(player);
  // R_DrawText(100, 100, "Hello world!"); 
  // float speed = vec2_length(&player->velocity);
  // R_DrawText(4, 384 - 16, "pos: %.0f,%.0f   vel: %.0f,%.0f  spd: %.0f", player->pos.x, player->pos.y, player->velocity.x, player->velocity.y, speed);

  // R_Debugging(player);

  R_UpdateScreen();
}
