#ifndef VEC2_H
#define VEC2_H

typedef struct {
  float x;
  float y;
} vec2_t;

vec2_t vec2_new(float x, float y);
vec2_t vec2_add(const vec2_t *a, const vec2_t *b);
void vec2_iadd(vec2_t *v, const vec2_t *b);
vec2_t vec2_sub(vec2_t *a, vec2_t *b);
vec2_t vec2_mul(vec2_t *v, float factor);
vec2_t vec2_div(vec2_t *v, float factor);
float vec2_length(vec2_t *v);
float vec2_dot(vec2_t *a, vec2_t *b);
float vec2_cross(vec2_t *a, vec2_t *b);
void vec2_normalize(vec2_t *v);

vec2_t vec2_rotate(const vec2_t *v, float angle);
void vec2_irotate(vec2_t *v, float angle);

#endif
