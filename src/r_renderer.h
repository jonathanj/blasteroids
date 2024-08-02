#ifndef R_RENDERER_H
#define R_RENDERER_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "g_game_state.h"
#include "p_player.h"

bool R_Init(SDL_Window *window, const game_state_t *game_state);
void R_Render(game_state_t *game_state, player_t *player);
void R_Shutdown();

// TODO: Maybe this should live elsewhere?
// Drawing
void R_DrawCircle(int32_t cx, int32_t cy, int32_t r, uint32_t color);
void R_DrawText(int32_t x, int32_t y, const char *format, ...);
void R_DrawSmallText(int32_t x, int32_t y, const char *format, ...);
void R_DrawWireframe(const vec2_t *v, size_t v_count, uint32_t color);

#endif
