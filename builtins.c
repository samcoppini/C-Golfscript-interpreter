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

void builtin_asterisk() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  if (item1.type < item2.type)
    swap_items(&item1, &item2);

  if (item2.type == TYPE_INTEGER) {
    if (item1.type == TYPE_INTEGER) {
      item1.int_val *= item2.int_val;
      stack_push(item1);
    }
    else if (item1.type == TYPE_ARRAY) {
      array_multiply(&item1.arr_val, item2.int_val);
      stack_push(item1);
    }
    else if (item1.type == TYPE_STRING) {
      string_multiply(&item1.str_val, item2.int_val);
      stack_push(item1);
    }
    else if (item1.type == TYPE_BLOCK) {
      while (item2.int_val-- > 0) {
        execute_string(&item1.str_val);
      }
      free(item1.str_val.str_data);
    }
  }
  else if (item2.type == TYPE_ARRAY) {
    if (item1.type == TYPE_ARRAY || item1.type == TYPE_STRING) {
      Item joined_array;

      if (item1.type == TYPE_ARRAY)
        joined_array = make_array();
      else
        joined_array = empty_string();

      for (uint32_t i = 0; i < item2.arr_val.length; i++) {
        Item to_add = make_copy(&item2.arr_val.items[i]);
        items_add(&joined_array, &to_add);
        if (i + 1 < item2.arr_val.length) {
          to_add = make_copy(&item1);
          items_add(&joined_array, &to_add);
        }
      }
      free_item(&item1);
      free_item(&item2);
      stack_push(joined_array);
    }
    else if (item1.type == TYPE_BLOCK) {
      if (item2.arr_val.length == 0) {
        fprintf(stderr, "Error! Cannot map over an empty array!\n");
        exit(1);
      }
      else {
        stack_push(item2.arr_val.items[0]);
        for (uint32_t i = 1; i < item2.arr_val.length; i++) {
          stack_push(item2.arr_val.items[i]);
          execute_string(&item1.str_val);
        }
      }
      free(item2.arr_val.items);
      free(item1.str_val.str_data);
    }
  }
  else if (item2.type == TYPE_STRING || item2.type == TYPE_BLOCK) {
    if (item1.type == TYPE_STRING) {
      Item joined_str = empty_string();
      for (uint32_t i = 0; i < item2.str_val.length; i++) {
        string_add_char(&joined_str.str_val, item2.str_val.str_data[i]);
        if (i + 1 < item2.str_val.length) {
          string_add_str(&joined_str.str_val, &item1.str_val);
        }
      }
      free(item1.str_val.str_data);
      free(item2.str_val.str_data);
      stack_push(joined_str);
    }
    else if (item1.type == TYPE_BLOCK) {
      if (item2.type == TYPE_BLOCK) {
        swap_items(&item1, &item2);
      }
      if (item2.str_val.length == 0) {
        fprintf(stderr, "Error! Cannot map over an empty string!\n");
        exit(1);
      }
      stack_push(make_integer(item2.str_val.str_data[0]));
      for (uint32_t i = 1; i < item2.str_val.length; i++) {
        stack_push(make_integer(item2.str_val.str_data[i]));
        execute_string(&item1.str_val);
      }
      free(item2.str_val.str_data);
      free(item1.str_val.str_data);
    }
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

void builtin_equal() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  if (item1.type < item2.type) {
    swap_items(&item1, &item2);
  }

  if (item2.type == TYPE_INTEGER) {
    if (item1.type == TYPE_INTEGER) {
      stack_push(make_integer(items_equal(&item1, &item2)));
    }
    else if (item1.type == TYPE_STRING || item1.type == TYPE_BLOCK) {
      if (item2.int_val < 0) {
        item2.int_val = item1.str_val.length + item2.int_val;
      }
      if (item2.int_val < item1.str_val.length && item2.int_val >= 0) {
        stack_push(make_integer(item1.str_val.str_data[item2.int_val]));
      }
      free_item(&item1);
    }
    else if (item1.type == TYPE_ARRAY) {
      if (item2.int_val < 0) {
        item2.int_val = item1.arr_val.length + item2.int_val;
      }
      if (item2.int_val < item1.arr_val.length && item2.int_val >= 0) {
        stack_push(make_copy(&item1.arr_val.items[item2.int_val]));
      }
      free_item(&item1);
    }
  }
  else {
    coerce_types(&item1, &item2);
    stack_push(make_integer(items_equal(&item1, &item2)));
    free_item(&item1);
    free_item(&item2);
  }
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

void builtin_less_than() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  if (item1.type < item2.type) {
    swap_items(&item1, &item2);
  }

  if (item2.type == TYPE_INTEGER) {
    if (item1.type == TYPE_INTEGER) {
      stack_push(make_integer(item2.int_val < item1.int_val));
    }
    else if (item1.type == TYPE_BLOCK || item1.type == TYPE_STRING) {
      if (item2.int_val < 0) {
        item2.int_val *= -1;
        if (item2.int_val > item1.str_val.length)
          item1.str_val.length = 0;
        else
          item1.str_val.length -= item2.int_val;
      }
      else {
        item1.str_val.length = min(item2.int_val, item1.str_val.length);
      }
      stack_push(item1);
    }
    else if (item1.type == TYPE_ARRAY) {
      int64_t to_remove;
      if (item2.int_val < 0)
        to_remove = min(-item2.int_val, item1.arr_val.length);
      else
        to_remove = item1.arr_val.length - item2.int_val;
      for (int64_t i = 1; i <= to_remove; i++) {
        free_item(&item1.arr_val.items[item1.arr_val.length - i]);
      }
      item1.arr_val.length -= max(0, to_remove);
      stack_push(item1);
    }
  }
  else {
    coerce_types(&item1, &item2);
    stack_push(make_integer(item_compare(&item1, &item2) > 0));
  }
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

void builtin_percent() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  if (item2.type > item1.type) {
    swap_items(&item1, &item2);
  }

  if (item2.type == TYPE_INTEGER) {
    if (item1.type == TYPE_INTEGER) {
      item2.int_val %= item1.int_val;
      stack_push(item2);
    }
    else if (item1.type == TYPE_ARRAY) {
      array_step_over(&item1.arr_val, item2.int_val);
      stack_push(item1);
    }
    else if (item1.type == TYPE_STRING || item1.type == TYPE_BLOCK) {
      string_step_over(&item1.str_val, item2.int_val);
      stack_push(item1);
    }
  }
  else if (item2.type == TYPE_ARRAY) {
    if (item1.type == TYPE_ARRAY) {
      array_split(&item2.arr_val, &item1.arr_val);
      Array to_remove = new_array();
      array_push(&to_remove, make_array());
      array_subtract(&item2.arr_val, &to_remove);
      stack_push(item2);
      free_item(&item1);
      free(to_remove.items);
    }
    else if (item1.type == TYPE_STRING) {
      Array str_array = new_array();
      for (uint32_t i = 0; i < item1.str_val.length; i++) {
        array_push(&str_array, make_integer(item1.str_val.str_data[i]));
      }
      free(item1.str_val.str_data);
      item1.arr_val = str_array;
      item1.type = TYPE_ARRAY;
      array_split(&item1.arr_val, &item2.arr_val);
      Array to_remove = new_array();
      array_push(&to_remove, make_array());
      array_subtract(&item1.arr_val, &to_remove);
      stack_push(item1);
      free_item(&item2);
      free(to_remove.items);
    }
    else if (item1.type == TYPE_BLOCK) {
      Item mapped_array = make_array();
      for (uint32_t i = 0; i < item2.arr_val.length; i++) {
        uint32_t start_stack_size = stack.length;
        stack_push(make_copy(&item2.arr_val.items[i]));
        execute_string(&item1.str_val);
        for (uint32_t j = start_stack_size; j < stack.length; j++) {
          array_push(&mapped_array.arr_val, stack.items[j]);
        }
        stack.length = min(stack.length, start_stack_size);
      }
      stack_push(mapped_array);
      free_item(&item1);
      free_item(&item2);
    }
  }
  else if (item2.type == TYPE_STRING) {
    if (item1.type == TYPE_STRING) {
      Item split_string = string_split(&item2.str_val, &item1.str_val);
      Array to_remove = new_array();
      array_push(&to_remove, empty_string());
      array_subtract(&split_string.arr_val, &to_remove);
      stack_push(split_string);
      free_item(&item1);
      free_item(&item2);
      free(to_remove.items);
    }
    else if (item1.type == TYPE_BLOCK) {
      Item mapped_str = empty_string();
      for (uint32_t i = 0; i < item2.arr_val.length; i++) {
        uint32_t start_stack_size = stack.length;
        stack_push(make_integer(item2.str_val.str_data[i]));
        execute_string(&item1.str_val);
        for (uint32_t j = start_stack_size; j < stack.length; j++) {
          Item new_item = stack_pop();
          if (new_item.type == TYPE_INTEGER) {
            string_add_char(&mapped_str.str_val, new_item.int_val);
          }
          else {
            items_add(&mapped_str, &new_item);
            if (mapped_str.type == TYPE_BLOCK) {
              if (i == 0) {
                string_remove_front(&mapped_str.str_val);
              }
              mapped_str.type = TYPE_STRING;
            }
          }
          free_item(&new_item);
        }
        stack.length = min(stack.length, start_stack_size);
      }
      stack_push(mapped_str);
      free_item(&item1);
      free_item(&item2);
    }
  }
  else if (item1.type == TYPE_BLOCK && item2.type == TYPE_BLOCK) {
    fprintf(stderr, "Error! %% operation undefined for two blocks!\n");
    exit(1);
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

  items_add(&item2, &item1);
  free_item(&item1);
  stack_push(item2);
}

void builtin_print() {
  Item item = stack_pop();
  output_item(&item);
  free_item(&item);
}

void builtin_question() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  if (item1.type < item2.type)
    swap_items(&item1, &item2);

  if (item1.type == TYPE_INTEGER) {
    if (item1.int_val < 0) {
      fprintf(stderr, "Error! Can't raise an integer to a negative power!\n");
      exit(1);
    }
    int64_t base = item2.int_val;
    item2.int_val = 1;
    while (item1.int_val) {
      if (item1.int_val & 1)
        item2.int_val *= base;
      item1.int_val >>= 1;
      base *= base;
    }
    stack_push(item2);
  }
  else if (item1.type == TYPE_ARRAY) {
    if (item2.type == TYPE_ARRAY) {
      swap_items(&item1, &item2);
    }
    stack_push(make_integer(array_find(&item1.arr_val, &item2)));
    free_item(&item2);
    free_item(&item1);
  }
  else if (item1.type == TYPE_STRING) {
    if (item2.type == TYPE_INTEGER) {
      stack_push(make_integer(string_find_char(&item1.str_val,
                                               item2.int_val)));
      free_item(&item2);
    }
    else if (item2.type == TYPE_ARRAY) {
      stack_push(make_integer(array_find(&item2.arr_val, &item1)));
      free_item(&item1);
      free_item(&item2);
    }
    else if (item2.type == TYPE_STRING) {
      stack_push(make_integer(string_find_str(&item2.str_val,
                                              &item1.str_val)));
      free_item(&item1);
      free_item(&item2);
    }
  }
  else if (item1.type == TYPE_BLOCK) {
    if (item2.type == TYPE_INTEGER) {
      fprintf(stderr, "Error! Can't perform find operation on an integer.\n");
      exit(1);
    }
    else if (item2.type == TYPE_BLOCK || item2.type == TYPE_STRING) {
      if (item2.type == TYPE_BLOCK) {
        swap_items(&item1, &item2);
      }
      for (uint32_t i = 0; i < item2.str_val.length; i++) {
        stack_push(make_integer(item2.str_val.str_data[i]));
        execute_string(&item1.str_val);
        Item item_bool = stack_pop();
        if (item_boolean(&item_bool)) {
          free_item(&item_bool);
          stack_push(make_integer(item2.str_val.str_data[i]));
          break;
        }
        free_item(&item_bool);
      }
      free_item(&item1);
      free_item(&item2);
    }
    else if (item2.type == TYPE_ARRAY) {
      for (uint32_t i = 0; i < item2.arr_val.length; i++) {
        stack_push(make_copy(&item2.arr_val.items[i]));
        execute_string(&item1.str_val);
        Item item_bool = stack_pop();
        if (item_boolean(&item_bool)) {
          free_item(&item_bool);
          stack_push(make_copy(&item2.arr_val.items[i]));
          break;
        }
        free_item(&item_bool);
      }
      free_item(&item1);
      free_item(&item2);
    }
  }
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

void builtin_slash() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  if (item1.type < item2.type)
    swap_items(&item1, &item2);

  if (item1.type == TYPE_INTEGER) {
    if (item2.type == TYPE_INTEGER) {
      item2.int_val /= item1.int_val;
      stack_push(item2);
    }
  }
  else if (item1.type == TYPE_ARRAY) {
    if (item2.type == TYPE_INTEGER) {
      Item split_array = array_split_into_groups(&item1.arr_val, item2.int_val);
      stack_push(split_array);
      free(item1.arr_val.items);
    }
    else if (item1.type == TYPE_ARRAY) {
      array_split(&item2.arr_val, &item1.arr_val);
      free_item(&item1);
      stack_push(item2);
    }
  }
  else if (item1.type == TYPE_STRING) {
    if (item2.type == TYPE_INTEGER) {
      Item split_str = string_split_into_groups(&item1.str_val, item2.int_val);
      stack_push(split_str);
      free_item(&item2);
    }
    else if (item2.type == TYPE_ARRAY) {
      Item array = make_array();
      for (uint32_t i = 0; i < item1.str_val.length; i++) {
        array_push(&array.arr_val, make_integer(item1.str_val.str_data[i]));
      }
      array_split(&array.arr_val, &item2.arr_val);
      stack_push(array);
      free_item(&item1);
      free_item(&item2);
    }
    else if (item2.type == TYPE_STRING) {
      Item split_str = string_split(&item2.str_val, &item1.str_val);
      stack_push(split_str);
      free_item(&item1);
      free_item(&item2);
    }
  }
  else if (item1.type == TYPE_BLOCK) {
    if (item2.type == TYPE_ARRAY) {
      for (uint32_t i = 0; i < item2.arr_val.length; i++) {
        stack_push(item2.arr_val.items[i]);
        execute_string(&item1.str_val);
      }
      free(item2.arr_val.items);
      free(item1.str_val.str_data);
    }
    else if (item2.type == TYPE_STRING) {
      for (uint32_t i = 0; i < item2.str_val.length; i++) {
        stack_push(make_integer(item2.str_val.str_data[i]));
        execute_string(&item1.str_val);
      }
      free(item2.str_val.str_data);
      free(item1.str_val.str_data);
    }
    else if (item2.type == TYPE_BLOCK) {
      Item final_array = make_array();
      Item top = stack_pop();
      stack_push(make_copy(&top));
      execute_item(&item2);
      Item cond_item = stack_pop();
      while (item_boolean(&cond_item)) {
        free_item(&cond_item);
        stack_push(make_copy(&top));
        array_push(&final_array.arr_val, top);
        execute_item(&item1);
        top = stack_pop();
        stack_push(make_copy(&top));
        execute_item(&item2);
        cond_item = stack_pop();
      }
      stack_push(final_array);
      free_item(&cond_item);
      free_item(&item1);
      free_item(&item2);
    }
    else if (item2.type == TYPE_INTEGER) {
      fprintf(stderr, "Error! Builtin / function undefined for block and int.\n");
      exit(1);
    }
  }
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
