#include <stdlib.h>
#include "golf.h"

extern Array stack;
extern Array bracket_stack;

void builtin_abs() {
  Item item = stack_pop();
  if (item.type == TYPE_INTEGER) {
    if (item.int_val < 0)
      item.int_val *= -1;
    stack_push(item);
  }
  else {
    fprintf(stderr, "Error! Invalid type for abs function!\n");
    exit(1);
  }
}

void builtin_at() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();
  Item item3 = stack_pop();

  stack_push(item2);
  stack_push(item1);
  stack_push(item3);
}

void builtin_backslash() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  stack_push(item1);
  stack_push(item2);
}

void builtin_backtick() {
  Item item = stack_pop();
  Item item_str = {TYPE_STRING, .str_val = get_literal(&item)};
  stack_push(item_str);
  free_item(&item);
}

void builtin_exclamation() {
  Item item = stack_pop();
  stack_push(make_integer(!item_boolean(&item)));
  free_item(&item);
}

void builtin_if() {
  Item false_item = stack_pop();
  Item true_item = stack_pop();
  Item cond = stack_pop();

  if (item_boolean(&cond))
    execute_item(&true_item);
  else
    execute_item(&false_item);

  free_item(&cond);
  free_item(&true_item);
  free_item(&false_item);
}

void builtin_lbracket() {
  array_push(&bracket_stack, make_integer(stack.length));
}

void builtin_period() {
  Item item = stack_pop();
  stack_push(make_copy(&item));
  stack_push(item);
}

void builtin_plus() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  coerce_types(&item1, &item2);

  if (item2.type == TYPE_INTEGER) {
    item2.int_val += item1.int_val;
  }
  else if (item2.type == TYPE_STRING) {
    string_add_str(&item2.str_val, item1.str_val.str_data);
    free(item1.str_val.str_data);
  }
  else if (item2.type == TYPE_BLOCK) {
    string_add_char(&item2.str_val, ' ');
    string_add_str(&item2.str_val, item1.str_val.str_data);
    free(item1.str_val.str_data);
  }
  else if (item2.type == TYPE_ARRAY) {
    for (uint32_t i = 0; i < item1.arr_val.length; i++) {
      array_push(&item2.arr_val, item1.arr_val.items[i]);
    }
    free(item1.arr_val.items);
  }

  stack_push(item2);
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

void builtin_tilde() {
  Item item = stack_pop();
  if (item.type == TYPE_INTEGER) {
    item.int_val = ~item.int_val;
    stack_push(item);
  }
  else if (item.type == TYPE_STRING || item.type == TYPE_BLOCK) {
    execute_string(&item.str_val);
    free(item.str_val.str_data);
  }
  else if (item.type == TYPE_ARRAY) {
    for (uint32_t i = 0; i < item.arr_val.length; i++) {
      stack_push(item.arr_val.items[i]);
    }
    free(item.arr_val.items);
  }
}

void builtin_until() {
  Item body = stack_pop();
  Item cond = stack_pop();

  execute_item(&cond);
  Item bool_item = stack_pop();
  while (!item_boolean(&bool_item)) {
    free_item(&bool_item);
    execute_item(&body);
    execute_item(&cond);
    bool_item = stack_pop();
  }

  free_item(&bool_item);
  free_item(&cond);
  free_item(&body);
}

void builtin_while() {
  Item body = stack_pop();
  Item cond = stack_pop();

  execute_item(&cond);
  Item bool_item = stack_pop();
  while (item_boolean(&bool_item)) {
    free_item(&bool_item);
    execute_item(&body);
    execute_item(&cond);
    bool_item = stack_pop();
  }

  free_item(&bool_item);
  free_item(&cond);
  free_item(&body);
}
