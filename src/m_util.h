#pragma once

#include <stdint.h>

#define M_PI      3.14159265358979323846264338327950288
#define M_2PI     6.28318530717958647692528676655900576
#define M_HALF_PI 1.57079632679489661923132169163975144

void rand_init();
uint32_t rand_uint();
float rand_float();
float lerpf(float a, float b, float t);
uint32_t lerpi(uint32_t a, uint32_t b, float t);
uint32_t lerpc(uint32_t a, uint32_t b, float t);

#define COLOR_RAMP_MAX_COLORS 5
typedef uint32_t m_color_ramp_t[COLOR_RAMP_MAX_COLORS];

uint32_t sample_color_ramp(const m_color_ramp_t ramp, float t);
