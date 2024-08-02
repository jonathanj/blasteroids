#ifndef G_CONTROLLER_H
#define G_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "linked_list.h"
#include "vec2.h"

typedef enum {
  CONTROLLER_KEYBOARD_1,
  CONTROLLER_KEYBOARD_2,
  CONTROLLER_MOUSE,
  CONTROLLER_GAMEPAD,
} g_controller_type;

typedef struct {
  vec2_t movement;
  float thrust;
} g_controller_input_state_t;

typedef struct {
  SDL_Scancode thrust;
  SDL_Scancode turn_left;
  SDL_Scancode turn_right;
} g_controller_keymap_t;

typedef struct g_controller_t g_controller_t;

struct g_controller_t {
  g_controller_input_state_t input_state;
  const g_controller_keymap_t *keymap;
  bool (*handle_event)(g_controller_t *controller, const SDL_Event *event);
};

void G_Controllers_Init();
void G_Controllers_Shutdown();
void G_Controllers_Attach(g_controller_t *controller);
void G_Controllers_HandleEvent(const SDL_Event *event);

g_controller_t *G_Controller_Create(g_controller_type type);
void G_Controller_Free(g_controller_t *controller);

#endif
