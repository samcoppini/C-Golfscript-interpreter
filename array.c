// array.c
// Contains functions for manipulating golfscript arrays

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

// Returns the index of an element, or -1 if it's not present in the array
int64_t array_find(Array *array, Item *item) {
  for (uint32_t i = 0; i < array->length; i++) {
    if (items_equal(item, &array->items[i])) {
      return i;
    }
  }
  return -1;
}

void array_reverse(Array *array) {
  for (uint32_t i = 0; i < array->length / 2; i++) {
    Item temp = array->items[i];
    array->items[i] = array->items[array->length - i - 1];
    array->items[array->length - i - 1] = temp;
  }
}

void array_multiply(Array *array, int64_t factor) {
  if (factor < 0) {
    fprintf(stderr, "Error! Cannot multiply array by a negative argument!\n");
    exit(1);
  }
  uint32_t new_len = array->length * factor;
  if (new_len > array->allocated) {
    while (new_len > array->allocated) {
      array->allocated <<= 1;
    }
    array->items = realloc(array->items, sizeof(Item) * array->allocated);
  }
  uint32_t cur_len = array->length;
  while (factor-- > 0) {
    for (uint32_t i = 0; i < array->length; i++) {
      array->items[i + cur_len] = make_copy(&array->items[i]);
    }
    cur_len += array->length;
  }
  array->length = new_len;
}

// Removes array members from array if they are present in to_subtract
void array_subtract(Array *array, Array *to_subtract) {
  uint32_t items_removed = 0;
  Set to_remove = new_set();
  for (uint32_t i = 0; i < to_subtract->length; i++) {
    set_add(&to_remove, &to_subtract->items[i]);
  }
  for (uint32_t i = 0; i < array->length; i++) {
    if (set_has(&to_remove, &array->items[i])) {
      items_removed++;
    }
    else {
      if (items_removed > 0) {
        free_item(&array->items[i - items_removed]);
        array->items[i - items_removed] = array->items[i];
      }
    }
  }
  array->length -= items_removed;
  free_set(&to_remove);
}

void array_split(Array *array, Array *sep) {
  Array split_array = new_array();
  Item cur_array = make_array();
  uint32_t i;
  for (i = 0; i < array->length - sep->length + 1; i++) {
    bool matched_sep = true;
    for (uint32_t j = 0; j < sep->length; j++) {
      if (!items_equal(&array->items[i + j], &sep->items[j])) {
        matched_sep = false;
        break;
      }
    }
    if (matched_sep) {
      array_push(&split_array, cur_array);
      cur_array = make_array();
      for (uint32_t j = 0; j < sep->length; j++) {
        free_item(&array->items[i + j]);
      }
      i += sep->length - 1;
    }
    else {
      array_push(&cur_array.arr_val, array->items[i]);
    }
  }
  while (i < array->length) {
    array_push(&cur_array.arr_val, array->items[i++]);
  }
  array_push(&split_array, cur_array);
  free(array->items);
  *array = split_array;
}

Item array_split_into_groups(Array *array, int64_t group_len) {
  Item split_array = make_array();
  Item cur_array = make_array();
  if (group_len < 0) {
    array_reverse(array);
    group_len *= -1;
  }
  else if (group_len == 0) {
    fprintf(stderr, "Error! Cannot split into groups of size 0!\n");
    exit(1);
  }
  for (uint32_t i = 0; i < array->length; i++) {
    array_push(&cur_array.arr_val, array->items[i]);
    if (cur_array.arr_val.length == group_len) {
      array_push(&split_array.arr_val, cur_array);
      cur_array = make_array();
    }
  }
  if (cur_array.arr_val.length > 0) {
    array_push(&split_array.arr_val, cur_array);
  }
  else {
    free_item(&cur_array);
  }
  return split_array;
}

void array_step_over(Array *array, int64_t step_size) {
  if (step_size == 0) {
    fprintf(stderr, "Error! Cannot select elements with 0 step size!\n");
    exit(1);
  }
  else if (step_size < 0) {
    array_reverse(array);
    step_size *= -1;
  }
  if (step_size == 1) {
    return;
  }

  for (uint32_t i = 1; i * step_size < array->length; i++) {
    free_item(&array->items[i]);
    array->items[i] = array->items[i * step_size];
  }
  for (uint32_t i = (array->length + 1) / step_size; i < array->length; i++) {
    free_item(&array->items[i]);
  }
  array->length = (array->length + 1) / step_size;
}

// Replaces array with the intersection of array and to_and
void array_and(Array *array, Array *to_and) {
  Set can_add = new_set();
  for (uint32_t i = 0; i < to_and->length; i++) {
    set_add(&can_add, &to_and->items[i]);
  }
  uint32_t items_removed = 0;
  for (uint32_t i = 0; i < array->length; i++) {
    if (!set_has(&can_add, &array->items[i])) {
      items_removed++;
    }
    else {
      set_remove(&can_add, &array->items[i]);
      if (items_removed > 0) {
        free_item(&array->items[i - items_removed]);
        array->items[i - items_removed] = array->items[i];
      }
    }
  }
  array->length -= items_removed;
  free_set(&can_add);
}

// Replaces array with the union of array and to_or
void array_or(Array *array, Array *to_or) {
  uint32_t items_removed = 0;
  Set already_added = new_set();
  for (uint32_t i = 0; i < array->length; i++) {
    if (!set_has(&already_added, &array->items[i])) {
      set_add(&already_added, &array->items[i]);
      if (items_removed > 0) {
        free_item(&array->items[i - items_removed]);
        array->items[i - items_removed] = array->items[i];
      }
    }
    else {
      items_removed++;
    }
  }
  array->length -= items_removed;
  for (uint32_t i = 0; i < to_or->length; i++) {
    if (!set_has(&already_added, &to_or->items[i])) {
      set_add(&already_added, &to_or->items[i]);
      array_push(array, make_copy(&to_or->items[i]));
    }
  }
  free_set(&already_added);
}

// Calculates the symmteric difference of two arrays and store the result
// in array
void array_xor(Array *array, Array *to_xor) {
  Set first_set  = new_set();
  Set second_set = new_set();
  for (uint32_t i = 0; i < to_xor->length; i++) {
    set_add(&second_set, &to_xor->items[i]);
  }
  uint32_t items_removed = 0;
  for (uint32_t i = 0; i < array->length; i++) {
    if (!set_has(&first_set, &array->items[i])) {
      set_add(&first_set, &array->items[i]);
      if (!set_has(&second_set, &array->items[i])) {
        if (items_removed > 0) {
          free_item(&array->items[i - items_removed]);
          array->items[i - items_removed] = array->items[i];
        }
        continue;
      }
    }
    items_removed++;
  }
  array->length -= items_removed;
  for (uint32_t i = 0; i < to_xor->length; i++) {
    if (!set_has(&first_set, &to_xor->items[i]) &&
         set_has(&second_set, &to_xor->items[i]))
    {
      set_remove(&second_set, &to_xor->items[i]);
      array_push(array, make_copy(&to_xor->items[i]));
    }
  }

  free_set(&first_set);
  free_set(&second_set);
}
