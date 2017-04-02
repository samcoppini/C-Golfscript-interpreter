#include "golf.h"

void builtin_semicolon() {
  Item item = stack_pop();
  free_item(&item);
}
