#ifndef M_PHYSICS_H
#define M_PHYSICS_H

#include <stdint.h>
#include <stdbool.h>

#include "g_game_state.h"
#include "vec2.h"

typedef struct {
  vec2_t position;
  vec2_t velocity;
  vec2_t accel;

  // Collision response.
  float restitution;
  float inv_mass;

  // Collision detection.
  // TODO: This assumes a circle, which is fine for for now.
  float radius;
} m_contact_body_t;

typedef struct {
  m_contact_body_t *a;
  m_contact_body_t *b;
  vec2_t normal;
} m_contact_t;

bool M_Physics_CheckCircleCollision(m_contact_body_t *a, m_contact_body_t *b, m_contact_t *contact_out);
void M_Physics_ResolveCollision(m_contact_t *contact);
void M_Physics_SimulateStep(m_contact_body_t *body, const world_t *world, float delta_time);

#endif
