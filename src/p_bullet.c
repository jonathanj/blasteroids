#include <stdlib.h>
#include <SDL2/SDL.h>

#include "p_bullet.h"
#include "p_entity.h"
#include "m_message.h"
#include "r_renderer.h"
#include "r_particles.h"

const float BULLET_SPEED = 220.0f;
const float BULLET_RADIUS = 1.0f;
const uint32_t BULLET_TTL = 2000;

typedef struct {
  uint32_t created;
} p_bullet_t;

void P_Bullet_Think(p_entity_t *entity, const game_state_t *state);
bool P_Bullet_Collided(p_entity_t *entity, p_entity_t *other, const m_contact_t *contact);
void P_Bullet_Render(p_entity_t *entity);
void P_Bullet_Destroy(void *data);

void P_Bullet_Spawn(vec2_t position, float angle) {
  vec2_t velocity = { BULLET_SPEED * SDL_cos(angle), BULLET_SPEED * SDL_sin(angle) };
  m_contact_body_t body = {
    .accel = {0, 0},
    .position = position,
    .velocity = velocity,
    .friction = 0,
    .restitution = 1.0f,
    .inv_mass = 1.0f,
    .radius = BULLET_RADIUS,
  };
  p_entity_t *entity = P_EntityManager_Spawn(ENTITY_BULLET, body, angle);
  if (entity == NULL) {
    M_Log("[Entity/Bullet] Unable to spawn bullet entity\n");
    return;
  }

  p_bullet_t *bullet = malloc(sizeof(p_bullet_t));
  if (bullet == NULL) {
    M_Log("[Entity/Bullet] Unable to spawn bullet\n");
    return;
  }

  // TODO: This should come from the game state.
  bullet->created = SDL_GetTicks();
  entity->data = (void *)bullet;

  entity->destroy = P_Bullet_Destroy;
  entity->render = P_Bullet_Render;
  entity->think = P_Bullet_Think;
  entity->collided = P_Bullet_Collided;
}


void P_Bullet_Think(p_entity_t *entity, const game_state_t *state SDL_UNUSED) {
  p_bullet_t *bullet = (p_bullet_t *)entity->data;
  SDL_assert(bullet != NULL);

  if (SDL_GetTicks() - bullet->created >= BULLET_TTL) {
    entity->dead = true;
  }
}

m_color_ramp_t ramp_hit = { 0xFF00FFFF, 0xFF00FFFF, 0xFF00FFFF, 0xFF00FFFF, 0x0000FFFF };

bool P_Bullet_Collided(p_entity_t *entity, p_entity_t *other SDL_UNUSED, const m_contact_t *contact) {
  vec2_t vel = entity->contact_body.velocity;
  vec2_imul(&vel, -0.1f);
  vec2_t vel_variation = contact->normal;
  vec2_imul(&vel_variation, 50.0f);
  vec2_t pos = contact->a->position;
  vec2_iadd(&pos, &contact->b->position);
  vec2_imul(&pos, 0.5f);
  R_Particles_Emit(25, pos, vel, vel_variation, 0.35f, 0.5f, ramp_hit);
  entity->dead = true;
  return true;
}

void P_Bullet_Render(p_entity_t *entity) {
  p_bullet_t *bullet = (p_bullet_t *)entity->data;
  SDL_assert(bullet != NULL);

  vec2_t position = entity->contact_body.position;
  // R_DrawCircle((uint32_t)position.x, (uint32_t)position.y, BULLET_RADIUS, 0xFF00FFFF);
  R_FillCircle((uint32_t)position.x, (uint32_t)position.y, BULLET_RADIUS, 0xFF00FFFF);
}

void P_Bullet_Destroy(void *data) {
  p_bullet_t *bullet = (p_bullet_t *)data;

  if (bullet != NULL) {
    free(bullet);
  }
}
