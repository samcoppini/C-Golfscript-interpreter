// item.c
// Contains functions for manipulating items

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "golf.h"

Item make_integer(int64_t int_val) {
  Item item = {TYPE_INTEGER, .int_val = bigint_from_int64(int_val)};
  return item;
}

Item make_integer_from_bigint(const Bigint *bigint) {
  Item item = {TYPE_INTEGER, .int_val = copy_bigint(bigint)};
  return item;
}

Item make_string(const String *str_val) {
  Item item = {TYPE_STRING, .str_val = copy_string(str_val)};
  return item;
}

Item empty_string() {
  Item item = {TYPE_STRING, .str_val = new_string()};
  return item;
}

Item make_block(String str_val) {
  Item item = {TYPE_BLOCK, .str_val = str_val};
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

// Returns a copy of an item, duplicating its dynamically allocated contents
Item make_copy(const Item *item) {
  Item new_item;

  new_item.type = item->type;

  if (item->type == TYPE_INTEGER)
    new_item.int_val = copy_bigint(&item->int_val);
  else if (item->type == TYPE_STRING || item->type == TYPE_BLOCK)
    new_item.str_val = copy_string(&item->str_val);
  else if (item->type == TYPE_ARRAY) {
    new_item.arr_val = new_array();
    for (uint32_t i = 0; i < item->arr_val.length; i++) {
      array_push(&new_item.arr_val, make_copy(&item->arr_val.items[i]));
    }
  }

  return new_item;
}

// Returns a string representation of an item, which returns the original
// item when evaluated
String get_literal(const Item *item) {
  String str = new_string();
  if (item->type == TYPE_INTEGER) {
    free(str.str_data);
    str = bigint_to_string(&item->int_val);
  }
  else if (item->type == TYPE_STRING) {
    string_add_char(&str, '"');
    for (uint32_t i = 0; i < item->str_val.length; i++) {
      unsigned char c = item->str_val.str_data[i];
      switch (c) {
        case '"':    string_add_c_str(&str, "\\\""); break;
        case '\\':   string_add_c_str(&str, "\\\\"); break;
        case '\a':   string_add_c_str(&str, "\\a");  break;
        case '\b':   string_add_c_str(&str, "\\b");  break;
        case '\t':   string_add_c_str(&str, "\\t");  break;
        case '\n':   string_add_c_str(&str, "\\n");  break;
        case '\v':   string_add_c_str(&str, "\\v");  break;
        case '\f':   string_add_c_str(&str, "\\f");  break;
        case '\r':   string_add_c_str(&str, "\\r");  break;
        case '\x1b': string_add_c_str(&str, "\\e");  break;
        default:
          if (c < 32 || c > 127) {
            // If it's non-printing, output its hex representation
            const char hex_digits[] = "0123456789ABCDEF";
            string_add_c_str(&str, "\\x");
            string_add_char(&str, hex_digits[(c & 0xF0) >> 4]);
            string_add_char(&str, hex_digits[(c & 0x0F)]);
          }
          else {
            string_add_char(&str, c);
          }
          break;
      }
    }
    string_add_char(&str, '"');
  }
  else if (item->type == TYPE_BLOCK) {
    string_add_char(&str, '{');
    string_add_str(&str, &item->str_val);
    string_add_char(&str, '}');
  }
  else if (item->type == TYPE_ARRAY) {
    string_add_char(&str, '[');
    for (uint32_t i = 0; i < item->arr_val.length; i++) {
      String item_string = get_literal(&item->arr_val.items[i]);
      string_add_str(&str, &item_string);
      free(item_string.str_data);
      if (i + 1 < item->arr_val.length) {
        string_add_char(&str, ' ');
      }
    }
    string_add_char(&str, ']');
  }
  return str;
}

// Returns whether the item evaluates as "true"
// Only zero, empty blocks, empty arrays and empty strings evaluate as false
bool item_boolean(const Item *item) {
  switch (item->type) {
    case TYPE_INTEGER:
      return !bigint_is_zero(&item->int_val);

    case TYPE_STRING:
    case TYPE_BLOCK:
      return item->str_val.length != 0;

    case TYPE_ARRAY:
      return item->arr_val.length != 0;

    default:
      assert(false);
  }
}

// Returns whether two types are "compatible", meaning that they can
// be considered as the same type in certain contexts. For instance,
// an array consisting solely of integers is considered equivalent to
// a string or a block, but not equivalent with an integer.
bool types_compatible(enum Type type1, enum Type type2) {
  return (type1 == TYPE_INTEGER) == (type2 == TYPE_INTEGER);
}

// Compares two items, returing a positive value if item1 is "greater"
// than item2, a negative value if item2 is greater, and 0 if the two
// items are the same
int item_compare(const Item *item1, const Item *item2) {
  if (!types_compatible(item1->type, item2->type)) {
    return item1->type - item2->type;
  }
  if (item1->type > item2->type) {
    return item_compare(item2, item1) * -1;
  }
  switch (item1->type) {
    case TYPE_INTEGER:
      return bigint_compare(&item1->int_val, &item2->int_val);

    case TYPE_ARRAY:
      if (item2->type == TYPE_ARRAY) {
        for (uint32_t i = 0; i < item1->arr_val.length; i++) {
          if (i >= item2->arr_val.length)
            return 1;
          int result = item_compare(&item1->arr_val.items[i],
                                    &item2->arr_val.items[i]);
          if (result != 0)
            return result;
        }
        return item1->arr_val.length - item2->arr_val.length;
      }
      else {
        for (uint32_t i = 0; i < item1->arr_val.length; i++) {
          if (i >= item2->str_val.length)
            return 1;
          if (item1->arr_val.items[i].type != TYPE_INTEGER)
            return 1;
          if (!bigint_fits_in_uint32(&item1->arr_val.items[i].int_val))
            return 1;
          uint32_t int_val = bigint_to_uint32(&item1->arr_val.items[i].int_val);
          if (int_val != item2->str_val.str_data[i]) {
            if (int_val > item2->str_val.str_data[i])
              return 1;
            else
              return -1;
          }
        }
        return item1->arr_val.length - item2->str_val.length;
      }

    case TYPE_STRING:
    case TYPE_BLOCK:
      return string_compare(&item1->str_val, &item2->str_val);

    default:
      assert(false);
  }
}

// Returns whether two items are equal
bool items_equal(const Item *item1, const Item *item2) {
  return item_compare(item1, item2) == 0;
}

// Adds item2 to item1
void items_add(Item *item1, Item *item2) {
  coerce_types(item1, item2);
  if (item1->type == TYPE_INTEGER) {
    bigint_add(&item1->int_val, &item2->int_val);
  }
  else if (item1->type == TYPE_STRING) {
    string_add_str(&item1->str_val, &item2->str_val);
  }
  else if (item1->type == TYPE_BLOCK) {
    string_add_char(&item1->str_val, ' ');
    string_add_str(&item1->str_val, &item2->str_val);
  }
  else if (item1->type == TYPE_ARRAY) {
    for (uint32_t i = 0; i < item2->arr_val.length; i++) {
      array_push(&item1->arr_val, make_copy(&item2->arr_val.items[i]));
    }
  }
}

void swap_items(Item *a, Item *b) {
  Item tmp = *a;
  *a = *b;
  *b = tmp;
}

// Frees the dynamically allocated contents of an item
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
  else if (item->type == TYPE_INTEGER) {
    free_bigint(&item->int_val);
  }
}

void output_item(const Item *item) {
  if (item->type == TYPE_INTEGER) {
    String str = bigint_to_string(&item->int_val);
    string_add_char(&str, '\0');
    printf("%s", str.str_data);
    free_string(&str);
  }
  else if (item->type == TYPE_STRING) {
    for (uint32_t i = 0; i < item->str_val.length; i++) {
      putchar(item->str_val.str_data[i]);
    }
  }
  else if (item->type == TYPE_BLOCK) {
    putchar('{');
    for (uint32_t i = 0; i < item->str_val.length; i++) {
      putchar(item->str_val.str_data[i]);
    }
    putchar('}');
  }
  else if (item->type == TYPE_ARRAY) {
    for (uint32_t i = 0; i < item->arr_val.length; i++) {
      output_item(&item->arr_val.items[i]);
    }
  }
}

// Converts an array to a string
// Note that integers are converted to their ascii equivalents
String array_to_string(const Item *array) {
  String str = new_string();
  for (uint32_t i = 0; i < array->arr_val.length; i++) {
    Item *cur_item = &array->arr_val.items[i];
    if (cur_item->type == TYPE_INTEGER)
      string_add_char(&str, cur_item->int_val.digits[0] & 0xFF);
    else if (cur_item->type == TYPE_STRING)
      string_add_str(&str, &cur_item->str_val);
    else if (cur_item->type == TYPE_BLOCK)
      string_add_str(&str, &cur_item->str_val);
    else if (cur_item->type == TYPE_ARRAY) {
      String array_str = array_to_string(cur_item);
      string_add_str(&str, &array_str);
      free(array_str.str_data);
    }
  }
  return str;
}

// Performs type conversion, converting two items to the same type
// With two different types, the item with the lower type priority
// is converted to other item's type
// The priority from smallest to largest is integer, array, string, block
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
          string_add_str(&block_str, &cur_item->str_val);
        }
        else if (cur_item->type == TYPE_ARRAY) {
          String arr_str = array_to_string(cur_item);
          string_add_str(&block_str, &arr_str);
          free(arr_str.str_data);
        }
        else if (cur_item->type == TYPE_INTEGER) {
          String int_str = bigint_to_string(&cur_item->int_val);
          string_add_str(&block_str, &int_str);
          free_string(&int_str);
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
      Bigint int_temp = item2->int_val;
      item2->str_val = bigint_to_string(&item2->int_val);
      free_bigint(&int_temp);
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
      Bigint int_temp = item2->int_val;
      item2->str_val = bigint_to_string(&item2->int_val);
      free_bigint(&int_temp);
    }
  }
  else if (item1->type == TYPE_ARRAY) {
    if (item2->type == TYPE_INTEGER) {
      Array coerced_array = new_array();
      array_push(&coerced_array, *item2);
      item2->type = TYPE_ARRAY;
      item2->arr_val = coerced_array;
    }
  }
}
