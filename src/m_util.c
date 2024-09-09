#include <time.h>
#include <SDL2/SDL.h>

#include "m_util.h"

// <https://stackoverflow.com/a/1227137>
static uint32_t rand_state[16];
static size_t rand_index = 0;
uint32_t WELLRNG512() {
  unsigned long a, b, c, d;
  a = rand_state[rand_index];
  c = rand_state[(rand_index + 13) & 15];
  b = a ^ c ^ (a << 16) ^ (c << 15);
  c = rand_state[(rand_index + 9) & 15];
  c ^= (c >> 11);
  a = rand_state[rand_index] = b ^ c;
  d = a ^ ((a << 5) & 0xDA442D24UL);
  rand_index = (rand_index + 15) & 15;
  a = rand_state[rand_index];
  rand_state[rand_index] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
  return rand_state[rand_index];
}

// <http://www.cse.yorku.ca/~oz/marsaglia-rng.html>
#define znew (z=36969*(z&65535)+(z>>16))
#define wnew (w=18000*(w&65535)+(w>>16))
#define MWC ((znew<<16)+wnew)
static uint32_t z = 362436069, w = 521288629;

void rand_init() {
  z = (uint32_t)time(NULL);
  w = rand_uint();
}

uint32_t rand_uint() {
  return MWC;
}

float rand_float() {
  return (float)(rand_uint()) / (float)UINT32_MAX;
}

float lerpf(float a, float b, float t) {
  return (a * t) + (b * (1.0f - t));
}

uint32_t lerpi(uint32_t a, uint32_t b, float t) {
  return (a * t) + (b * (1.0f - t));
}

uint8_t lerpi8(uint8_t a, uint8_t b, float t) {
  return (uint8_t)(a + t * (b - a));
}

#define RGBA_A(c) (c & 0xFF)
#define RGBA_B(c) ((c >> 8) & 0xFF)
#define RGBA_G(c) ((c >> 16) & 0xFF)
#define RGBA_R(c) ((c >> 24) & 0xFF)

// Convert sRGB component to linear space
float srgb_to_linear(uint8_t component) {
  float normalized = component / 255.0f;
  if (normalized <= 0.04045f) {
    return normalized / 12.92f;
  } else {
    return powf((normalized + 0.055f) / 1.055f, 2.4f);
  }
}

// Convert linear space component to sRGB
uint8_t linear_to_srgb(float component) {
  float normalized;
  if (component <= 0.0031308f) {
    normalized = component * 12.92f;
  } else {
    normalized = 1.055f * powf(component, 1.0f / 2.4f) - 0.055f;
  }
  return (uint8_t)(normalized * 255.0f + 0.5f);  // Round to nearest integer
}

uint32_t lerp_color_linear(uint32_t color1, uint32_t color2, float t) {
  // Extract RGBA components from the colors and convert to linear space
  float r1 = srgb_to_linear((color1 >> 24) & 0xFF);
  float g1 = srgb_to_linear((color1 >> 16) & 0xFF);
  float b1 = srgb_to_linear((color1 >> 8) & 0xFF);
  uint8_t a1 = color1 & 0xFF;

  float r2 = srgb_to_linear((color2 >> 24) & 0xFF);
  float g2 = srgb_to_linear((color2 >> 16) & 0xFF);
  float b2 = srgb_to_linear((color2 >> 8) & 0xFF);
  uint8_t a2 = color2 & 0xFF;

  // Interpolate each component in linear space
  float r = r1 + t * (r2 - r1);
  float g = g1 + t * (g2 - g1);
  float b = b1 + t * (b2 - b1);
  uint8_t a = (uint8_t)(a1 + t * (a2 - a1));

  // Convert the result back to sRGB space and combine components into a uint32_t color
  return (linear_to_srgb(r) << 24) | (linear_to_srgb(g) << 16) | (linear_to_srgb(b) << 8) | a;
}

uint32_t lerpc(uint32_t a, uint32_t b, float t) {
  // uint8_t r1 = RGBA_R(a), r2 = RGBA_R(b);
  // uint8_t g1 = RGBA_G(a), g2 = RGBA_G(b);
  // uint8_t b1 = RGBA_B(a), b2 = RGBA_B(b);
  // uint8_t a1 = RGBA_A(a), a2 = RGBA_A(b);
  // return (lerpi8(r1, r2, t) << 24) | (lerpi8(g1, g2, t) << 16) | (lerpi8(b1, b2, t) << 8) | lerpi(a1, a2, t);
  return lerp_color_linear(a, b, t);
}

uint32_t sample_color_ramp(const m_color_ramp_t ramp, float t) {
  t = SDL_clamp(t, 0.0f, 1.0f);
  // NOTE: This is for a ramp with 3 stops.
  // if (t > 0.5f) {
  //   return lerpc(ramp[1], ramp[2], (t - 0.5f) / 0.5f);
  // } else {
  //   return lerpc(ramp[0], ramp[1], t / 0.5f);
  // }

  if (t <= 0.25f) {
    return lerpc(ramp[0], ramp[1], t / 0.25f);
  } else if (t <= 0.5f) {
    return lerpc(ramp[1], ramp[2], (t - 0.25f) / 0.25f);
  } else if (t <= 0.75f) {
    return lerpc(ramp[2], ramp[3], (t - 0.5f) / 0.25f);
  } else {
    return lerpc(ramp[3], ramp[4], (t - 0.75f) / 0.25f);
  }
}
