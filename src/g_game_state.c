#include <SDL2/SDL.h>
#include "g_game_state.h"

game_state_t G_Init(
  const uint32_t screen_width,
  const uint32_t screen_height,
  const uint8_t target_fps
) {
  game_state_t game_state = {
    .screen_width = screen_width,
    .screen_height = screen_height,
    .target_fps = target_fps,
    .target_frame_time = 1.0 / (double)target_fps,
    .delta_time = 0,
    .is_running = true,
    // FIXME
    .world = {.width = 1024 / 2, .height = 768 / 2},
  };
  game_state.delta_time = game_state.target_frame_time;
  return game_state;
}

uint32_t frame_start = 0;

void G_FrameStart() {
  frame_start = SDL_GetTicks();
}

void G_FrameEnd(game_state_t *state) {
  state->delta_time = (SDL_GetTicks() - frame_start) / 1000.0;

  if (state->delta_time < state->target_frame_time) {
    SDL_Delay((state->target_frame_time - state->delta_time) * 1000.0);
    state->delta_time = state->target_frame_time;
  }
}
