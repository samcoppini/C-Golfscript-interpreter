#include <assert.h>
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
    free(str.str_data);
    str = int_to_string(item->int_val);
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

bool item_boolean(Item *item) {
  switch (item->type) {
    case TYPE_INTEGER:
      return item->int_val != 0;

    case TYPE_STRING:
    case TYPE_BLOCK:
      return item->str_val.length != 0;

    case TYPE_ARRAY:
      return item->arr_val.length != 0;

    default:
      assert(false);
  }
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

String array_to_string(Item *array) {
  String str = new_string();
  for (uint32_t i = 0; i < array->arr_val.length; i++) {
    Item *cur_item = &array->arr_val.items[i];
    if (cur_item->type == TYPE_INTEGER)
      string_add_char(&str, cur_item->int_val);
    else if (cur_item->type == TYPE_STRING)
      string_add_str(&str, cur_item->str_val.str_data);
    else if (cur_item->type == TYPE_BLOCK)
      string_add_str(&str, cur_item->str_val.str_data);
    else if (cur_item->type == TYPE_ARRAY) {
      String array_str = array_to_string(cur_item);
      string_add_str(&str, array_str.str_data);
      free(array_str.str_data);
    }
  }
  return str;
}

void coerce_types(Item *item1, Item *item2) {
  if (item1->type == item2->type) {
    return;
  }
  else if (item1->type < item2->type) {
    Item *tmp = item1;
    item1 = item2;
    item2 = tmp;
  }
  if (item1->type == TYPE_BLOCK) {
    if (item2->type == TYPE_STRING) {
      item2->type = TYPE_BLOCK;
    }
    else if (item2->type == TYPE_ARRAY) {
      String block_str = new_string();
      for (uint32_t i = 0; i < item2->arr_val.length; i++) {
        Item *cur_item = &item2->arr_val.items[i];
        if (cur_item->type == TYPE_STRING ||cur_item->type == TYPE_BLOCK) {
          string_add_str(&block_str, cur_item->str_val.str_data);
        }
        else if (cur_item->type == TYPE_ARRAY) {
          String arr_str = array_to_string(cur_item);
          string_add_str(&block_str, arr_str.str_data);
          free(arr_str.str_data);
        }
        else if (cur_item->type == TYPE_INTEGER) {
          String int_str = int_to_string(cur_item->int_val);
          string_add_str(&block_str, int_str.str_data);
          free(int_str.str_data);
        }
        if (i + 1 < item2->arr_val.length)
          string_add_char(&block_str, ' ');
      }
      free_item(item2);
      item2->type = TYPE_BLOCK;
      item2->str_val = block_str;
    }
    else if (item2->type == TYPE_INTEGER) {
      item2->type = TYPE_BLOCK;
      item2->str_val = int_to_string(item2->int_val);
    }
  }
  else if (item1->type == TYPE_STRING) {
    if (item2->type == TYPE_ARRAY) {
      String str = array_to_string(item2);
      free_item(item2);
      item2->type = TYPE_STRING;
      item2->str_val = str;
    }
    else if (item2->type == TYPE_INTEGER) {
      item2->type = TYPE_STRING;
      item2->str_val = int_to_string(item2->int_val);
    }
  }
  else if (item1->type == TYPE_ARRAY) {
    if (item2->type == TYPE_INTEGER) {
      int64_t int_val = item2->int_val;
      item2->type = TYPE_ARRAY;
      item2->arr_val = new_array();
      array_push(&item2->arr_val, make_integer(int_val));
    }
  }
}
