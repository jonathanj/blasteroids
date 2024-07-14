#ifndef R_RENDERER_H
#define R_RENDERER_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "g_game_state.h"
#include "p_player.h"

bool R_Init(SDL_Window *window, const game_state_t *game_state);
void R_Render(game_state_t *game_state, player_t *player);
void R_Shutdown();

#endif
