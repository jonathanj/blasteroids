#ifndef K_KEYBOARD_H
#define K_KEYBOARD_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "g_game_state.h"
#include "p_player.h"

typedef struct keymap {
  SDL_Scancode turn_left;
  SDL_Scancode turn_right;
  SDL_Scancode forward;
  SDL_Scancode backward;
  SDL_Scancode strafe_left;
  SDL_Scancode strafe_right;
  SDL_Scancode quit;
} keymap_t;

typedef struct keymap_state {
  bool turn_left;
  bool turn_right;
  bool forward;
  bool backward;
  bool strafe_left;
  bool strafe_right;
  bool quit;
} keymap_state_t;

enum key_state_t {
  KEY_STATE_UP,
  KEY_STATE_DOWN
};

void K_InitKeymap();
void K_HandleEvents(game_state_t *state, player_t *player);
void K_ProcessKeyStates(player_t *player, double delta_time);
void K_HandleRealtimeKeys(SDL_Scancode scancode, enum key_state_t state);

#endif
