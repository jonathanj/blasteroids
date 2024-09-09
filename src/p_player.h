#ifndef P_PLAYER_H
#define P_PLAYER_H

#include "vec2.h"
#include "g_game_state.h"
#include "g_controller.h"

void P_Player_Spawn(const char *name, const g_controller_t *controller, vec2_t position, uint32_t color);

#endif
