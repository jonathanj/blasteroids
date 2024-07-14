#include "p_player.h"

player_t P_Init(double x, double y, double angle) {
  player_t player = {
    .dir_angle = angle,
    .pos = {x, y},
    .accel = {0.0f, 0.0f},
    .velocity = {0.0f, 0.0f}
  };
  return player;
}

void P_Think(player_t *player, game_state_t *state) {
  float delta_time = state->delta_time;

  // Update physics.
  player->velocity.x += player->accel.x * delta_time;
  player->velocity.y += player->accel.y * delta_time;
  player->pos.x += player->velocity.x * delta_time;
  player->pos.y += player->velocity.y * delta_time;
}
