// error.c
// Contains functions for printing error messages

#include <stdarg.h>
#include <stdlib.h>
#include "golf.h"

noreturn void error(const char *msg, ...) {
  va_list ap;

  va_start(ap, msg);
  fprintf(stderr, "Error! ");
  vfprintf(stderr, msg, ap);
  fprintf(stderr, "\n");
  va_end(ap);

  exit(1);
}
