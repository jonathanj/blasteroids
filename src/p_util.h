#ifndef P_UTIL_H
#define P_UTIL_H

#include <stdint.h>

#include "vec2.h"

void P_Wrap_Position(vec2_t *position, uint32_t width, uint32_t height);

#endif
