#define SDL_MAIN_HANDLED

#include <time.h>

#include "g_game_state.h"
#include "g_controller.h"
#include "r_renderer.h"
#include "k_keyboard.h"
#include "w_window.h"
#include "p_entity.h"

// TODO: remove this
#include "p_asteroid.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define FPS 60

void GameLoop(game_state_t *game_state) {
  while (game_state->is_running) {
    G_FrameStart();

    // K_HandleEvents(game_state, player);
    G_HandleEvents(game_state);
    // P_Think(player, game_state);
    P_EntityManager_Think(game_state);

    R_Render(game_state, NULL);

    G_FrameEnd(game_state);
  }
}

float random_float() {
  return (float)rand() / (float)RAND_MAX;
}

int main() {
  srand((uint32_t)time(NULL));

  game_state_t game_state = G_Init(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FPS);
  // player_t player = P_Init(200, 200, 0);

  G_Controllers_Init();
  g_controller_t *keyboard_1 = G_Controller_Create(CONTROLLER_KEYBOARD_1);
  G_Controllers_Attach(keyboard_1);
  g_controller_t *keyboard_2 = G_Controller_Create(CONTROLLER_KEYBOARD_2);
  G_Controllers_Attach(keyboard_2);

  // TODO: Attach the controller to the player(s)

  K_InitKeymap();
  if (!W_Init(SCREEN_WIDTH, SCREEN_HEIGHT)) {
    return 2;
  }
  if (!R_Init(W_Get(), &game_state)) {
    return 1;
  }

  P_EntityManager_Init();

  // TODO: remove this temp code
  for (uint32_t i = 0; i < 5; ++i) {
    float x = random_float() * game_state.screen_width;
    float y = random_float() * game_state.screen_height;
    float vx = (random_float() * 2.0f - 1.0f) * 50;
    float vy = (random_float() * 2.0f - 1.0f) * 50;
    P_Asteroid_Spawn((vec2_t) {
      x, y
    }, (vec2_t) {
        vx, vy
      }, 10);
  }

  P_Player_Spawn("Emma", keyboard_2, (vec2_t) {
    400, 200
  }, 0xFFFF0000);

  P_Player_Spawn("Bob", keyboard_1, (vec2_t) {
    200, 200
  }, 0xFF00FF00);

  GameLoop(&game_state);

  G_Controllers_Shutdown();
  P_EntityManager_Shutdown();
  R_Shutdown();
  W_Shutdown();

  return 0;
}
