// builtins.c
// Contains functions implementing golfscript's builtin functions

#include <stdlib.h>
#include "golf.h"

void builtin_abs() {
  Item item = stack_pop();
  if (item.type == TYPE_INTEGER) {
    item.int_val.is_negative = false;
    stack_push(item);
  }
  else {
    error("Invalid type for abs function!");
  }
}

void builtin_ampersand() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  coerce_types(&item1, &item2);
  if (item1.type == TYPE_INTEGER) {
    Bigint and_val = bigint_and(&item1.int_val, &item2.int_val);
    free_bigint(&item2.int_val);
    item2.int_val = and_val;
  }
  else if (item1.type == TYPE_BLOCK || item1.type == TYPE_STRING) {
    string_setwise_and(&item2.str_val, &item1.str_val);
  }
  else if (item1.type == TYPE_ARRAY) {
    array_and(&item2.arr_val, &item1.arr_val);
  }

  free_item(&item1);
  stack_push(item2);
}

void builtin_asterisk() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  if (item1.type < item2.type)
    swap_items(&item1, &item2);

  if (item2.type == TYPE_INTEGER) {
    if (item1.type == TYPE_INTEGER) {
      Bigint product = bigint_multiply(&item1.int_val, &item2.int_val);
      free_bigint(&item1.int_val);
      item1.int_val = product;
    }
    else if (item1.type == TYPE_ARRAY)
      array_multiply(&item1.arr_val, item2.int_val);
    else if (item1.type == TYPE_STRING)
      string_multiply(&item1.str_val, item2.int_val);
    else if (item1.type == TYPE_BLOCK) {
      repeat_block(&item1, item2.int_val);
      free_item(&item1);
      free_item(&item2);
      return;
    }
    free_item(&item2);
    stack_push(item1);
    return;
  }
  else if (item2.type == TYPE_ARRAY) {
    if (item1.type == TYPE_ARRAY || item1.type == TYPE_STRING)
      stack_push(join_array(&item2.arr_val, &item1));
    else if (item1.type == TYPE_BLOCK)
      fold_array(&item2.arr_val, &item1);
  }
  else if (item2.type == TYPE_STRING) {
    if (item1.type == TYPE_STRING)
      stack_push(string_join(&item2.str_val, &item1.str_val));
    else if (item1.type == TYPE_BLOCK)
      fold_string(&item2.str_val, &item1);
  }
  else if (item2.type == TYPE_BLOCK) {
    if (item1.type == TYPE_BLOCK)
      fold_string(&item1.str_val, &item2);
  }

  free_item(&item1);
  free_item(&item2);
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
    Bigint or_val = bigint_or(&item1.int_val, &item2.int_val);
    free_bigint(&item2.int_val);
    item2.int_val = or_val;
  }
  else if (item1.type == TYPE_STRING || item1.type == TYPE_BLOCK) {
    string_setwise_or(&item2.str_val, &item1.str_val);
  }
  else if (item1.type == TYPE_ARRAY) {
    array_or(&item2.arr_val, &item1.arr_val);
  }

  free_item(&item1);
  stack_push(item2);
}

void builtin_base() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  if (item1.type == TYPE_INTEGER) {
    if (bigint_is_zero(&item1.int_val)) {
      error("Cannot use a base of 0!");
    }
    if (item2.type == TYPE_INTEGER) {
      Item digits = make_array();
      if (bigint_is_zero(&item2.int_val)) {
        array_push(&digits.arr_val, make_integer(0));
      }
      item2.int_val.is_negative = false;
      while (!bigint_is_zero(&item2.int_val)) {
        Bigint quotient, remainder;
        bigint_divmod(&item2.int_val, &item1.int_val, &quotient, &remainder);
        array_push(&digits.arr_val, make_integer_from_bigint(&remainder));
        free_bigint(&item2.int_val);
        free_bigint(&remainder);
        item2.int_val = quotient;
      }
      array_reverse(&digits.arr_val);
      stack_push(digits);
    }
    else if (item2.type == TYPE_ARRAY) {
      Bigint base_val = bigint_from_int64(1);
      Bigint result = new_bigint();
      for (int32_t i = item2.arr_val.length - 1; i >= 0; i--) {
        if (item2.arr_val.items[i].type != TYPE_INTEGER) {
          error("Cannot perform base operation on array with non-integers!");
        }
        Bigint product = bigint_multiply(&base_val,
                                         &item2.arr_val.items[i].int_val);
        bigint_add(&result, &product);
        Bigint temp_bigint = base_val;
        base_val = bigint_multiply(&item1.int_val, &base_val);
        free_bigint(&temp_bigint);
        free_bigint(&product);
      }
      stack_push(make_integer_from_bigint(&result));
      free_bigint(&result);
      free_bigint(&base_val);
    }
    else if (item2.type == TYPE_STRING || item2.type == TYPE_BLOCK) {
      Bigint base_val = bigint_from_int64(1);
      Bigint result = new_bigint();
      for (int32_t i = item2.str_val.length - 1; i >= 0; i--) {
        Bigint temp_char = bigint_from_int64(item2.str_val.str_data[i]);
        Bigint product = bigint_multiply(&temp_char, &base_val);
        bigint_add(&result, &product);
        free_bigint(&temp_char);
        free_bigint(&product);
        Bigint temp_base = base_val;
        base_val = bigint_multiply(&base_val, &temp_base);
        free_bigint(&temp_base);
      }
      stack_push(make_integer_from_bigint(&result));
      free_bigint(&result);
      free_bigint(&base_val);
    }
    free_item(&item2);
    free_item(&item1);
  }
  else {
    error("Invalid arguments for base operation!");
  }
}

void builtin_caret() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  coerce_types(&item1, &item2);
  if (item1.type == TYPE_INTEGER) {
    Bigint xor_val = bigint_xor(&item1.int_val, &item2.int_val);
    free_bigint(&item2.int_val);
    item2.int_val = xor_val;
  }
  else if (item1.type == TYPE_STRING || item1.type == TYPE_BLOCK) {
    string_setwise_xor(&item2.str_val, &item1.str_val);
  }
  else if (item1.type == TYPE_ARRAY) {
    array_xor(&item2.arr_val, &item1.arr_val);
  }

  free_item(&item1);
  stack_push(item2);
}

void builtin_comma() {
  Item item = stack_pop();
  if (item.type == TYPE_INTEGER) {
    Item array = make_array();
    Bigint cur_val;
    for (cur_val = new_bigint();
         bigint_compare(&cur_val, &item.int_val) < 0;
         bigint_increment(&cur_val))
    {
      array_push(&array.arr_val, make_integer_from_bigint(&cur_val));
    }
    free_bigint(&cur_val);
    stack_push(array);
  }
  else if (item.type == TYPE_STRING) {
    stack_push(make_integer(item.str_val.length));
  }
  else if (item.type == TYPE_ARRAY) {
    stack_push(make_integer(item.arr_val.length));
  }
  else if (item.type == TYPE_BLOCK) {
    Item to_filter = stack_pop();
    if (to_filter.type == TYPE_ARRAY) {
      filter_array(&to_filter.arr_val, &item);
    }
    else if (to_filter.type == TYPE_STRING || to_filter.type == TYPE_BLOCK) {
      filter_string(&to_filter.str_val, &item);
    }
    else if (to_filter.type == TYPE_INTEGER) {
      error("Cannot filter over an integer!");
    }
    stack_push(to_filter);
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

void builtin_dollar_sign() {
  Item item = stack_pop();

  if (item.type == TYPE_INTEGER) {
    if (item.int_val.is_negative) {
      item.int_val.is_negative = false;
      bigint_decrement(&item.int_val);
      if (bigint_fits_in_uint32(&item.int_val) &&
          bigint_to_uint32(&item.int_val) < stack.length)
      {
        stack_push(make_copy(&stack.items[bigint_to_uint32(&item.int_val)]));
      }
    }
    else if (bigint_fits_in_uint32(&item.int_val) &&
             bigint_to_uint32(&item.int_val) < stack.length)
    {
      uint32_t index = stack.length - bigint_to_uint32(&item.int_val) - 1;
      stack_push(make_copy(&stack.items[index]));
    }
    free_item(&item);
  }
  else if (item.type == TYPE_STRING) {
    string_sort(&item.str_val);
    stack_push(item);
  }
  else if (item.type == TYPE_ARRAY) {
    array_sort(&item.arr_val);
    stack_push(item);
  }
  else if (item.type == TYPE_BLOCK) {
    Item to_sort = stack_pop();
    if (to_sort.type == TYPE_INTEGER) {
      error("Cannot sort an integer!");
    }
    else if (to_sort.type == TYPE_BLOCK || to_sort.type == TYPE_STRING) {
      Item mapped_array = make_array();
      for (uint32_t i = 0; i < to_sort.str_val.length; i++) {
        stack_push(make_integer(to_sort.str_val.str_data[i]));
        execute_string(&item.str_val);
        array_push(&mapped_array.arr_val, stack_pop());
      }
      string_sort_by_mapping(&to_sort.str_val, &mapped_array.arr_val);
      stack_push(to_sort);
      free_item(&mapped_array);
    }
    else if (to_sort.type == TYPE_ARRAY) {
      Item mapped_array = make_array();
      for (uint32_t i = 0; i < to_sort.arr_val.length; i++) {
        stack_push(make_copy(&to_sort.arr_val.items[i]));
        execute_string(&item.str_val);
        array_push(&mapped_array.arr_val, stack_pop());
      }
      array_sort_by_mapping(&to_sort.arr_val, &mapped_array.arr_val);
      stack_push(to_sort);
      free_item(&mapped_array);
    }
    free_item(&item);
  }
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
      free_item(&item1);
      free_item(&item2);
    }
    else if (item1.type == TYPE_STRING || item1.type == TYPE_BLOCK) {
      if (bigint_fits_in_uint32(&item2.int_val)) {
        bool was_negative = item2.int_val.is_negative;
        item2.int_val.is_negative = false;
        int64_t index = bigint_to_uint32(&item2.int_val);
        if (was_negative) {
          index = item1.str_val.length - index;
        }
        if (index < item1.str_val.length && index >= 0) {
          stack_push(make_integer(item1.str_val.str_data[index]));
        }
      }
      free_item(&item1);
      free_item(&item2);
    }
    else if (item1.type == TYPE_ARRAY) {
      if (bigint_fits_in_uint32(&item2.int_val)) {
        bool was_negative = item2.int_val.is_negative;
        item2.int_val.is_negative = false;
        int64_t index = bigint_to_uint32(&item2.int_val);
        if (was_negative) {
          index = item1.arr_val.length - index;
        }
        if (index < item1.arr_val.length && index >= 0) {
          stack_push(make_copy(&item1.arr_val.items[index]));
        }
      }
      free_item(&item1);
      free_item(&item2);
    }
  }
  else {
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

void builtin_greater_than() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  if (item1.type < item2.type) {
    swap_items(&item1, &item2);
  }

  if (item2.type == TYPE_INTEGER) {
    if (item1.type == TYPE_INTEGER) {
      stack_push(make_integer(item_compare(&item1, &item2) < 0));
      free_item(&item1);
      free_item(&item2);
    }
    else if (item1.type == TYPE_STRING || item1.type == TYPE_BLOCK) {
      if (item2.int_val.is_negative) {
        Bigint str_len = bigint_from_int64(item1.str_val.length);
        bigint_add(&item2.int_val, &str_len);
        free_bigint(&str_len);
      }
      string_remove_from_front(&item1.str_val, item2.int_val);
      free_item(&item2);
      stack_push(item1);
    }
    else if (item1.type == TYPE_ARRAY) {
      if (item2.int_val.is_negative) {
        Bigint arr_len = bigint_from_int64(item1.arr_val.length);
        bigint_add(&item2.int_val, &arr_len);
        free_bigint(&arr_len);
      }
      array_remove_from_front(&item1.arr_val, item2.int_val);
      free_item(&item2);
      stack_push(item1);
    }
  }
  else {
    stack_push(make_integer(item_compare(&item1, &item2) < 0));
    free_item(&item1);
    free_item(&item2);
  }
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


void builtin_less_than() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  if (item1.type < item2.type) {
    swap_items(&item1, &item2);
  }

  if (item2.type == TYPE_INTEGER) {
    if (item1.type == TYPE_INTEGER) {
      stack_push(make_integer(item_compare(&item1, &item2) > 0));
      free_item(&item1);
      free_item(&item2);
    }
    else if (item1.type == TYPE_BLOCK || item1.type == TYPE_STRING) {
      if (item2.int_val.is_negative) {
        item2.int_val.is_negative = false;
        if (!bigint_fits_in_uint32(&item2.int_val))
          item1.str_val.length = 0;
        else {
          uint32_t to_subtract = bigint_to_uint32(&item2.int_val);
          if (to_subtract > item1.str_val.length)
            item1.str_val.length = 0;
          else
            item1.str_val.length -= to_subtract;
        }
      }
      else {
        if (bigint_fits_in_uint32(&item2.int_val)) {
          uint32_t new_len = bigint_to_uint32(&item2.int_val);
          item1.str_val.length = min(new_len, item1.str_val.length);
        }
      }
      stack_push(item1);
    }
    else if (item1.type == TYPE_ARRAY) {
      uint32_t to_remove = 0;
      if (item2.int_val.is_negative) {
        item2.int_val.is_negative = false;
        if (bigint_fits_in_uint32(&item2.int_val)) {
          to_remove = bigint_to_uint32(&item2.int_val);
          to_remove = min(to_remove, item1.arr_val.length);
        }
        else {
          to_remove = item1.arr_val.length;
        }
      }
      else {
        if (bigint_fits_in_uint32(&item2.int_val)) {
          uint32_t index = bigint_to_uint32(&item2.int_val);
          if (index < item1.arr_val.length)
            to_remove = item1.arr_val.length - index;
        }
      }
      for (int64_t i = 1; i <= to_remove; i++) {
        free_item(&item1.arr_val.items[item1.arr_val.length - i]);
      }
      item1.arr_val.length -= to_remove;
      stack_push(item1);
    }
  }
  else {
    stack_push(make_integer(item_compare(&item1, &item2) > 0));
    free_item(&item1);
    free_item(&item2);
  }
}

void builtin_minus() {
  Item item1 = stack_pop();
  Item item2 = stack_pop();

  coerce_types(&item1, &item2);

  if (item2.type == TYPE_INTEGER) {
    bigint_subtract(&item2.int_val, &item1.int_val);
  }
  else if (item2.type == TYPE_STRING || item2.type == TYPE_BLOCK) {
    string_subtract(&item2.str_val, &item1.str_val);
    free(item1.str_val.str_data);
  }
  else if (item2.type == TYPE_ARRAY) {
    array_subtract(&item2.arr_val, &item1.arr_val);
    free_item(&item1);
  }
  free_item(&item1);
  stack_push(item2);
}

void builtin_lparen() {
  Item item = stack_pop();

  if (item.type == TYPE_INTEGER) {
    bigint_decrement(&item.int_val);
    stack_push(item);
  }
  else if (item.type == TYPE_STRING || item.type == TYPE_BLOCK) {
    if (item.str_val.length == 0) {
      error("Unable to uncons from empty %s!",
            item.type == TYPE_STRING ? "string": "block");
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
      error("Unable to uncons from empty array");
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
      Bigint remainder;
      bigint_divmod(&item2.int_val, &item1.int_val, NULL, &remainder);
      free_bigint(&item1.int_val);
      item1.int_val = remainder;
      free_item(&item2);
    }
    else if (item1.type == TYPE_ARRAY)
      array_step_over(&item1.arr_val, item2.int_val);
    else if (item1.type == TYPE_STRING || item1.type == TYPE_BLOCK)
      string_step_over(&item1.str_val, item2.int_val);
    stack_push(item1);
  }
  else if (item2.type == TYPE_ARRAY) {
    if (item1.type == TYPE_ARRAY) {
      array_split(&item2.arr_val, &item1.arr_val);
      array_remove_empty_arrays(&item2.arr_val);
    }
    else if (item1.type == TYPE_STRING) {
      Array str_array = array_from_string(&item1.str_val);
      array_split(&str_array, &item2.arr_val);
      array_remove_empty_arrays(&str_array);
      free_item(&item2);
      item2.arr_val = str_array;
    }
    else if (item1.type == TYPE_BLOCK) {
      map_array(&item2.arr_val, &item1);
    }
    stack_push(item2);
    free_item(&item1);
  }
  else if (item2.type == TYPE_STRING) {
    if (item1.type == TYPE_STRING) {
      Item split_string = string_split(&item2.str_val, &item1.str_val);
      array_remove_empty_strings(&split_string.arr_val);
      free_item(&item2);
      item2 = split_string;
    }
    else if (item1.type == TYPE_BLOCK) {
      map_string(&item2.str_val, &item1);
    }
    stack_push(item2);
    free_item(&item1);
  }
  else if (item1.type == TYPE_BLOCK && item2.type == TYPE_BLOCK) {
    error("%% operation undefined for two blocks!");
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
    if (item1.int_val.is_negative) {
      error("Can't raise an integer to a negative power!");
    }
    Bigint power = bigint_exponent(&item2.int_val, &item1.int_val);
    free_bigint(&item2.int_val);
    item2.int_val = power;
    stack_push(item2);
    free_item(&item1);
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
                                               item2.int_val.digits[0] & 255)));
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
      error("Can't perform find operation on an integer.");
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
    error("rand requires an integer!");
  }
  else {
    item.int_val = get_randint(item.int_val);
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
    Bigint first_bracket = bracket_stack.items[bracket_stack.length].int_val;
    first_item = bigint_to_uint32(&first_bracket);
    free_bigint(&first_bracket);
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
    bigint_increment(&item.int_val);
    stack_push(item);
  }
  else if (item.type == TYPE_STRING || item.type == TYPE_BLOCK) {
    if (item.str_val.length == 0) {
      error("Unable to uncons from empty %s!",
            item.type == TYPE_STRING ? "string": "block");
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
      error("Unable to uncons from empty array");
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
      if (bigint_is_zero(&item1.int_val)) {
        error("Attempted to divide by zero!");
      }
      Bigint quotient;
      bigint_divmod(&item2.int_val, &item1.int_val, &quotient, NULL);
      free_bigint(&item2.int_val);
      item2.int_val = quotient;
      stack_push(item2);
      free_item(&item1);
    }
  }
  else if (item1.type == TYPE_ARRAY) {
    if (item2.type == TYPE_INTEGER) {
      Item split_array = array_split_into_groups(&item1.arr_val, item2.int_val);
      stack_push(split_array);
      free_item(&item2);
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
      free_item(&item1);
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
      error("Builtin / function undefined for block and int.");
    }
  }
}

void builtin_tilde() {
  Item item = stack_pop();
  if (item.type == TYPE_INTEGER) {
    item.int_val.is_negative = !item.int_val.is_negative;
    bigint_decrement(&item.int_val);
    stack_push(item);
  }
  else if (item.type == TYPE_STRING || item.type == TYPE_BLOCK) {
    execute_string(&item.str_val);
    free_item(&item);
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

void builtin_zip() {
  Item item = stack_pop();
  if (item.type != TYPE_ARRAY) {
    error("Cannot zip over non-array!");
  }
  Item zipped_array = make_array();
  for (uint32_t i = 0; i < item.arr_val.length; i++) {
    Item cur_item = item.arr_val.items[i];
    if (cur_item.type == TYPE_INTEGER) {
      error("Cannot zip over array with an integer!");
    }
    else if (cur_item.type == TYPE_ARRAY) {
      for (uint32_t j = 0; j < cur_item.arr_val.length; j++) {
        if (j >= zipped_array.arr_val.length) {
          if (item.arr_val.items[0].type == TYPE_ARRAY)
            array_push(&zipped_array.arr_val, make_array());
          else if (item.arr_val.items[0].type == TYPE_STRING)
            array_push(&zipped_array.arr_val, empty_string());
          else if (item.arr_val.items[0].type == TYPE_BLOCK)
            array_push(&zipped_array.arr_val, make_block(new_string()));
        }
        if (zipped_array.arr_val.items[j].type == TYPE_STRING ||
            zipped_array.arr_val.items[j].type == TYPE_BLOCK)
        {
          if (cur_item.arr_val.items[j].type != TYPE_INTEGER) {
            error("Invalid array for zip!");
          }
          string_add_char(&zipped_array.arr_val.items[j].str_val,
                          cur_item.arr_val.items[j].int_val.digits[0] & 255);
        }
        else if (zipped_array.arr_val.items[j].type == TYPE_ARRAY) {
          array_push(&zipped_array.arr_val.items[j].arr_val,
                     cur_item.arr_val.items[j]);
        }
      }
    }
    else if (cur_item.type == TYPE_STRING || cur_item.type == TYPE_BLOCK) {
      for (uint32_t j = 0; j < cur_item.str_val.length; j++) {
        if (j >= zipped_array.arr_val.length) {
          if (item.arr_val.items[0].type == TYPE_ARRAY)
            array_push(&zipped_array.arr_val, make_array());
          else if (item.arr_val.items[0].type == TYPE_STRING)
            array_push(&zipped_array.arr_val, empty_string());
          else if (item.arr_val.items[0].type == TYPE_BLOCK)
            array_push(&zipped_array.arr_val, make_block(new_string()));
        }
        if (zipped_array.arr_val.items[j].type == TYPE_STRING ||
            zipped_array.arr_val.items[j].type == TYPE_BLOCK)
        {
          string_add_char(&zipped_array.arr_val.items[j].str_val,
                          cur_item.str_val.str_data[j]);
        }
        else if (zipped_array.arr_val.items[j].type == TYPE_ARRAY) {
          array_push(&zipped_array.arr_val.items[j].arr_val,
                     make_integer(cur_item.str_val.str_data[j]));
        }
      }
    }
  }
  stack_push(zipped_array);
  free(item.arr_val.items);
}
