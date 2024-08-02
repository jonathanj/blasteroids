#include <stdlib.h>

#include "p_entity.h"
#include "p_asteroid.h"
#include "p_util.h"
#include "m_message.h"
#include "r_renderer.h"

void P_Asteroid_Destroy(void *data);
void P_Asteroid_Think(p_entity_t *entity, const game_state_t *state __attribute__((unused)));
bool P_Asteroid_Collided(p_entity_t *entity, p_entity_t *other);
void P_Asteroid_Render(p_entity_t *entity);

void P_Asteroid_Spawn(vec2_t position, vec2_t velocity, uint32_t size) {
  m_contact_body_t body = {
    .accel = {0, 0},
    .position = position,
    .velocity = velocity,
    .restitution = 0.9f,
    .inv_mass = 1 / ((float)size * 2.0f),
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
}

bool P_Asteroid_Collided(p_entity_t *entity SDL_UNUSED, p_entity_t *other SDL_UNUSED) {
  return true;
}

void P_Asteroid_Render(p_entity_t *entity) {
  p_asteroid_t *asteroid = (p_asteroid_t *)entity->data;
  SDL_assert(asteroid != NULL);

  vec2_t position = entity->contact_body.position;
  R_DrawCircle((uint32_t)position.x, (uint32_t)position.y, asteroid->size, 0xFFFFFFFF);
  //R_DrawText((uint32_t)position.x, (uint32_t)position.y, "%0.f,%0.f", position.x, position.y);
  R_DrawSmallText((uint32_t)position.x, (uint32_t)position.y + 12, "%0.f,%0.f", position.x, position.y);
}

void P_Asteroid_Destroy(void *data) {
  p_asteroid_t *asteroid = (p_asteroid_t *)data;

  if (asteroid != NULL) {
    free(asteroid);
  }
}
