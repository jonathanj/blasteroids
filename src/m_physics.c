#include <math.h>

#include "m_physics.h"

// TODO: This doesn't check for wrapped collisions, I wonder how to do that?
bool M_Physics_CheckCircleCollision(m_contact_body_t *a, m_contact_body_t *b, m_contact_t *contact_out) {
  if (a->radius > 0 && b->radius > 0) {
    vec2_t diff = vec2_sub(&a->position, &b->position);
    float dst = fabs(vec2_length(&diff));
    float dst_centers = a->radius + b->radius;
    if (dst < dst_centers) {
      contact_out->a = a;
      contact_out->b = b;
      vec2_t normal = vec2_sub(&a->position, &b->position);
      contact_out->depth = dst_centers - dst;
      vec2_normalize(&normal);
      contact_out->normal = normal;
      return true;
    }
  }
  return false;
}

/// @brief Apply an impulse to a body's velocity.
void M_Physics_ApplyImpulse(m_contact_body_t *body, vec2_t *impulse) {
  vec2_t impulse_with_mass = vec2_mul(impulse, body->inv_mass);
  vec2_iadd(&body->velocity, &impulse_with_mass);
}

/// @brief Resolve a collision between two bodies.
/// @see <https://www.youtube.com/watch?v=1L2g4ZqmFLQ>
/// @see <https://chrishecker.com/images/e/e7/Gdmphys3.pdf>
void M_Physics_ResolveCollision(m_contact_t *contact) {
  m_contact_body_t *a = contact->a;
  m_contact_body_t *b = contact->b;

  // Resolve the overlap
  vec2_t overlap = vec2_mul(&contact->normal, contact->depth * 0.5f);
  vec2_iadd(&a->position, &overlap);
  vec2_isub(&b->position, &overlap);

  // Elasticity
  float e = fminf(a->restitution, b->restitution);

  // Calculate the impulse vector, along the normal.
  vec2_t vrel = vec2_sub(&a->velocity, &b->velocity);
  float j = -(1.0f + e) * vec2_dot(&vrel, &contact->normal) / (a->inv_mass + b->inv_mass);
  vec2_t impulse = vec2_mul(&contact->normal, j);
  vec2_t impulse_neg = vec2_mul(&impulse, -1.0f);

  M_Physics_ApplyImpulse(a, &impulse);
  M_Physics_ApplyImpulse(b, &impulse_neg);
}

/// @brief Wrap a position around the world.
void M_Physics_WrapPosition(vec2_t *position, uint32_t width, uint32_t height) {
  if (position->x < 0) {
    position->x += width;
  }
  if (position->x >= width) {
    position->x -= width;
  }

  if (position->y < 0) {
    position->y += height;
  }
  if (position->y >= height) {
    position->y -= height;
  }
}

// FIXME: This should be a per-body thing.
// TODO: Consider imposing a maximum limit by introducing friction, or resistance that naturally imposes a limit.
const float MAX_SPEED_FIXME = 200;

void M_Physics_SimulateStep(m_contact_body_t *body, const world_t *world, float delta_time) {
  // TODO: derive accel as a function of mass and force / thrust.
  vec2_t new_vel = vec2_mul(&body->accel, delta_time);
  vec2_iadd(&new_vel, &body->velocity);

  // Apply maximum maximum speed limit.
  float speed = vec2_length(&new_vel);
  if (speed > MAX_SPEED_FIXME) {
    vec2_normalize(&new_vel);
    vec2_imul(&new_vel, MAX_SPEED_FIXME);
  }

  body->velocity = new_vel;

  vec2_imul(&new_vel, delta_time);
  vec2_iadd(&body->position, &new_vel);
  M_Physics_WrapPosition(&body->position, world->width, world->height);
}
