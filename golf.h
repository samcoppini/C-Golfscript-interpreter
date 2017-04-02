#ifndef GOLF_H
#define GOLF_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum Type {
  TYPE_INTEGER,
  TYPE_STRING,
  TYPE_ARRAY,
  TYPE_BLOCK,
  TYPE_FUNCTION
};

typedef struct String {
  char *str_data;
  uint32_t length, allocated;
} String;

typedef struct Item {
  enum Type type;
  union {
    int64_t int_val;
    String str_val;
    void (*function)();
  };
} Item;

typedef struct Array {
  Item *items;
  uint32_t length, allocated;
} Array;

typedef struct Map {
  char **keys;
  Item *items;
  uint32_t num_items, allocated;
} Map;

// array.c
Array new_array();
void array_push(Array *arr, Item item);

// builtin.c
void builtin_semicolon();

// item.c
Item make_integer(int64_t int_val);
Item make_string(char *str_val);
Item make_builtin(void (*function)());
void free_item(Item *item);
void output_item(Item *item);

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
void string_add_char(String *str, char c);
String read_file_to_string(FILE *file);

#endif
