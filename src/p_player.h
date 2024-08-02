#ifndef P_PLAYER_H
#define P_PLAYER_H

#include "vec2.h"
#include "g_game_state.h"

void P_Player_Spawn(const char *name, vec2_t position);

/////////////

typedef struct player {
  vec2_t pos;
  vec2_t velocity;
  vec2_t accel;
  double dir_angle;
} player_t;

player_t P_Init(double x, double y, double angle);
void P_Think(player_t *player, game_state_t *game_state);

#endif
