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

void array_subtract(Array *array, Array *to_subtract) {
  uint32_t items_removed = 0;
  for (uint32_t i = 0; i < array->length; i++) {
    bool array_has = false;
    for (uint32_t j = 0; j < to_subtract->length; j++) {
      if (items_equal(&array->items[i], &to_subtract->items[j])) {
        array_has = true;
        break;
      }
    }
    if (array_has) {
      items_removed++;
    }
    else {
      array->items[i - items_removed] = array->items[i];
    }
  }
  array->length -= items_removed;
}

void array_and(Array *array, Array *to_and) {
  uint32_t items_removed = 0;
  for (uint32_t i = 0; i < array->length; i++) {
    bool array_has = false;
    for (uint32_t j = 0; j < to_and->length; j++) {
      if (items_equal(&array->items[i], &to_and->items[j])) {
        array_has = true;
        break;
      }
    }
    if (!array_has) {
      items_removed++;
    }
    else {
      bool array_has = false;
      for (uint32_t j = 0; j < i - items_removed; j++) {
        if (items_equal(&array->items[i], &array->items[j])) {
          array_has = true;
          break;
        }
      }
      if (array_has) {
        items_removed++;
      }
      else {
        array->items[i - items_removed] = array->items[i];
      }
    }
  }
  array->length -= items_removed;
}
