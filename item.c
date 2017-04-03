#include <stdio.h>
#include <stdlib.h>
#include "golf.h"

Item make_integer(int64_t int_val) {
  Item item = {TYPE_INTEGER, .int_val = int_val};
  return item;
}

Item make_string(char *str_val) {
  Item item = {TYPE_STRING, .str_val = create_string(str_val)};
  return item;
}

Item make_block(char *str_val) {
  Item item = {TYPE_BLOCK, .str_val = create_string(str_val)};
  return item;
}

Item make_array() {
  Item item = {TYPE_ARRAY, .arr_val = new_array()};
  return item;
}

Item make_builtin(void (*function)()) {
  Item item = {TYPE_FUNCTION, .function = function};
  return item;
}

Item make_copy(Item *item) {
  Item new_item;

  new_item.type = item->type;

  if (item->type == TYPE_INTEGER)
    new_item.int_val = item->int_val;
  else if (item->type == TYPE_STRING || item->type == TYPE_BLOCK)
    new_item.str_val = create_string(item->str_val.str_data);
  else if (item->type == TYPE_ARRAY) {
    new_item.arr_val = new_array();
    for (uint32_t i = 0; i < item->arr_val.length; i++) {
      array_push(&new_item.arr_val, make_copy(&item->arr_val.items[i]));
    }
  }

  return new_item;
}

String get_literal(Item *item) {
  String str = new_string();
  if (item->type == TYPE_INTEGER) {
    int64_t int_val = item->int_val;
    if (int_val < 0)
      int_val *= -1;
    else if (int_val == 0)
      string_add_char(&str, '0');
    while (int_val > 0) {
      string_add_char(&str, '0' + (int_val % 10));
      int_val /= 10;
    }
    if (item->int_val < 0)
      string_add_char(&str, '-');
    string_reverse(&str);
  }
  else if (item->type == TYPE_STRING) {
    string_add_char(&str, '"');
    for (uint32_t i = 0; i < item->str_val.length; i++) {
      if (item->str_val.str_data[i] == '"')
        string_add_str(&str, "\\\"");
      else if (item->str_val.str_data[i] == '\\')
        string_add_str(&str, "\\\\");
      else
        string_add_char(&str, item->str_val.str_data[i]);
    }
    string_add_char(&str, '"');
  }
  else if (item->type == TYPE_BLOCK) {
    string_add_char(&str, '{');
    string_add_str(&str, item->str_val.str_data);
    string_add_char(&str, '}');
  }
  else if (item->type == TYPE_ARRAY) {
    string_add_char(&str, '[');
    for (uint32_t i = 0; i < item->arr_val.length; i++) {
      String item_string = get_literal(&item->arr_val.items[i]);
      string_add_str(&str, item_string.str_data);
      free(item_string.str_data);
      if (i + 1 < item->arr_val.length) {
        string_add_char(&str, ' ');
      }
    }
    string_add_char(&str, ']');
  }
  return str;
}

void free_item(Item *item) {
  if (item->type == TYPE_STRING || item->type == TYPE_BLOCK) {
    free(item->str_val.str_data);
  }
  else if (item->type == TYPE_ARRAY) {
    for (uint32_t i = 0; i < item->arr_val.length; i++) {
      free_item(&item->arr_val.items[i]);
    }
    free(item->arr_val.items);
  }
}

void output_item(Item *item) {
  if (item->type == TYPE_INTEGER) {
    printf("%lld", item->int_val);
  }
  else if (item->type == TYPE_STRING) {
    printf("%s", item->str_val);
  }
  else if (item->type == TYPE_BLOCK) {
    printf("{%s}", item->str_val);
  }
  else if (item->type == TYPE_ARRAY) {
    for (uint32_t i = 0; i < item->arr_val.length; i++) {
      output_item(&item->arr_val.items[i]);
    }
  }
}
