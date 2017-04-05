// builtins.c
// Contains functions implementing golfscript's builtin functions

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

void builtin_ampersand() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  coerce_types(&item1, &item2);
  if (item1.type == TYPE_INTEGER) {
    item2.int_val &= item1.int_val;
  }
  else if (item1.type == TYPE_BLOCK || item1.type == TYPE_STRING) {
    string_setwise_and(&item2.str_val, &item1.str_val);
    free_item(&item1);
  }
  else if (item1.type == TYPE_ARRAY) {
    array_and(&item2.arr_val, &item1.arr_val);
    free_item(&item1);
  }

  stack_push(item2);
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

void builtin_bar() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  coerce_types(&item1, &item2);

  if (item1.type == TYPE_INTEGER) {
    item2.int_val |= item1.int_val;
  }
  else if (item1.type == TYPE_STRING || item1.type == TYPE_BLOCK) {
    string_setwise_or(&item2.str_val, &item1.str_val);
    free(item1.str_val.str_data);
  }
  else if (item1.type == TYPE_ARRAY) {
    array_or(&item2.arr_val, &item1.arr_val);
    free_item(&item1);
  }

  stack_push(item2);
}

void builtin_caret() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  coerce_types(&item1, &item2);
  if (item1.type == TYPE_INTEGER) {
    item2.int_val ^= item1.int_val;
  }
  else if (item1.type == TYPE_STRING || item1.type == TYPE_BLOCK) {
    string_setwise_xor(&item2.str_val, &item1.str_val);
    free(item1.str_val.str_data);
  }
  else if (item1.type == TYPE_ARRAY) {
    array_xor(&item2.arr_val, &item1.arr_val);
    free_item(&item1);
  }

  stack_push(item2);
}

void builtin_comma() {
  Item item = stack_pop();
  if (item.type == TYPE_INTEGER) {
    Item array = make_array();
    for (int64_t i = 0; i < item.int_val; i++) {
      array_push(&array.arr_val, make_integer(i));
    }
    stack_push(array);
  }
  else if (item.type == TYPE_STRING) {
    stack_push(make_integer(item.str_val.length));
  }
  else if (item.type == TYPE_ARRAY) {
    stack_push(make_integer(item.arr_val.length));
  }
  else if (item.type == TYPE_BLOCK) {
    Item to_map = stack_pop();
    if (to_map.type != TYPE_ARRAY) {
      fprintf(stderr, "Error! Operand for , must be array!\n");
      exit(1);
    }
    uint32_t items_removed = 0;
    for (uint32_t i = 0; i < to_map.arr_val.length; i++) {
      stack_push(make_copy(&to_map.arr_val.items[i]));
      execute_string(&item.str_val);
      Item mapped_item = stack_pop();
      if (item_boolean(&mapped_item)) {
        to_map.arr_val.items[i - items_removed] = to_map.arr_val.items[i];
      }
      else {
        items_removed++;
        free_item(&to_map.arr_val.items[i]);
      }
      free_item(&mapped_item);
    }
    to_map.arr_val.length -= items_removed;
    stack_push(to_map);
  }
  free_item(&item);
}

void builtin_do() {
  Item block = stack_pop();

  execute_item(&block);
  Item cond = stack_pop();
  while (item_boolean(&cond)) {
    free_item(&cond);
    execute_item(&block);
    cond = stack_pop();
  }

  free_item(&cond);
  free_item(&block);
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

void builtin_minus() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  coerce_types(&item1, &item2);

  if (item2.type == TYPE_INTEGER) {
    item2.int_val -= item1.int_val;
  }
  else if (item2.type == TYPE_STRING || item2.type == TYPE_BLOCK) {
    string_subtract(&item2.str_val, &item1.str_val);
    free(item1.str_val.str_data);
  }
  else if (item2.type == TYPE_ARRAY) {
    array_subtract(&item2.arr_val, &item1.arr_val);
    free_item(&item1);
  }
  stack_push(item2);
}

void builtin_lparen() {
  Item item = stack_pop();

  if (item.type == TYPE_INTEGER) {
    item.int_val--;
    stack_push(item);
  }
  else if (item.type == TYPE_STRING || item.type == TYPE_BLOCK) {
    if (item.str_val.length == 0) {
      fprintf(stderr, "Error! Unable to uncons from empty %s!",
              item.type == TYPE_STRING ? "string": "block");
      exit(1);
    }
    Item new_item = make_integer(item.str_val.str_data[0]);
    for (uint32_t i = 0; i < item.str_val.length; i++) {
      item.str_val.str_data[i] = item.str_val.str_data[i + 1];
    }
    item.str_val.length--;
    stack_push(item);
    stack_push(new_item);
  }
  else if (item.type == TYPE_ARRAY) {
    if (item.arr_val.length == 0) {
      fprintf(stderr, "Error! Unable to uncons from empty array");
      exit(1);
    }
    Item new_item = item.arr_val.items[0];
    for (uint32_t i = 0; i < item.arr_val.length; i++) {
      item.arr_val.items[i] = item.arr_val.items[i + 1];
    }
    item.arr_val.length--;
    stack_push(item);
    stack_push(new_item);
  }
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

void builtin_rand() {
  Item item = stack_pop();
  if (item.type != TYPE_INTEGER) {
    fprintf(stderr, "Error! rand requires an integer!\n");
    exit(1);
  }
  else {
    item.int_val = rand() % item.int_val;
    stack_push(item);
  }
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

void builtin_rparen() {
  Item item = stack_pop();

  if (item.type == TYPE_INTEGER) {
    item.int_val++;
    stack_push(item);
  }
  else if (item.type == TYPE_STRING || item.type == TYPE_BLOCK) {
    if (item.str_val.length == 0) {
      fprintf(stderr, "Error! Unable to uncons from empty %s!",
              item.type == TYPE_STRING ? "string": "block");
      exit(1);
    }
    String *str = &item.str_val;
    Item new_item = make_integer(str->str_data[str->length - 1]);
    str->length -= 1;
    str->str_data[str->length] = '\0';

    stack_push(item);
    stack_push(new_item);
  }
  else if (item.type == TYPE_ARRAY) {
    if (item.arr_val.length == 0) {
      fprintf(stderr, "Error! Unable to uncons from empty array");
      exit(1);
    }
    Item new_item = item.arr_val.items[item.arr_val.length - 1];
    item.arr_val.length -= 1;
    stack_push(item);
    stack_push(new_item);
  }
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
