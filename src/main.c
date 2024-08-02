#define SDL_MAIN_HANDLED

#include <time.h>

#include "g_game_state.h"
#include "r_renderer.h"
#include "k_keyboard.h"
#include "w_window.h"
#include "p_entity.h"

// TODO: remove this
#include "p_asteroid.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define FPS 30

void GameLoop(game_state_t *game_state, player_t *player) {
  while (game_state->is_running) {
    G_FrameStart();

    // input_state_t *input_state = K_HandleEvents(game_state, player);
    K_HandleEvents(game_state, player);
    P_Think(player, game_state);
    P_EntityManager_Think(game_state);

    R_Render(game_state, player);

    G_FrameEnd(game_state);
  }
}

float random_float() {
  return (float)rand() / (float)RAND_MAX;
}

//int main(int argc, char *argv[]) {
int main() {
  srand((uint32_t)time(NULL));

  game_state_t game_state = G_Init(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FPS);
  player_t player = P_Init(200, 200, 0);

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

  P_Player_Spawn("Bob", (vec2_t) {
    400, 200
  });

  GameLoop(&game_state, &player);

  P_EntityManager_Shutdown();
  R_Shutdown();
  W_Shutdown();

  return 0;
}
