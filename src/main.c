#define SDL_MAIN_HANDLED

#include "g_game_state.h"
#include "r_renderer.h"
#include "k_keyboard.h"
#include "w_window.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define FPS 30

void GameLoop(game_state_t *game_state, player_t *player) {
  while (game_state->is_running) {
    G_FrameStart();

    K_HandleEvents(game_state, player);
    P_Think(player, game_state);

    R_Render(game_state, player);

    G_FrameEnd(game_state);
  }
}

//int main(int argc, char *argv[]) {
int main() {
  game_state_t game_state = G_Init(SCREEN_WIDTH, SCREEN_HEIGHT, FPS);
  player_t player = P_Init(100, 100, 0);

  K_InitKeymap();
  if (!W_Init(SCREEN_WIDTH, SCREEN_HEIGHT)) {
    return 2;
  }
  if (!R_Init(W_Get(), &game_state)) {
    return 1;
  }

  GameLoop(&game_state, &player);

  R_Shutdown();
  W_Shutdown();

  return 0;
}
