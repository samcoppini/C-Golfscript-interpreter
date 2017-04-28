// golf.h
// Contains type declarations and forward declarations for everything used
// for the interpreter

#ifndef GOLF_H
#define GOLF_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdnoreturn.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

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
  unsigned char *str_data;
  uint32_t length, allocated;
} String;

struct Item;

typedef struct Array {
  struct Item *items;
  uint32_t length, allocated;
} Array;

typedef struct Bigint {
  uint64_t *digits;
  uint32_t length, allocated;
  bool is_negative;
} Bigint;

typedef struct Item {
  enum Type type; // The type of the item
  union {
    Bigint int_val;     // Used for integers
    String str_val;     // Used for strings and blocks
    Array arr_val;      // Used for arrays
    void (*function)(); // Used for builtin functions
  };
} Item;

// A simple hash table
// Used for assigned values and builtin functions
typedef struct Map {
  String **keys;
  Item *items;
  uint32_t num_items, allocated;
} Map;

// Structure for implementing an AVL tree
typedef struct TreeNode {
  Item item;
  struct TreeNode *left, *right;
  uint16_t height;
} TreeNode;

// A set data structure for implementing setwise data operations on arrays
typedef struct Set {
  TreeNode *root;
} Set;

extern Array stack;
extern Array bracket_stack;

// array.c
Array new_array();
void free_array(Array *array);
Array array_from_string(const String *str);
void array_push(Array *arr, Item item);
void array_remove_from_front(Array *array, Bigint to_remove);
int64_t array_find(const Array *arr, const Item *item);
void array_reverse(Array *array);
Item join_array(Array *array, const Item *sep);
void map_array(Array *array, Item *block);
void fold_array(Array *array, Item *block);
void filter_array(Array *array, Item *block);
void array_remove_empty_strings(Array *array);
void array_remove_empty_arrays(Array *array);
void array_multiply(Array *array, Bigint factor);
void array_subtract(Array *array, const Array *to_subtract);
void array_split(Array *array, const Array *sep);
void array_split_into_groups(Array *array, Bigint group_len);
void array_step_over(Array *array, Bigint step_size);
void array_sort(Array *array);
void array_sort_by_mapping(Array *array, const Array *mapped_array);
void string_sort_by_mapping(String *str, const Array *mapped_str);
void array_and(Array *array, const Array *to_and);
void array_or(Array *array, const Array *to_or);
void array_xor(Array *array, const Array *to_xor);

// bigint.c
Bigint new_bigint();
Bigint bigint_with_digits(uint32_t num_digits);
void free_bigint(Bigint *num);
Bigint bigint_from_int64(int64_t int_val);
Bigint bigint_from_string(const String *str);
String bigint_to_string(const Bigint *num);
bool bigint_fits_in_uint32(const Bigint *num);
uint32_t bigint_to_uint32(const Bigint *num);
Bigint copy_bigint(const Bigint *to_copy);
bool bigint_is_zero(const Bigint *num);
void bigint_increment(Bigint *num);
void bigint_decrement(Bigint *num);
int bigint_compare(const Bigint *a, const Bigint *b);
Bigint bigint_or(const Bigint *a, const Bigint *b);
Bigint bigint_and(const Bigint *a, const Bigint *b);
Bigint bigint_xor(const Bigint *a, const Bigint *b);
void bigint_add(Bigint *a, const Bigint *b);
void bigint_subtract(Bigint *a, const Bigint *b);
Bigint bigint_multiply(const Bigint *a, const Bigint *b);
void bigint_divmod(const Bigint *a, const Bigint *b, Bigint *quotient_result,
                   Bigint *remainder_result);
Bigint bigint_exponent(const Bigint *base, const Bigint *exponent);

// builtin.c
void builtin_abs();
void builtin_ampersand();
void builtin_asterisk();
void builtin_at();
void builtin_backslash();
void builtin_backtick();
void builtin_bar();
void builtin_base();
void builtin_caret();
void builtin_comma();
void builtin_do();
void builtin_dollar_sign();
void builtin_equal();
void builtin_exclamation();
void builtin_greater_than();
void builtin_if();
void builtin_lbracket();
void builtin_less_than();
void builtin_lparen();
void builtin_minus();
void builtin_percent();
void builtin_period();
void builtin_plus();
void builtin_print();
void builtin_question();
void builtin_rand();
void builtin_rbracket();
void builtin_rparen();
void builtin_semicolon();
void builtin_slash();
void builtin_until();
void builtin_tilde();
void builtin_while();
void builtin_zip();

// error.c
noreturn void error(const char *msg, ...);

// item.c
Item make_integer(int64_t int_val);
Item make_integer_from_bigint(const Bigint *bigint);
Item make_string(const String *str_val);
Item empty_string();
Item make_block(String str_val);
Item make_array();
Item make_builtin(void (*function)());
Item make_copy(const Item *item);
String get_literal(const Item *item);
bool item_boolean(const Item *item);
int item_compare(const Item *item1, const Item *item2);
bool items_equal(const Item *item1, const Item *item2);
void items_add(Item *item1, Item *item2);
void swap_items(Item *a, Item *b);
void free_item(Item *item);
void output_item(const Item *item);
String array_to_string(const Item *array);
void coerce_types(Item *item1, Item *item2);

// execute.c
void init_interpreter();
void end_interpreter();
void stack_push(Item item);
Item stack_pop();
void execute_string(String *str);
void repeat_block(Item *block, Bigint times);
void execute_item(Item *item);

// map.c
Map new_map();
void free_map(Map *map);
void map_set(Map *map, String key, Item item);
Item *map_get(Map *map, const String *key);

// random.c
void init_rng();
Bigint get_randint(Bigint max_val);

// set.c
Set new_set();
void free_set(Set *set);
bool set_has(const Set *set, const Item *item);
void set_add(Set *set, const Item *item);
void set_remove(Set *set, const Item *item);

// string.c
String new_string();
void free_string(String *str);
String copy_string(const String *str);
String create_string(const char *str);
int string_compare(const String *str1, const String *str2);
void string_reverse(String *str);
void string_add_char(String *str, char c);
void string_add_str(String *str, const String *to_append);
void string_add_c_str(String *str, const char *to_append);
void string_remove_from_front(String *str, Bigint to_remove);
Item string_join(String *str, String *sep);
void map_string(String *str, Item *block);
void fold_string(String *str, Item *block);
void filter_string(String *str, Item *block);
int64_t string_find_char(const String *str, char c);
int64_t string_find_str(const String *str, const String *to_find);
Item string_split(String *str, const String *sep);
Item string_split_into_groups(String *str, Bigint group_len);
void string_step_over(String *str, Bigint step_size);
void string_sort(String *str);
void string_multiply(String *str, Bigint factor);
void string_subtract(String *str, const String *to_subtract);
void string_setwise_and(String *str, const String *to_and);
void string_setwise_or(String *str, const String *to_or);
void string_setwise_xor(String *str, const String *to_xor);
String read_file_to_string(FILE *file);

#endif
