#include "p_player.h"
#include "p_util.h"
#include "p_entity.h"
#include "m_message.h"
#include "r_renderer.h"

#define PLAYER_DEATH_INVULNERABLE_TIME_SECONDS 2.0f;

typedef struct {
  const g_controller_t *controller;
  const char *name;
  uint32_t color;
  float invulnerable_time;
} p_player_t;

void P_Player_Destroy(void *data);
void P_Player_Think(p_entity_t *entity, const game_state_t *state);
bool P_Player_Collided(p_entity_t *entity, p_entity_t *other);
void P_Player_Render(p_entity_t *entity);

void P_Player_Spawn(const char *name, const g_controller_t *controller, vec2_t position, uint32_t color) {
  vec2_t velocity = { 0, 0 };
  m_contact_body_t body = {
    .accel = {0, 0},
    .position = position,
    .velocity = velocity,
    .restitution = 0.3f,
    .inv_mass = 1 / (float)5.0f,
    .radius = 5.0f,
  };
  p_entity_t *entity = P_EntityManager_Spawn(ENTITY_PLAYER, body, 0.0);
  if (entity == NULL) {
    M_Log("[Entity/Player] Unable to spawn player entity\n");
    return;
  }

  p_player_t *player = malloc(sizeof(p_player_t));
  if (player == NULL) {
    M_Log("[Entity/Player] Unable to spawn player\n");
    return;
  }
  player->controller = controller;
  player->name = name;
  player->color = color;
  player->invulnerable_time = 0.0f;
  entity->data = (void *)player;

  entity->destroy = P_Player_Destroy;
  entity->render = P_Player_Render;
  entity->think = P_Player_Think;
  entity->collided = P_Player_Collided;
}

// TODO: move these somewhere
#define M_PI    3.14159265358979323846264338327950288
#define M_HALF_PI 1.57079632679
const float PLAYER_TURN_RATE = M_PI;
const float PLAYER_ACCEL_RATE = 100;

void P_Player_Think(p_entity_t *entity, const game_state_t *state) {
  p_player_t *player = entity->data;
  SDL_assert(player != NULL);

  float dt = state->delta_time;

  if (player->invulnerable_time > 0.0f) {
    player->invulnerable_time = SDL_max(player->invulnerable_time - dt, 0);
  }

  // Process input.
  const g_controller_input_state_t *input_state = &player->controller->input_state;
  float thrust = input_state->thrust;
  if (thrust > 0) {
    float theta = entity->dir_angle - M_HALF_PI;
    entity->contact_body.accel.x = thrust * PLAYER_ACCEL_RATE * SDL_cos(theta);
    entity->contact_body.accel.y = thrust * PLAYER_ACCEL_RATE * SDL_sin(theta);
  } else {
    entity->contact_body.accel.x = 0;
    entity->contact_body.accel.y = 0;
  }

  entity->dir_angle += input_state->movement.x * PLAYER_TURN_RATE * dt;
}

bool P_Player_Collided(p_entity_t *entity, p_entity_t *other) {
  p_player_t *player = entity->data;
  SDL_assert(player != NULL);

  if (player->invulnerable_time > 0) {
    // Player is invulnerable, ignore collisions.
    return false;
  }

  switch (other->type) {
    case ENTITY_ASTEROID:
      // TODO: Implement real logic.
      entity->dir_angle += 0.39;
      player->invulnerable_time = PLAYER_DEATH_INVULNERABLE_TIME_SECONDS;
      return true;
    case ENTITY_PLAYER:
      return true;
    default:
      return false;
  }
}

void P_Player_Render(p_entity_t *entity) {
  p_player_t *player = entity->data;
  SDL_assert(player != NULL);
  m_contact_body_t *body = &entity->contact_body;

  // Draw the thruster.
  if (vec2_length(&body->accel) > 0) {
    vec2_t vt[] = {
      {-2.5f,  5.5f},
      { 0.0f,  10.0f},
      { 2.5f,  5.5f}
    };
    size_t vt_count = sizeof(vt) / sizeof(vec2_t);
    for (size_t i = 0; i < vt_count; ++i) {
      vec2_irotate(&vt[i], entity->dir_angle);
      vec2_iadd(&vt[i], &body->position);
    }
    R_DrawWireframe(vt, vt_count, 0xFF00FFFF);
  }

  // Draw the ship.
  vec2_t vs[] = {
    {0.0f, -10.0f},
    {5.0f, 5.0f},
    {-5.f, 5.0f}
  };
  size_t vs_count = sizeof(vs) / sizeof(vec2_t);
  for (size_t i = 0; i < vs_count; ++i) {
    vec2_irotate(&vs[i], entity->dir_angle);
    vec2_iadd(&vs[i], &body->position);
  }
  uint32_t player_color = player->invulnerable_time > 0 ? 0xFFFFFFFF : player->color;
  R_DrawWireframe(vs, vs_count, player_color);

  uint32_t name_x = (strlen(player->name) / 2.0f) * 6;
  R_DrawText(body->position.x - name_x, body->position.y + 10, player->name);

  // DEBUG: Draw collision circle.
  // R_DrawCircle(body->position.x, body->position.y, body->radius, 0xFFFF00FF);
}

void P_Player_Destroy(void *data) {
  p_player_t *player = (p_player_t *)data;

  if (player) {
    free(player);
  }
}

///////////////

#define PLAYER_MAX_SPEED 200

player_t P_Init(double x, double y, double angle) {
  player_t player = {
    .dir_angle = angle,
    .pos = {x, y},
    .accel = {0.0f, 0.0f},
    .velocity = {0.0f, 0.0f}
  };
  return player;
}

void P_Think(player_t *player, game_state_t *state) {
  float delta_time = state->delta_time;

  // Update physics.
  vec2_t velocity = vec2_mul(&player->accel, delta_time);
  vec2_iadd(&velocity, &player->velocity);
  if (vec2_length(&velocity) <= PLAYER_MAX_SPEED) {
    player->velocity = velocity;
  }

  vec2_imul(&velocity, delta_time);
  vec2_iadd(&player->pos, &velocity);
  // P_Wrap_Position(&player->pos, state->screen_width, state->screen_height);

  // float screen_width = state->screen_width;
  // float screen_height = state->screen_height;
  // player->pos.x += player->velocity.x * delta_time;
  // if (player->pos.x < 0) {
  //   player->pos.x += screen_width;
  // }
  // if (player->pos.x >= screen_width) {
  //   player->pos.x -= screen_width;
  // }

  // player->pos.y += player->velocity.y * delta_time;
  // if (player->pos.y < 0) {
  //   player->pos.y += screen_height;
  // }
  // if (player->pos.y >= screen_height) {
  //   player->pos.y -= screen_height;
  // }
}
