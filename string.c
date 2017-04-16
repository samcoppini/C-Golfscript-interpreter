// string.c
// Contains functions for manipulating strings

#include <stdlib.h>
#include <string.h>
#include "golf.h"

#define STRING_INIT_SIZE 16

String new_string() {
  String str = {malloc(STRING_INIT_SIZE), 0, STRING_INIT_SIZE};
  if (str.str_data == NULL) {
    error("Unable to allocate space for new string!");
  }
  return str;
}

void free_string(String *str) {
  free(str->str_data);
}

// Makes sure that the length of the allocated string is at least new_len bytes
// long, and if not, we reallocate more space for the string
void string_request_size(String *str, uint32_t new_len) {
  if (new_len > str->allocated) {
    do {
      str->allocated <<= 1;
    } while (new_len > str->allocated);
    str->str_data = realloc(str->str_data, str->allocated);
    if (str->str_data == NULL) {
      error("Unable to allocate additional space for string!");
    }
  }
}

// Returns a copy of a string
String copy_string(String *str) {
  String new_str = {malloc(str->allocated), str->length, str->allocated};
  if (new_str.str_data == NULL) {
    error("Unable to allocate space for new string!");
  }
  memcpy(new_str.str_data, str->str_data, str->length);
  return new_str;
}

// Creates a String from a C string
String create_string(char *to_copy) {
  size_t old_len = strlen(to_copy);
  uint32_t new_len = STRING_INIT_SIZE;
  while (new_len < old_len) {
    new_len <<= 1;
  }
  String str = {malloc(new_len), old_len, new_len};
  if (str.str_data == NULL) {
    error("Unable to allocate space for new string!");
  }
  memcpy(str.str_data, to_copy, old_len);
  return str;
}

// Compares one string to another, returning a negative value if str1 is less
// than str1, a positive value if str2 is greater, and 0 if they are equal
int string_compare(String *str1, String *str2) {
  uint32_t min_len = min(str1->length, str2->length);
  int result = memcmp(str1->str_data, str2->str_data, min_len);

  if (result != 0)
    return result;

  if (str1->length < str2->length)
    return -1;
  else if (str1->length > str2->length)
    return 1;
  else
    return 0;
}

// Reverses a string in-place
void string_reverse(String *str) {
  for (uint32_t i = 0; i < str->length / 2; i++) {
    char temp = str->str_data[i];
    str->str_data[i] = str->str_data[str->length - i - 1];
    str->str_data[str->length - i - 1] = temp;
  }
}

// Adds a character to the end of a string
void string_add_char(String *str, char c) {
  string_request_size(str, str->length + 1);
  str->str_data[str->length++] = c;
}

// Adds a string to the end of a string
void string_add_str(String *str, String *to_append) {
  string_request_size(str, str->length + to_append->length);
  for (uint32_t i = 0; i < to_append->length; i++) {
    str->str_data[str->length + i] = to_append->str_data[i];
  }
  str->length += to_append->length;
}

// Adds an old-school null-terminated C string to the end of a string
void string_add_c_str(String *str, char *to_append) {
  size_t append_len = strlen(to_append);
  string_request_size(str, str->length + append_len);
  for (uint32_t i = 0; i < append_len; i++) {
    str->str_data[str->length + i] = to_append[i];
  }
  str->length += append_len;
}

// Returns the position of the first occurrence of a character in a string,
// returning -1 if it isn't in the string
int64_t string_find_char(String *str, char c) {
  for (uint32_t i = 0; i < str->length; i++) {
    if (str->str_data[i] == c)
      return i;
  }
  return -1;
}

// Returns the position of the first occurrence of to_find in str,
// returning -1 if it's not in the string
int64_t string_find_str(String *str, String *to_find) {
  for (uint32_t i = 0; i + to_find->length <= str->length; i++) {
    if (memcmp(str->str_data + i, to_find->str_data, to_find->length) == 0)
      return i;
  }
  return -1;
}

// Replaces a string with the string from every nth step, e.g. with a step size
// of 3, "wordsmith" becomes "wdi". With a negative step size, the steps begin
// from the end of the string
void string_step_over(String *str, int64_t step_size) {
  if (step_size == 0) {
    error("Step size of string select must be nonzero!");
  }
  else if (step_size < 0) {
    string_reverse(str);
    step_size *= -1;
  }
  if (step_size == 1) {
    return;
  }
  for (uint32_t i = 1; i * step_size < str->length; i++) {
    str->str_data[i] = str->str_data[i * step_size];
  }
  str->length = (str->length + 1) / step_size;
}

// Splits a string into parts divided by a given seperator string
Item string_split(String *str, String *sep) {
  Item arr = make_array();
  Item cur_string = empty_string();

  for (uint32_t i = 0; i < str->length; i++) {
    if (str->length - i > sep->length - 1 &&
        memcmp(str->str_data + i, sep->str_data, sep->length) == 0)
    {
      array_push(&arr.arr_val, cur_string);
      cur_string = empty_string();
      i += sep->length - 1;
    }
    else {
      string_add_char(&cur_string.str_val, str->str_data[i]);
    }
  }
  array_push(&arr.arr_val, cur_string);
  return arr;
}

// Returns an array item consisting of the given string split up into
// substrings of a given length
Item string_split_into_groups(String *str, int64_t group_size) {
  Item array = make_array();
  Item cur_string = empty_string();

  if (group_size < 0) {
    string_reverse(str);
    group_size *= -1;
  }
  else if (group_size == 0) {
    error("Cannot split string into groups of size 0!");
  }

  for (uint32_t i = 0; i < str->length; i++) {
    string_add_char(&cur_string.str_val, str->str_data[i]);
    if (cur_string.str_val.length == group_size) {
      array_push(&array.arr_val, cur_string);
      cur_string = empty_string();
    }
  }

  if (cur_string.str_val.length > 0) {
    array_push(&array.arr_val, cur_string);
  }

  return array;
}

// Remove a certain number of characters from the start of a string
void string_remove_from_front(String *str, int64_t to_remove) {
  if (str->length - to_remove <= 0) {
    str->length = 0;
    return;
  }
  else {
    str->length -= to_remove;
  }

  for (uint32_t i = 0; i < str->length; i++) {
    str->str_data[i] = str->str_data[i + to_remove];
  }
}

Item string_join(String *str, String *sep) {
  Item joined_str = empty_string();
  for (uint32_t i = 0; i < str->length; i++) {
    string_add_char(&joined_str.str_val, str->str_data[i]);
    if (i + 1 < str->length) {
      string_add_str(&joined_str.str_val, sep);
    }
  }
  return joined_str;
}

void fold_string(String *str, Item *block) {
  if (str->length > 0) {
    stack_push(make_integer(str->str_data[0]));
    for (uint32_t i = 1; i < str->length; i++) {
      stack_push(make_integer(str->str_data[i]));
      execute_string(&block->str_val);
    }
  }
}

void filter_string(String *str, Item *block) {
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    stack_push(make_integer(str->str_data[i]));
    execute_string(&block->str_val);
    Item mapped_item = stack_pop();
    if (item_boolean(&mapped_item)) {
      str->str_data[i - chars_removed] = str->str_data[i];
    }
    else {
      chars_removed++;
    }
    free_item(&mapped_item);
  }
  str->length -= chars_removed;
}

// Sorts a string. Utilizes counting sort and runs in O(n) time
void string_sort(String *str) {
  uint32_t counts[256] = {0};
  for (uint32_t i = 0; i < str->length; i++) {
    counts[str->str_data[i]]++;
  }
  uint32_t cur_index = 0;
  for (uint32_t i = 0; i < 256; i++) {
    while (counts[i] > 0) {
      str->str_data[cur_index++] = i;
      counts[i]--;
    }
  }
}

// Repeats a string a given number of times
void string_multiply(String *str, int64_t factor) {
  if (factor < 0) {
    error("Cannot multiply array by a negative argument!");
  }
  uint32_t new_len = str->length * factor;
  string_request_size(str, new_len);
  uint32_t cur_len = str->length;
  while (factor > 1) {
    factor -= 1;

    for (uint32_t i = 0; i < str->length; i++) {
      str->str_data[i + cur_len] = str->str_data[i];
    }

    cur_len += str->length;
  }
  str->length = new_len;
}

// Removes the characters in to_subtract from str
void string_subtract(String *str, String *to_subtract) {
  bool subtracted_chars[256] = {0};
  for (uint32_t i = 0; i < to_subtract->length; i++) {
    subtracted_chars[to_subtract->str_data[i]] = true;
  }
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (subtracted_chars[str->str_data[i]]) {
      chars_removed++;
    }
    else {
      str->str_data[i - chars_removed] = str->str_data[i];
    }
  }
  str->length -= chars_removed;
}

// Removes all characters from str that aren't in to_and, and removes
// duplicate characters
void string_setwise_and(String *str, String *to_and) {
  bool present_chars[256] = {0};
  for (uint32_t i = 0; i < to_and->length; i++) {
    present_chars[to_and->str_data[i]] = true;
  }
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (present_chars[str->str_data[i]]) {
      present_chars[str->str_data[i]] = false;
      str->str_data[i - chars_removed] = str->str_data[i];
    }
    else {
      chars_removed++;
    }
  }
  str->length -= chars_removed;
}

// Replaces str with the union of str and to_or
void string_setwise_or(String *str, String *to_or) {
  bool present_chars[256] = {0};
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (present_chars[str->str_data[i]]) {
      chars_removed++;
    }
    else {
      present_chars[str->str_data[i]] = true;
      str->str_data[i - chars_removed] = str->str_data[i];
    }
  }
  str->length -= chars_removed;
  for (uint32_t i = 0; i < to_or->length; i++) {
    if (!present_chars[to_or->str_data[i]]) {
      present_chars[to_or->str_data[i]] = true;
      string_add_char(str, to_or->str_data[i]);
    }
  }
}

// Replaces str with the setwise symmetric difference of str and to_xor
void string_setwise_xor(String *str, String *to_xor) {
  bool in_string1[256] = {0};
  bool in_string2[256] = {0};
  for (uint32_t i = 0; i < to_xor->length; i++) {
    in_string2[to_xor->str_data[i]] = true;
  }
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (in_string2[str->str_data[i]]) {
      in_string1[str->str_data[i]] = true;
      chars_removed++;
    }
    else if (in_string1[str->str_data[i]]) {
      chars_removed++;
    }
    else {
      in_string1[str->str_data[i]] = true;
      str->str_data[i - chars_removed] = str->str_data[i];
    }
  }
  str->length -= chars_removed;
  for (uint32_t i = 0; i < to_xor->length; i++) {
    if (!in_string1[to_xor->str_data[i]] && in_string2[to_xor->str_data[i]]) {
      in_string2[to_xor->str_data[i]] = false;
      string_add_char(str, to_xor->str_data[i]);
    }
  }
}

// Converts an integer to its string representation
String int_to_string(int64_t int_val) {
  String str = new_string();
  bool was_negative = int_val < 0;

  if (int_val < 0)
    int_val *= -1;
  else if (int_val == 0)
    string_add_char(&str, '0');

  while (int_val > 0) {
    string_add_char(&str, '0' + (int_val % 10));
    int_val /= 10;
  }

  if (was_negative)
    string_add_char(&str, '-');
  string_reverse(&str);

  return str;
}

// Converts a string to an integer, assuming it represents an int
int64_t string_to_int(String *str) {
  int64_t val = 0;
  uint32_t cur_pos = 0;
  bool was_negative = false;
  if (str->str_data[0] == '-') {
    was_negative = true;
    cur_pos++;
  }
  while (cur_pos < str->length) {
    val *= 10;
    val += str->str_data[cur_pos++] - '0';
  }
  if (was_negative)
    val *= -1;
  return val;
}

// Reads the complete contents of a file to a string
String read_file_to_string(FILE *file) {
  String str = new_string();
  int c;

  while ((c = fgetc(file)) != EOF) {
    string_add_char(&str, c);
  }

  return str;
}
