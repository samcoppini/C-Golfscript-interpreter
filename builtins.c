#include "golf.h"

extern Array stack;
extern Array bracket_stack;

void builtin_backslash() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  stack_push(item1);
  stack_push(item2);
}

void builtin_lbracket() {
  array_push(&bracket_stack, make_integer(stack.length));
}

void builtin_period() {
  Item item = stack_pop();
  stack_push(make_copy(&item));
  stack_push(item);
}

void builtin_print() {
  Item item = stack_pop();
  output_item(&item);
  free_item(&item);
}

void builtin_rbracket() {
  uint32_t first_item;
  if (bracket_stack.length == 0) {
    first_item = 0;
  }
  else {
    bracket_stack.length--;
    first_item = bracket_stack.items[bracket_stack.length].int_val;
  }
  Item array = make_array();
  for (uint32_t i = first_item; i < stack.length; i++) {
    array_push(&array.arr_val, stack.items[i]);
  }
  stack.length = first_item;
  stack_push(array);
}

void builtin_semicolon() {
  Item item = stack_pop();
  free_item(&item);
}
