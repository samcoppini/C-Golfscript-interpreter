#include "golf.h"

void builtin_period() {
  Item item = stack_pop();
  stack_push(make_copy(&item));
  stack_push(item);
}

void builtin_semicolon() {
  Item item = stack_pop();
  free_item(&item);
}
