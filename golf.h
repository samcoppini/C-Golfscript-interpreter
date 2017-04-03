#ifndef GOLF_H
#define GOLF_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum Type {
  TYPE_INTEGER,
  TYPE_ARRAY,
  TYPE_STRING,
  TYPE_BLOCK,
  TYPE_FUNCTION
};

typedef struct String {
  char *str_data;
  uint32_t length, allocated;
} String;

struct Item;

typedef struct Array {
  struct Item *items;
  uint32_t length, allocated;
} Array;

typedef struct Item {
  enum Type type;
  union {
    int64_t int_val;
    String str_val;
    Array arr_val;
    void (*function)();
  };
} Item;

typedef struct Map {
  char **keys;
  Item *items;
  uint32_t num_items, allocated;
} Map;

// array.c
Array new_array();
void array_push(Array *arr, Item item);
void array_subtract(Array *array, Array *to_subtract);
void array_and(Array *array, Array *to_and);

// builtin.c
void builtin_abs();
void builtin_ampersand();
void builtin_at();
void builtin_backslash();
void builtin_backtick();
void builtin_comma();
void builtin_exclamation();
void builtin_if();
void builtin_lbracket();
void builtin_lparen();
void builtin_minus();
void builtin_period();
void builtin_plus();
void builtin_print();
void builtin_rbracket();
void builtin_rparen();
void builtin_semicolon();
void builtin_until();
void builtin_tilde();
void builtin_while();

// item.c
Item make_integer(int64_t int_val);
Item make_string(char *str_val);
Item make_block(char *str_val);
Item make_array();
Item make_builtin(void (*function)());
Item make_copy(Item *item);
String get_literal(Item *item);
bool item_boolean(Item *item);
bool items_equal(Item *item1, Item *item2);
void free_item(Item *item);
void output_item(Item *item);
String array_to_string(Item *array);
void coerce_types(Item *item1, Item *item2);

// execute.c
void init_interpreter();
void stack_push(Item item);
Item stack_pop();
void execute_string(String *str);
void execute_item(Item *item);
void output_final_stack();

// map.c
Map new_map();
bool map_has(Map *map, char *key);
void map_set(Map *map, char *key, Item item);
Item *map_get(Map *map, char *key);

// string.c
String new_string();
String create_string(char *str);
void string_reverse(String *str);
void string_add_char(String *str, char c);
void string_add_str(String *str, char *to_append);
void string_subtract(String *str, String *to_subtract);
void string_setwise_and(String *str, String *to_and);
String int_to_string(int64_t int_val);
String read_file_to_string(FILE *file);

#endif
