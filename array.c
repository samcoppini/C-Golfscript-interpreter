// array.c
// Contains functions for manipulating golfscript arrays

#include <assert.h>
#include <stdlib.h>
#include "golf.h"

#define ARRAY_INIT_SIZE 8

Array new_array() {
  Array arr = {malloc(ARRAY_INIT_SIZE * sizeof(Item)), 0, ARRAY_INIT_SIZE};
  if (arr.items == NULL) {
    error("Unable to allocate space for new array!");
  }
  return arr;
}

void free_array(Array *array) {
  for (uint32_t i = 0; i < array->length; i++) {
    free_item(&array->items[i]);
  }
  free(array->items);
}

// Converts a string into an array of integers
Array array_from_string(const String *str) {
  Array int_array = new_array();
  for (uint32_t i = 0; i < str->length; i++) {
    array_push(&int_array, make_integer(str->str_data[i]));
  }
  return int_array;
}

void array_push(Array *arr, Item item) {
  if (arr->length >= arr->allocated) {
    arr->allocated <<= 1;
    arr->items = realloc(arr->items, sizeof(Item) * arr->allocated);
    if (arr->items == NULL) {
      error("Unable to allocate additional space for array!");
    }
  }
  arr->items[arr->length++] = item;
}

// Removes a number of elements from the array's front
void array_remove_from_front(Array *array, Bigint to_remove) {
  if (to_remove.is_negative || bigint_is_zero(&to_remove))
    return;

  uint32_t to_remove_int;
  if (!bigint_fits_in_uint32(&to_remove))
    to_remove_int = min(UINT32_MAX, array->length);
  else
    to_remove_int = min(bigint_to_uint32(&to_remove), array->length);

  for (uint32_t i = 0; i < to_remove_int; i++) {
    free_item(&array->items[i]);
  }

  array->length -= to_remove_int;

  for (uint32_t i = 0; i < array->length; i++) {
    array->items[i] = array->items[i + to_remove_int];
  }
}

// Returns the index of an element, or -1 if it's not present in the array
int64_t array_find(const Array *array, const Item *item) {
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

// Joins an array's elements with a string or array in between each element
Item join_array(Array *array, const Item *sep) {
  Item joined_array;

  if (sep->type == TYPE_ARRAY)
    joined_array = make_array();
  else if (sep->type == TYPE_STRING)
    joined_array = empty_string();
  else
    assert(false);

  for (uint32_t i = 0; i < array->length; i++) {
    items_add(&joined_array, &array->items[i]);
    if (i + 1 < array->length) {
      Item to_add = make_copy(sep);
      items_add(&joined_array, &to_add);
      free_item(&to_add);
    }
  }

  return joined_array;
}

void map_array(Array *array, Item *block) {
  Array mapped_array = new_array();
  for (uint32_t i = 0; i < array->length; i++) {
    uint32_t start_stack_size = stack.length;
    stack_push(array->items[i]);
    execute_string(&block->str_val);
    for (uint32_t j = start_stack_size; j < stack.length; j++) {
      array_push(&mapped_array, stack.items[j]);
    }
    stack.length = min(stack.length, start_stack_size);
  }
  free(array->items);
  *array = mapped_array;
}

void fold_array(Array *array, Item *block) {
  if (array->length > 0) {
    stack_push(make_copy(&array->items[0]));
    for (uint32_t i = 1; i < array->length; i++) {
      stack_push(make_copy(&array->items[i]));
      execute_string(&block->str_val);
    }
  }
}

void filter_array(Array *array, Item *block) {
  uint32_t items_removed = 0;
  for (uint32_t i = 0; i < array->length; i++) {
    stack_push(make_copy(&array->items[i]));
    execute_string(&block->str_val);
    Item mapped_item = stack_pop();
    if (item_boolean(&mapped_item)) {
      array->items[i - items_removed] = array->items[i];
    }
    else {
      free_item(&array->items[i]);
      items_removed++;
    }
    free_item(&mapped_item);
  }
  array->length -= items_removed;
}

// Removes all empty strings from the array
void array_remove_empty_strings(Array *array) {
  uint32_t removed_elements = 0;
  for (uint32_t i = 0; i < array->length; i++) {
    if (array->items[i].type == TYPE_STRING &&
        array->items[i].str_val.length == 0)
    {
      free_item(&array->items[i]);
      removed_elements++;
    }
    else if (removed_elements > 0) {
      array->items[i - removed_elements] = array->items[i];
    }
  }
  array->length -= removed_elements;
}

// Removes all empty arrays from the array
void array_remove_empty_arrays(Array *array) {
  uint32_t removed_elements = 0;
  for (uint32_t i = 0; i < array->length; i++) {
    if (array->items[i].type == TYPE_ARRAY &&
        array->items[i].arr_val.length == 0)
    {
      free_item(&array->items[i]);
      removed_elements++;
    }
    else if (removed_elements > 0) {
      array->items[i - removed_elements] = array->items[i];
    }
  }
  array->length -= removed_elements;
}

void array_multiply(Array *array, Bigint factor) {
  if (factor.is_negative) {
    error("Cannot multiply array by a negative argument!");
  }
  if (!bigint_fits_in_uint32(&factor)) {
    error("Factor too large to multiply array by!");
  }
  uint32_t to_multiply_by = bigint_to_uint32(&factor);
  uint64_t new_len = array->length * to_multiply_by;
  if (new_len > UINT32_MAX) {
    error("Factor too large to multiply array by!");
  }
  if (new_len > array->allocated) {
    while (new_len > array->allocated) {
      array->allocated <<= 1;
    }
    array->items = realloc(array->items, sizeof(Item) * array->allocated);
    if (array->items == NULL) {
      error("Unable to allocate additional space for array!");
    }
  }
  uint32_t cur_len = array->length;
  while (--to_multiply_by > 0) {
    for (uint32_t i = 0; i < array->length; i++) {
      array->items[i + cur_len] = make_copy(&array->items[i]);
    }
    cur_len += array->length;
  }
  array->length = new_len;
}

// Removes array members from array if they are present in to_subtract
void array_subtract(Array *array, const Array *to_subtract) {
  uint32_t items_removed = 0;
  Set to_remove = new_set();
  for (uint32_t i = 0; i < to_subtract->length; i++) {
    set_add(&to_remove, &to_subtract->items[i]);
  }
  for (uint32_t i = 0; i < array->length; i++) {
    if (set_has(&to_remove, &array->items[i])) {
      items_removed++;
      free_item(&array->items[i]);
    }
    else {
      if (items_removed > 0) {
        array->items[i - items_removed] = array->items[i];
      }
    }
  }
  array->length -= items_removed;
  free_set(&to_remove);
}

void array_split(Array *array, const Array *sep) {
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

void array_split_into_groups(Array *array, Bigint group_len) {
  Array split_array = new_array();
  Item cur_array = make_array();

  if (group_len.is_negative) {
    array_reverse(array);
    group_len.is_negative = false;
  }
  else if (bigint_is_zero(&group_len)) {
    error("Cannot split into groups of size 0!");
  }

  uint32_t group_size;
  if (!bigint_fits_in_uint32(&group_len))
    group_size = UINT32_MAX;
  else
    group_size = bigint_to_uint32(&group_len);

  for (uint32_t i = 0; i < array->length; i++) {
    array_push(&cur_array.arr_val, array->items[i]);
    if (cur_array.arr_val.length == group_size) {
      array_push(&split_array, cur_array);
      cur_array = make_array();
    }
  }
  if (cur_array.arr_val.length > 0) {
    array_push(&split_array, cur_array);
  }
  else {
    free_item(&cur_array);
  }

  free(array->items);
  *array = split_array;
}

void array_step_over(Array *array, Bigint step_size) {
  if (bigint_is_zero(&step_size)) {
    error("Cannot select elements with 0 step size!");
  }
  else if (step_size.is_negative) {
    array_reverse(array);
    step_size.is_negative = false;
  }
  uint32_t step_len;
  if (!bigint_fits_in_uint32(&step_size)) {
    step_len = UINT32_MAX;
  }
  else {
    step_len = bigint_to_uint32(&step_size);
  }

  for (uint32_t i = 1; i < array->length; i++) {
    if (i % step_len) {
      free_item(&array->items[i]);
    }
    else {
      array->items[i / step_len] = array->items[i];
    }
  }

  array->length = ((array->length - 1) / step_len) + 1;
}

int *get_sorted_indexes(const Array *array, uint32_t start, uint32_t end) {
  if (start > end) {
    return NULL;
  }
  int *indexes = malloc(sizeof(uint32_t) * (end - start + 1));
  if (end == start) {
    indexes[0] = start;
  }
  else if (end - start == 1) {
    if (item_compare(&array->items[start], &array->items[end]) <= 0) {
      indexes[0] = start;
      indexes[1] = end;
    }
    else {
      indexes[0] = end;
      indexes[1] = start;
    }
  }
  else {
    uint32_t mid = ((end - start) / 2) + start;
    int *first_half = get_sorted_indexes(array, start, mid);
    int *second_half = get_sorted_indexes(array, mid + 1, end);

    uint32_t first_len = mid - start + 1;
    uint32_t second_len = end - mid;
    uint32_t first_index = 0;
    uint32_t second_index = 0;
    uint32_t joined_pos = 0;

    while (first_index < first_len && second_index < second_len) {
      if (item_compare(&array->items[first_half[first_index]],
                       &array->items[second_half[second_index]]) <= 0)
      {
        indexes[joined_pos++] = first_half[first_index++];
      }
      else {
        indexes[joined_pos++] = second_half[second_index++];
      }
    }

    while (first_index < first_len) {
      indexes[joined_pos++] = first_half[first_index++];
    }
    while (second_index < second_len) {
      indexes[joined_pos++] = second_half[second_index++];
    }

    free(first_half);
    free(second_half);
  }

  return indexes;
}

void array_sort(Array *array) {
  if (array->length <= 1)
    return;

  int *indexes = get_sorted_indexes(array, 0, array->length - 1);
  Array temp = new_array();
  for (uint32_t i = 0; i < array->length; i++) {
    array_push(&temp, array->items[indexes[i]]);
  }

  free(indexes);
  free(array->items);
  *array = temp;
}

void array_sort_by_mapping(Array *array, const Array *mapped_array) {
  if (array->length <= 1)
    return;

  int *indexes = get_sorted_indexes(mapped_array, 0, array->length - 1);
  Array temp = new_array();
  for (uint32_t i = 0; i < array->length; i++) {
    array_push(&temp, array->items[indexes[i]]);
  }

  free(indexes);
  free(array->items);
  *array = temp;
}

void string_sort_by_mapping(String *str, const Array *mapped_str) {
  if (str->length <= 1)
    return;

  int *indexes = get_sorted_indexes(mapped_str, 0, str->length - 1);
  String sorted_str = new_string();
  for (uint32_t i = 0; i < str->length; i++) {
    string_add_char(&sorted_str, str->str_data[indexes[i]]);
  }

  free(indexes);
  free(str->str_data);
  *str = sorted_str;
}

// Replaces array with the intersection of array and to_and
void array_and(Array *array, const Array *to_and) {
  Set can_add = new_set();
  for (uint32_t i = 0; i < to_and->length; i++) {
    set_add(&can_add, &to_and->items[i]);
  }
  uint32_t items_removed = 0;
  for (uint32_t i = 0; i < array->length; i++) {
    if (!set_has(&can_add, &array->items[i])) {
      free_item(&array->items[i]);
      items_removed++;
    }
    else {
      set_remove(&can_add, &array->items[i]);
      if (items_removed > 0) {
        array->items[i - items_removed] = array->items[i];
      }
    }
  }
  array->length -= items_removed;
  free_set(&can_add);
}

// Replaces array with the union of array and to_or
void array_or(Array *array, const Array *to_or) {
  uint32_t items_removed = 0;
  Set already_added = new_set();
  for (uint32_t i = 0; i < array->length; i++) {
    if (!set_has(&already_added, &array->items[i])) {
      set_add(&already_added, &array->items[i]);
      if (items_removed > 0) {
        array->items[i - items_removed] = array->items[i];
      }
    }
    else {
      free_item(&array->items[i]);
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
void array_xor(Array *array, const Array *to_xor) {
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
          array->items[i - items_removed] = array->items[i];
        }
        continue;
      }
    }
    free_item(&array->items[i]);
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
