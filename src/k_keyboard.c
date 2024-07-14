#include "k_keyboard.h"

keymap_t keymap;
keymap_state_t keymap_state;

void K_InitKeymap() {
  keymap.turn_left = SDL_SCANCODE_A;
  keymap.turn_right = SDL_SCANCODE_D;
  keymap.forward = SDL_SCANCODE_W;
  // keymap.turn_left = SDL_SCANCODE_LEFT;
  // keymap.turn_right = SDL_SCANCODE_RIGHT;
  // keymap.forward = SDL_SCANCODE_UP;
  // keymap.backward = SDL_SCANCODE_S;
  //keymap.strafe_left = SDL_SCANCODE_A;
  //keymap.strafe_right = SDL_SCANCODE_D;
  keymap.quit = SDL_SCANCODE_ESCAPE;
}

void K_HandleEvents(game_state_t *game_state, player_t *player) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYDOWN:
        K_HandleRealtimeKeys(event.key.keysym.scancode, KEY_STATE_DOWN);
        break;
      case SDL_KEYUP:
        K_HandleRealtimeKeys(event.key.keysym.scancode, KEY_STATE_UP);
        break;
      case SDL_QUIT:
        game_state->is_running = false;
        break;
    }
  }

  K_ProcessKeyStates(player, game_state->delta_time);
}

#define M_PI    3.14159265358979323846264338327950288
#define M_HALF_PI 1.57079632679
const float TURN_RATE = M_PI;

// NOTE: I don't like that the keyboard has to know about the player.
void K_ProcessKeyStates(player_t *player, double delta_time) {
  if (keymap_state.forward) {
    float theta = player->dir_angle - M_HALF_PI;
    player->accel.x = 100 * SDL_cos(theta);
    player->accel.y = 100 * SDL_sin(theta);
  } else {
    player->accel.x = 0;
    player->accel.y = 0;
  }
  // if (keymap_state.backward) {
  //   player->pos.y += 40 * delta_time;
  // }
  // if (keymap_state.strafe_left) {
  //   player->pos.x -= 40 * delta_time;
  // }
  // if (keymap_state.strafe_right) {
  //   player->pos.x += 40 * delta_time;
  // }
  if (keymap_state.turn_left) {
    player->dir_angle -= TURN_RATE * delta_time;
  }
  if (keymap_state.turn_right) {
    player->dir_angle += TURN_RATE * delta_time;
  }
}

void K_HandleRealtimeKeys(SDL_Scancode scancode, enum key_state_t state) {
  if (scancode == keymap.forward) {
    keymap_state.forward = state;
  } else if (scancode == keymap.backward) {
    keymap_state.backward = state;
  }

  if (scancode == keymap.turn_left) {
    keymap_state.turn_left = state;
  } else if (scancode == keymap.turn_right) {
    keymap_state.turn_right = state;
  }

  if (scancode == keymap.strafe_left) {
    keymap_state.strafe_left = state;
  } else if (scancode == keymap.strafe_right) {
    keymap_state.strafe_right = state;
  }
}
