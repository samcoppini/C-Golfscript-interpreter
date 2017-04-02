#include <stdlib.h>
#include "golf.h"

#define ARRAY_INIT_SIZE 8

Array new_array() {
  Array arr = {malloc(ARRAY_INIT_SIZE * sizeof(Item)), 0, ARRAY_INIT_SIZE};
  return arr;
}

void array_push(Array *arr, Item item) {
  if (arr->length >= arr->allocated) {
    arr->allocated <<= 1;
    arr->items = realloc(arr->items, sizeof(Item) * arr->allocated);
  }
  arr->items[arr->length++] = item;
}
