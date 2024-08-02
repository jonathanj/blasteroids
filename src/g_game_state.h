#ifndef G_GAME_STATE_H
#define G_GAME_STATE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint32_t width;
  uint32_t height;
} world_t;

typedef struct game_state {
  uint32_t screen_width;
  uint32_t screen_height;
  double target_fps;
  double target_frame_time;
  double delta_time;
  bool is_running;

  world_t world;
} game_state_t;

game_state_t G_Init(const uint32_t screen_width, const uint32_t screen_height, const uint8_t target_fps);
void G_FrameStart();
void G_FrameEnd(game_state_t *game_state);
void G_HandleEvents(game_state_t *state);

#endif
