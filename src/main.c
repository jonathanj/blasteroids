#define SDL_MAIN_HANDLED

#include <time.h>

#include "g_game_state.h"
#include "g_controller.h"
#include "r_renderer.h"
#include "w_window.h"
#include "p_entity.h"
#include "m_util.h"

// TODO: remove this
#include "p_player.h"
#include "p_asteroid.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960
#define FPS 60

void GameLoop(game_state_t *game_state) {
  while (game_state->is_running) {
    G_FrameStart(game_state);

    G_HandleEvents(game_state);
    P_EntityManager_Think(game_state);

    R_Render(game_state);

    G_FrameEnd(game_state);

    P_EntityManager_Sweep();
  }
}

int main() {
  rand_init();
  game_state_t game_state = G_Init(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FPS);

  if (!W_Init(SCREEN_WIDTH, SCREEN_HEIGHT)) {
    return 2;
  }
  if (!R_Init(W_Get(), &game_state)) {
    return 1;
  }

  G_Controllers_Init();
  // g_controller_t *controller_player_1 = G_Controller_Create(CONTROLLER_GAMEPAD_1);
  g_controller_t *controller_player_1 = G_Controller_Create(CONTROLLER_KEYBOARD_1);
  G_Controllers_Attach(controller_player_1);
  g_controller_t *controller_player_2 = G_Controller_Create(CONTROLLER_KEYBOARD_2);
  G_Controllers_Attach(controller_player_2);
  // g_controller_t *controller_player_3 = G_Controller_Create(CONTROLLER_KEYBOARD_1);
  // G_Controllers_Attach(controller_player_3);

  P_EntityManager_Init();

  // TODO: remove this temp code
  for (uint32_t i = 0; i < 5; ++i) {
    float x = rand_float() * game_state.screen_width;
    float y = rand_float() * game_state.screen_height;
    float vx = (rand_float() * 2.0f - 1.0f) * 50;
    float vy = (rand_float() * 2.0f - 1.0f) * 50;
    P_Asteroid_Spawn((vec2_t) {
      x, y
    }, (vec2_t) {
        vx, vy
      }, 40);
  }

  P_Player_Spawn("Emma", controller_player_1, (vec2_t) {
    400, 200
  }, 0xFFFF0000);

  P_Player_Spawn("Bob", controller_player_2, (vec2_t) {
    200, 200
  }, 0xFF00FF00);

  // P_Player_Spawn("Sam", controller_player_3, (vec2_t) {
  //   300, 200
  // }, 0xFF0000FF);

  GameLoop(&game_state);

  G_Controllers_Shutdown();
  P_EntityManager_Shutdown();
  R_Shutdown();
  W_Shutdown();

  return 0;
}
