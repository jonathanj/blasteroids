#include "g_controller.h"
#include "m_message.h"

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


static const g_controller_keymap_t KEYBOARD1_KEYMAP = {
  .thrust = SDL_SCANCODE_W,
  .turn_left = SDL_SCANCODE_A,
  .turn_right = SDL_SCANCODE_D,
};

static const g_controller_keymap_t KEYBOARD2_KEYMAP = {
  .thrust = SDL_SCANCODE_UP,
  .turn_left = SDL_SCANCODE_LEFT,
  .turn_right = SDL_SCANCODE_RIGHT,
};

static linked_list_node_t *controllers = NULL;

void G_Controllers_Init() {
  controllers = linked_list_new(NULL);
}

void G_Controllers_Shutdown() {
  for (linked_list_node_t *node = controllers; node->next != NULL; node = node->next) {
    G_Controller_Free(node->data);
  }

  if (controllers) {
    linked_list_free(controllers);
  }
}

void G_Controllers_Attach(g_controller_t *controller) {
  linked_list_append(controllers, controller);
}

void G_Controllers_HandleEvent(const SDL_Event *event) {
  for (linked_list_node_t *node = controllers; node != NULL; node = node->next) {
    g_controller_t *controller = node->data;
    if (controller != NULL) {
      if (controller->handle_event(controller, event)) {
        break;
      }
    }
  }
}

bool G_Controller_Keyboard_HandleEvent(g_controller_t *controller, const SDL_Event *event);
bool G_Controller_Gamepad_HandleEvent(g_controller_t *controller, const SDL_Event *event);

g_controller_t *G_Controller_Create(g_controller_type type) {
  g_controller_t *controller = malloc(sizeof(g_controller_t));
  SDL_assert(controller != NULL);
  controller->device = NULL;
  controller->device_id = -1;
  controller->input_state = (g_controller_input_state_t){
    .movement = {0, 0},
    .thrust = 0,
  };

  switch (type) {
    case CONTROLLER_KEYBOARD_1:
      controller->handle_event = G_Controller_Keyboard_HandleEvent;
      controller->keymap = &KEYBOARD1_KEYMAP;
      break;
    case CONTROLLER_KEYBOARD_2:
      controller->handle_event = G_Controller_Keyboard_HandleEvent;
      controller->keymap = &KEYBOARD2_KEYMAP;
      break;
    case CONTROLLER_MOUSE:
      SDL_assert(false);
    case CONTROLLER_GAMEPAD_1:
      SDL_assert(SDL_NumJoysticks() > 0);
      SDL_assert(SDL_IsGameController(0));
      SDL_GameController *gamecontroller = SDL_GameControllerOpen(0);
      SDL_assert(gamecontroller);
      M_Log("[Game/Controller] Found game controller \"%s\"\n", SDL_GameControllerName(gamecontroller));
      controller->handle_event = G_Controller_Gamepad_HandleEvent;
      controller->device = gamecontroller;
      controller->device_id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gamecontroller));
      SDL_GameControllerEventState(SDL_ENABLE);
      break;
  }

  return controller;
}

void G_Controller_Free(g_controller_t *controller) {
  if (controller) {
    // TODO: free/close device
    free(controller);
  }
}

bool G_Controller_Keyboard_HandleEvent(g_controller_t *controller, const SDL_Event *event) {
  if (event->type != SDL_KEYDOWN && event->type != SDL_KEYUP) {
    // Not a key event, ignore.
    return false;
  }

  const g_controller_keymap_t *keymap = controller->keymap;
  SDL_assert(keymap != NULL);

  SDL_Scancode scancode = event->key.keysym.scancode;
  bool pressed = event->type == SDL_KEYDOWN;
  g_controller_input_state_t *input_state = &controller->input_state;

  if (scancode == keymap->thrust) {
    input_state->thrust = pressed ? 1.0f : 0.0f;
  }

  if (scancode == keymap->turn_left) {
    input_state->movement.x = pressed ? -1.0f : 0.0f;
  } else if (scancode == keymap->turn_right) {
    input_state->movement.x = pressed ? 1.0f : 0.0f;
  }

  // TODO: We need to return false here, otherwise the other keyboard controller
  // can't process keys.
  return false;
}

// A 10% deadzone.
const int16_t CONTROLLER_DEADZONE = 3276;

float normalize_gamepad_axis(int16_t value) {
  if (abs(value) < CONTROLLER_DEADZONE) {
    return 0.0f;
  } else {
    return value / 32768.0f;
  }
}

bool G_Controller_Gamepad_HandleEvent(g_controller_t *controller, const SDL_Event *event) {
  if (event->type != SDL_CONTROLLERAXISMOTION
    && event->type != SDL_CONTROLLERBUTTONDOWN
    && event->type != SDL_CONTROLLERBUTTONUP
    && event->type != SDL_JOYAXISMOTION) {
    return false;
  }

  g_controller_input_state_t *input_state = &controller->input_state;

  switch (event->type) {
    case SDL_CONTROLLERAXISMOTION:
      if (event->caxis.which != controller->device_id) {
        // The event is not for this game controller;
        return false;
      }
      switch (event->caxis.axis) {
        case SDL_CONTROLLER_AXIS_LEFTX:
          input_state->movement.x = normalize_gamepad_axis(event->caxis.value);
          break;
        case SDL_CONTROLLER_AXIS_LEFTY:
          input_state->thrust = -normalize_gamepad_axis(SDL_min(0, event->caxis.value));
          break;
      }
  }

  return true;
}
