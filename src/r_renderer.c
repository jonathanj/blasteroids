#include "r_renderer.h"
#include "m_message.h"
#include "vec2.h"

SDL_Renderer *sdl_renderer = NULL;
SDL_Texture *framebuffer_texture = NULL;
uint32_t *framebuffer = NULL;
size_t framebuffer_size = 0;

uint16_t screen_width, screen_height;

bool R_InitScreen(uint16_t width, uint16_t height) {
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

bool R_Init(SDL_Window *window, const game_state_t *game_state) {
  sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  if (sdl_renderer == NULL) {
    M_Log("[Renderer] SDL renderer could not be created: %s\n", SDL_GetError());
    return false;
  }

  uint32_t render_width = game_state->screen_width / 2;
  uint32_t render_height = game_state->screen_height / 2;

  if (!R_InitScreen(render_width, render_height)) {
    return false;
  }
  SDL_RenderSetLogicalSize(sdl_renderer, render_width, render_height);

  M_Log("[Renderer] Initialized %dx%d\n", render_width, render_height);

  return true;
}

void R_Shutdown() {
  M_Log("[Renderer] Destroying\n");
  if (framebuffer) {
    free(framebuffer);
  }
  if (framebuffer_texture) {
    SDL_DestroyTexture(framebuffer_texture);
  }
  if (sdl_renderer) {
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
  if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) {
    return;
  }

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

void R_DrawPlayer(const player_t *player) {
  vec2_t v[] = {
    {0.0f, -10.0f},
    {5.0f, 5.0f},
    {-5.f, 5.0f}
  };
  size_t v_count = sizeof(v) / sizeof(vec2_t);
  for (size_t i = 0; i < v_count; ++i) {
    vec2_irotate(&v[i], player->dir_angle);
    vec2_iadd(&v[i], &player->pos);
  }
  for (size_t i = 1; i < v_count; ++i) {
    R_DrawLine(v[i - 1].x, v[i - 1].y, v[i].x, v[i].y, 0xFF0000FF);
  }
  R_DrawLine(v[v_count - 1].x, v[v_count - 1].y, v[0].x, v[0].y, 0xFF0000FF);
  //R_DrawCircle((uint32_t)player->pos.x, (uint32_t)player->pos.y, 8, 0xFFFFFFFF);
}

void R_Render(game_state_t *game_state __attribute__((unused)), player_t *player) {
  R_ClearFrameBuffer();

  R_DrawPlayer(player);

  R_UpdateScreen();
}
