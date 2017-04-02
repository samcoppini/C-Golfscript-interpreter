#ifndef GOLF_H
#define GOLF_H

#include <stdint.h>
#include <stdio.h>

enum Type {
  TYPE_INTEGER,
  TYPE_STRING,
  TYPE_ARRAY,
  TYPE_BLOCK
};

typedef struct String {
  char *str_data;
  uint32_t length, allocated;
} String;

typedef struct Item {
  enum Type type;
  union {
    int64_t int_val;
  };
} Item;

typedef struct Array {
  Item *items;
  uint32_t length, allocated;
} Array;

// array.c
Array new_array();
void array_push(Array *arr, Item item);

// item.c
Item make_integer(int64_t int_val);
void output_item(Item *item);

// execute.c
void init_stack();
void stack_push(Item item);
void execute_string(String *str);
void output_final_stack();

// string.c
String new_string();
void string_add_char(String *str, char c);
String read_file_to_string(FILE *file);

#endif
