#include <math.h>
#include "vec2.h"

vec2_t vec2_new(float x, float y) {
  vec2_t v = { x, y };
  return v;
}

vec2_t vec2_add(const vec2_t *a, const vec2_t *b) {
  vec2_t v = { .x = a->x + b->x, .y = a->y + b->y };
  return v;
}
void vec2_iadd(vec2_t *v, const vec2_t *b) {
  v->x += b->x;
  v->y += b->y;
}

vec2_t vec2_sub(const vec2_t *a, const vec2_t *b) {
  vec2_t v = { .x = a->x - b->x, .y = a->y - b->y };
  return v;
}
vec2_t vec2_mul(const vec2_t *v, float factor) {
  vec2_t u = { .x = v->x * factor, .y = v->y * factor };
  return u;
}
void vec2_imul(vec2_t *v, float factor) {
  v->x *= factor;
  v->y *= factor;
}
vec2_t vec2_div(const vec2_t *v, float factor) {
  vec2_t u = { .x = v->x / factor, .y = v->y / factor };
  return u;
}
float vec2_length(const vec2_t *v) {
  return sqrt(v->x * v->x + v->y * v->y);
}
float vec2_dot(const vec2_t *a, const vec2_t *b) {
  return (a->x * b->x) + (a->y * b->y);
}
float vec2_cross(const vec2_t *a, const vec2_t *b) {
  return (a->x * b->y) - (a->y * b->x);
}
void vec2_normalize(vec2_t *v) {
  float len = vec2_length(v);
  v->x /= len;
  v->y /= len;
}

vec2_t vec2_rotate(const vec2_t *v, float angle) {
  float x = v->x;
  float y = v->y;
  vec2_t rot = { x, y };
  rot.x = x * cos(angle) - y * sin(angle);
  rot.y = x * sin(angle) + y * cos(angle);
  return rot;
}

void vec2_irotate(vec2_t *v, float angle) {
  float x = v->x;
  float y = v->y;
  v->x = x * cos(angle) - y * sin(angle);
  v->y = x * sin(angle) + y * cos(angle);
}

// vec2_t vec2_rotate_about(vec2_t v, vec2_t center, float angle) {
//   vec2_t rot = {v.x, v.y};
//   rot.x -= center.x;
//   rot.y -= center.y;
//   rot.x = v.x * cos(angle) - v.y * sin(angle);
//   rot.y = v.x * sin(angle) + v.y * cos(angle);
//   rot.x += center.x;
//   rot.y += center.y;
//   return rot;
// }
