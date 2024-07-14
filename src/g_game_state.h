#ifndef G_GAME_STATE_H
#define G_GAME_STATE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct game_state {
  uint32_t screen_width;
  uint32_t screen_height;
  double target_fps;
  double target_frame_time;
  double delta_time;
  bool is_running;
} game_state_t;

game_state_t G_Init(const uint32_t screen_width, const uint32_t screen_height, uint8_t target_fps);
void G_FrameStart();
void G_FrameEnd(game_state_t *game_state);

#endif
