#include <stdlib.h>

#include "r_particles.h"
#include "r_renderer.h"

#define PARTICLES_POOL_MAX 1000

static r_particle_t particle_pool[PARTICLES_POOL_MAX];
static size_t particle_pool_index = 0;

void R_Particles_Init() {
  memset(particle_pool, 0, sizeof(particle_pool));
}

void R_Particles_Emit(
  uint32_t count,
  vec2_t position,
  vec2_t velocity,
  vec2_t velocity_variation,
  float lifetime,
  float lifetime_variation,
  m_color_ramp_t color
) {
  for (size_t i = 0; i < count; ++i) {
    r_particle_t *particle = &particle_pool[particle_pool_index];
    particle->active = true;
    particle->position = position;
    particle->velocity = velocity;
    particle->velocity.x += velocity_variation.x * (rand_float() - 0.5f);
    particle->velocity.y += velocity_variation.y * (rand_float() - 0.5f);
    particle->lifetime = lifetime;
    particle->color[0] = color[0];
    particle->color[1] = color[1];
    particle->color[2] = color[2];
    particle->color[3] = color[3];
    particle->color[4] = color[4];
    particle->age = lifetime * (lifetime_variation * rand_float());

    particle_pool_index = (particle_pool_index + 1) % PARTICLES_POOL_MAX;
  }
}

void R_Particles_Update(float dt) {
  for (size_t i = 0; i < PARTICLES_POOL_MAX; ++i) {
    r_particle_t *particle = &particle_pool[i];
    if (!particle->active) {
      continue;
    }

    if (particle->age >= particle->lifetime) {
      particle->active = false;
      continue;
    }

    vec2_t vel = particle->velocity;
    vec2_imul(&vel, dt);
    vec2_iadd(&particle->position, &vel);
    particle->age += dt;
  }
}

void R_Particles_Render() {
  for (size_t i = 0; i < PARTICLES_POOL_MAX; ++i) {
    r_particle_t *particle = &particle_pool[i];
    if (!particle->active) {
      continue;
    }

    // TODO: implement color lerping
    float life = SDL_clamp(particle->age / particle->lifetime, 0, 1.0f);
    // uint32_t color = lerpi(particle->color_start, particle->color_end, life);
    // uint32_t color = lerpc(particle->color_start, particle->color_end, life);
    uint32_t color = sample_color_ramp(particle->color, life);
    R_DrawPoint(particle->position.x, particle->position.y, color);
    //uint32_t alpha = ((uint32_t)(life * 255.0f) << 24);
    // R_DrawPoint(particle->position.x, particle->position.y, alpha | (color & 0x00FFFFFF));
  }
}
