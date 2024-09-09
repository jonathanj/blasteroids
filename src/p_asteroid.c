#include <stdlib.h>

#include "p_entity.h"
#include "p_asteroid.h"
#include "p_util.h"
#include "m_util.h"
#include "m_message.h"
#include "r_renderer.h"
#include "r_particles.h"

void P_Asteroid_Think(p_entity_t *entity, const game_state_t *state __attribute__((unused)));
bool P_Asteroid_Collided(p_entity_t *entity, p_entity_t *other, const m_contact_t *contact);
void P_Asteroid_Render(p_entity_t *entity);
void P_Asteroid_Destroy(void *data);

#define ASTEROID_MIN_SIZE 4.0f
#define ASTEROID_WIREFRAME_NUM_POINTS 16

typedef struct {
  uint32_t size;
  vec2_t wireframe[ASTEROID_WIREFRAME_NUM_POINTS + 1];
} p_asteroid_t;

void P_Asteroid_GenerateWireframe(p_asteroid_t *asteroid) {
  float r = asteroid->size;
  float angle_delta = M_2PI / (float)(ASTEROID_WIREFRAME_NUM_POINTS);
  float angle = 0.0f;
  float modulation = r / 4.0;
  for (int i = 0; i < ASTEROID_WIREFRAME_NUM_POINTS; ++i) {
    asteroid->wireframe[i].x = rand_float() * modulation + r * SDL_cosf(angle);
    asteroid->wireframe[i].y = rand_float() * modulation + r * SDL_sinf(angle);
    angle += angle_delta;
  }
  asteroid->wireframe[ASTEROID_WIREFRAME_NUM_POINTS].x = asteroid->wireframe[0].x;
  asteroid->wireframe[ASTEROID_WIREFRAME_NUM_POINTS].y = asteroid->wireframe[0].y;
}

void P_Asteroid_Spawn(vec2_t position, vec2_t velocity, uint32_t size) {
  m_contact_body_t body = {
    .accel = {0, 0},
    .position = position,
    .velocity = velocity,
    .friction = 0,
    .restitution = 0.8f,
    .inv_mass = 1 / ((float)size * 5.0f),
    .radius = (float)size,
  };
  p_entity_t *entity = P_EntityManager_Spawn(ENTITY_ASTEROID, body, 0.0);
  if (entity == NULL) {
    M_Log("[Entity/Asteroid] Unable to spawn asteroid entity\n");
    return;
  }

  p_asteroid_t *asteroid = malloc(sizeof(p_asteroid_t));
  if (asteroid == NULL) {
    M_Log("[Entity/Asteroid] Unable to spawn asteroid\n");
    return;
  }

  entity->data = (void *)asteroid;
  entity->destroy = P_Asteroid_Destroy;
  entity->render = P_Asteroid_Render;
  entity->collided = P_Asteroid_Collided;
  asteroid->size = size;
  P_Asteroid_GenerateWireframe(asteroid);
}

m_color_ramp_t ramp_dust = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x7F000000, 0x00000000 };

bool P_Asteroid_Collided(p_entity_t *entity SDL_UNUSED, p_entity_t *other SDL_UNUSED, const m_contact_t *contact SDL_UNUSED) {
  p_asteroid_t *asteroid = entity->data;
  SDL_assert(asteroid != NULL);

  switch (other->type) {
    case ENTITY_ASTEROID:
    case ENTITY_PLAYER:
      return true;
    case ENTITY_BULLET:
      entity->dead = true;
      float new_size = asteroid->size * 0.5f;
      if (new_size >= ASTEROID_MIN_SIZE) {
        vec2_t vel = entity->contact_body.velocity;
        vec2_iadd(&vel, &other->contact_body.velocity);
        float speed = (vec2_length(&vel) * 0.5f) / (entity->contact_body.inv_mass + other->contact_body.inv_mass);
        vec2_normalize(&vel);
        float xxx = 1.0f / sqrtf(2);
        vec2_t a_vel = { vel.x - vel.y, vel.x + vel.y };
        vec2_imul(&a_vel, xxx);
        vec2_t b_vel = { vel.x + vel.y, -vel.x + vel.y };
        vec2_imul(&b_vel, xxx);

        vec2_t pos = entity->contact_body.position;
        vec2_t a_pos = { pos.x + a_vel.x * new_size, pos.y + a_vel.y * new_size };
        vec2_t b_pos = { pos.x + b_vel.x * new_size, pos.y + b_vel.y * new_size };
        vec2_imul(&a_vel, speed * rand_float());
        vec2_imul(&b_vel, speed * rand_float());
        R_Particles_Emit(10, entity->contact_body.position, vec2_new(0, 0), vec2_new(40.0f, 40.0f), 0.75f, 0.5f, ramp_dust);
        P_Asteroid_Spawn(a_pos, a_vel, new_size);
        P_Asteroid_Spawn(b_pos, b_vel, new_size);
      } else {
        R_Particles_Emit(30, entity->contact_body.position, vec2_new(0, 0), vec2_new(150.0f, 150.0f), 1.25f, 0.5f, ramp_dust);
      }
      return false;
    default:
      return false;
  }
}

void P_Asteroid_Render(p_entity_t *entity) {
  p_asteroid_t *asteroid = (p_asteroid_t *)entity->data;
  SDL_assert(asteroid != NULL);

  vec2_t position = entity->contact_body.position;
  // R_DrawCircle((uint32_t)position.x, (uint32_t)position.y, asteroid->size, 0xFFFFFFFF);

  int32_t x0, y0, x1, y1;
  for (int i = 0; i < ASTEROID_WIREFRAME_NUM_POINTS; ++i) {
    x0 = asteroid->wireframe[i].x;
    y0 = asteroid->wireframe[i].y;
    x1 = asteroid->wireframe[i + 1].x;
    y1 = asteroid->wireframe[i + 1].y;
    R_DrawLineAA(position.x + x0, position.y + y0, position.x + x1, position.y + y1, 0xFFFFFFFF);
  }
  // R_DrawWireframe(asteroid->wireframe, ASTEROID_WIREFRAME_NUM_POINTS, 0xFFFFFFFF);
  // R_DrawSmallText((uint32_t)position.x, (uint32_t)position.y + 12, "%0.f,%0.f", position.x, position.y);
}

void P_Asteroid_Destroy(void *data) {
  p_asteroid_t *asteroid = (p_asteroid_t *)data;

  if (asteroid != NULL) {
    free(asteroid);
  }
}
