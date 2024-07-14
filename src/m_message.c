#include <stdarg.h>
#include <stdio.h>

#include "m_message.h"

void M_Log(const char *fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  vprintf(fmt, arg);
  va_end(arg);
}
