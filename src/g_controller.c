#include <SDL2/SDL.h>

#include "g_controller.h"

// TODO

/*

Could be:

- Keyboard region 1
- Keyboard region 2
- Mouse
- Controller

Controller needs to know what input it should read, and set the input state
accordingly.

This is more or less what k_keyboard.c does but directly, we need this indirectly.

- init a controller type
- controller type reads state from the input device
- controller map/input state is set based on device inputs

Things to consider:

- analgoue inputs will handle up/down/left/right in a single vector, it might
  be worth treating motion inputs as a vector regardless?

*/

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

typedef struct g_controller_t g_controller_t;

struct g_controller_t {
  g_controller_input_state_t input_state;
  void(*handle_event)(g_controller_t *controller, const SDL_Event *event);
};

void G_Controller_Keyboard1_HandleEvent();

g_controller_t *G_Controller_Init(g_controller_type type) {
  g_controller_t *controller = malloc(sizeof(g_controller_t));
  SDL_assert(controller != NULL);

  controller->input_state = {
    .movement = {0, 0},
    .menu = false,
  };

  switch (type) {
    case CONTROLLER_KEYBOARD_1:
      controller->handle_event = G_Controller_Keyboard1_HandleEvent;
      break;
    case CONTROLLER_KEYBOARD_2:
    case CONTROLLER_MOUSE:
    case CONTROLLER_GAMEPAD:
      SDL_assert(false);
      break;
  }

  return controller;
}

void G_Controller_HandleEvent(linked_list_node_t *controllers, const SDL_Event *event) {
  for (linked_list_node_t *node = controllers; node->next != NULL; node = node->next) {
    g_controller_t *controller = node->data;
    if (controller->can_handle_event(controller, event)) {
      controller->handle_event(controller, event);
    }
  }
}

void G_Controller_Free(g_controller_t *controller) {
  if (controller) {
    free(controller);
  }
}

void G_Controller_Keyboard1_HandleEvent(g_controller_t *controller, const SDL_Event *event) {
  // TODO: How does one controller not handle another controller's input?
  // For example, how does one controller not handle WASD while the other controller does not handle UDLR?
  // Maybe the controller has a keymap?
  // Gamepads have an instance ID, how does it use a keymap? Maybe keymap is a pointer and can be NULL?
  if (event->type != SDL_KEYDOWN || event->type != SDL_KEYUP) {
    return;
  }

  SDL_Scancode scancode = event->key.keysym.scancode;
  bool pressed = event->type == SDL_KEYDOWN;
  g_controller_input_state_t *input_state = &controller->input_state;

  if (scancode == SDL_SCANCODE_W) {
    input_state->thrust = pressed ? 1.0f : 0.0f;
  }

  if (scancode == SDL_SCANCODE_A) {
    input_state->movement.x = -1.0f;
  } else if (scancode == SDL_SCANCODE_D) {
    input_state->movement.x = 1.0f;
  }
}
