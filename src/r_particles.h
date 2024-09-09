#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "vec2.h"
#include "m_util.h"

typedef struct {
  bool active;
  vec2_t position;
  vec2_t velocity;
  float lifetime;
  float age;
  m_color_ramp_t color;
} r_particle_t;

void R_Particles_Init();
void R_Particles_Emit(
  uint32_t count,
  vec2_t position,
  vec2_t velocity,
  vec2_t velocity_variation,
  float lifetime,
  float lifetime_variation,
  m_color_ramp_t color
);
void R_Particles_Update(float dt);
void R_Particles_Render();
