#include <stdio.h>
#include "golf.h"

Item make_integer(int64_t int_val) {
  Item item = {TYPE_INTEGER, .int_val = int_val};
  return item;
}

void output_item(Item *item) {
  if (item->type == TYPE_INTEGER) {
    printf("%lld", item->int_val);
  }
}
