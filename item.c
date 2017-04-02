#include <stdio.h>
#include <stdlib.h>
#include "golf.h"

Item make_integer(int64_t int_val) {
  Item item = {TYPE_INTEGER, .int_val = int_val};
  return item;
}

Item make_string(char *str_val) {
  Item item = {TYPE_STRING, .str_val = create_string(str_val)};
  return item;
}

Item make_builtin(void (*function)()) {
  Item item = {TYPE_FUNCTION, .function = function};
  return item;
}

void free_item(Item *item) {
  if (item->type == TYPE_STRING) {
    free(item->str_val.str_data);
  }
}

void output_item(Item *item) {
  if (item->type == TYPE_INTEGER) {
    printf("%lld", item->int_val);
  }
  else if (item->type == TYPE_STRING) {
    printf("%s", item->str_val);
  }
}
