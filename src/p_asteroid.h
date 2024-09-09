#ifndef P_ASTEROID_H
#define P_ASTEROID_H

#include <stdint.h>

#include "p_entity.h"
#include "g_game_state.h"
#include "vec2.h"

void P_Asteroid_Spawn(vec2_t position, vec2_t velocity, uint32_t size);
void P_Asteroid_Think(p_entity_t *asteroid, const game_state_t *state);

#endif
