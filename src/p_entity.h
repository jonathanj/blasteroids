#ifndef P_ENTITY_H
#define P_ENTITY_H

#include "vec2.h"
#include "g_game_state.h"
#include "m_physics.h"

typedef struct p_entity_t p_entity_t;

typedef enum {
  ENTITY_PLAYER,
  ENTITY_ASTEROID,
} p_entity_type_t;

const char *P_EntityManager_NameByType(p_entity_type_t type);

struct p_entity_t {
  p_entity_type_t type;
  m_contact_body_t contact_body;
  double dir_angle;

  // Entity-specific
  void *data;
  void(*think)(p_entity_t *entity, const game_state_t *state);
  bool(*collided)(p_entity_t *entity, p_entity_t *other);
  void(*render)(p_entity_t *entity);
  void(*destroy)(void *data);
};

void P_EntityManager_Init();
p_entity_t *P_EntityManager_Spawn(p_entity_type_t type, m_contact_body_t body, double dir_angle);
void P_EntityManager_Think(const game_state_t *state);
void P_EntityManager_Render();
void P_EntityManager_Destroy(p_entity_t *entity);
void P_EntityManager_Shutdown();

#endif
