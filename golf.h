// golf.h
// Contains type declarations and forward declarations for everything used
// for the interpreter

#ifndef GOLF_H
#define GOLF_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// An enumeration of the types an item can be
enum Type {
  TYPE_INTEGER,
  TYPE_ARRAY,
  TYPE_STRING,
  TYPE_BLOCK,

  // Not usable by golfscript programmers, only used to implement
  // the builtin functions
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
  enum Type type; // The type of the item
  union {
    int64_t int_val;    // Used for integers
    String str_val;     // Used for strings and blocks
    Array arr_val;      // Used for arrays
    void (*function)(); // Used for builtin functions
  };
} Item;

// A simple hash table
// Used for assigned values and builtin functions
typedef struct Map {
  char **keys;
  Item *items;
  uint32_t num_items, allocated;
} Map;

// Structure for implementing an AVL tree
typedef struct TreeNode {
  Item item;
  struct TreeNode *left, *right;
  uint16_t height;
} TreeNode;

// A set data structure for implementing setwise data structures on arrays
typedef struct Set {
  TreeNode *root;
} Set;

// array.c
Array new_array();
void array_push(Array *arr, Item item);
void array_multiply(Array *array, int64_t factor);
void array_subtract(Array *array, Array *to_subtract);
void array_and(Array *array, Array *to_and);
void array_or(Array *array, Array *to_or);
void array_xor(Array *array, Array *to_xor);

// builtin.c
void builtin_abs();
void builtin_ampersand();
void builtin_asterisk();
void builtin_at();
void builtin_backslash();
void builtin_backtick();
void builtin_bar();
void builtin_caret();
void builtin_comma();
void builtin_do();
void builtin_exclamation();
void builtin_if();
void builtin_lbracket();
void builtin_lparen();
void builtin_minus();
void builtin_period();
void builtin_plus();
void builtin_print();
void builtin_rand();
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
int item_compare(Item *item1, Item *item2);
bool items_equal(Item *item1, Item *item2);
void items_add(Item *item1, Item *item2);
void swap_items(Item *a, Item *b);
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

// set.c
Set new_set();
void free_set(Set *set);
bool set_has(Set *set, Item *item);
void set_add(Set *set, Item *item);
void set_remove(Set *set, Item *item);

// string.c
String new_string();
String create_string(char *str);
void string_reverse(String *str);
void string_add_char(String *str, char c);
void string_add_str(String *str, char *to_append);
void string_multiply(String *str, int64_t factor);
void string_subtract(String *str, String *to_subtract);
void string_setwise_and(String *str, String *to_and);
void string_setwise_or(String *str, String *to_or);
void string_setwise_xor(String *str, String *to_xor);
String int_to_string(int64_t int_val);
String read_file_to_string(FILE *file);

#endif
