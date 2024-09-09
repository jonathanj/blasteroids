#include <SDL2/SDL.h>

#include "g_game_state.h"
#include "g_controller.h"

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
    .world = {.width = screen_width, .height = screen_height},
  };
  game_state.delta_time = game_state.target_frame_time;
  return game_state;
}


void G_FrameStart(game_state_t *state) {
  state->current_tick = SDL_GetTicks();
}

void G_FrameEnd(game_state_t *state) {
  state->delta_time = (SDL_GetTicks() - state->current_tick) / 1000.0;

  if (state->delta_time < state->target_frame_time) {
    SDL_Delay((state->target_frame_time - state->delta_time) * 1000.0);
    state->delta_time = state->target_frame_time;
  }
}

void G_HandleEvents(game_state_t *state) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        state->is_running = false;
        break;
      default:
        if (!G_Controllers_HandleEvent(&event)) {
          // TODO: If the event wasn't handled by a controller, then maybe the
          // game should try handle it?
          //
          // for example: pressing a key to join the game and spawn your player
        }
        break;
    }
  }
}
