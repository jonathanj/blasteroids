#include "w_window.h"
#include "m_message.h"

SDL_Window *sdl_window = NULL;

bool W_Init(const uint32_t window_width, const uint32_t window_height) {
  // SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    M_Log("[Window] SDL could not initialize: %s\n", SDL_GetError());
    return false;
  }

  sdl_window = SDL_CreateWindow(
    "SDL",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    window_width,
    window_height,
    SDL_WINDOW_SHOWN
  );
  if (sdl_window == NULL) {
    M_Log("[Window] Could not be created: %s\n", SDL_GetError());
    return false;
  }

  M_Log("[Window] Created %dx%d\n", window_width, window_height);

  return true;
}

void W_Shutdown() {
  if (sdl_window) {
    M_Log("[Window] Destroying\n");
    SDL_DestroyWindow(sdl_window);
  }
  SDL_Quit();
}

SDL_Window *W_Get() {
  return sdl_window;
}
