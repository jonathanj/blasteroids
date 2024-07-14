#ifndef W_WINDOW_H
#define W_WINDOW_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

bool W_Init(const uint32_t window_width, const uint32_t window_height);
void W_Shutdown();
SDL_Window *W_Get();

#endif
