// string.c
// Contains functions for manipulating strings

#include <stdlib.h>
#include <string.h>
#include "golf.h"

#define STRING_INIT_SIZE 16

String new_string() {
  String str = {malloc(STRING_INIT_SIZE), 0, STRING_INIT_SIZE};
  return str;
}

String copy_string(String *str) {
  String new_str = {malloc(str->allocated), str->length, str->allocated};
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
  strcpy(str.str_data, to_copy);
  return str;
}

int string_compare(String *str1, String *str2) {
  uint32_t min_len = min(str1->length, str2->length);
  int result = strncmp(str1->str_data, str2->str_data, min_len);

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
  if (str->length >= str->allocated) {
    str->allocated <<= 1;
    str->str_data = realloc(str->str_data, str->allocated);
  }
  str->str_data[str->length++] = c;
}

// Adds a string to the end of a string
void string_add_str(String *str, String *to_append) {
  if (str->length + to_append->length >= str->allocated) {
    while (str->length + to_append->length >= str->allocated) {
      str->allocated <<= 1;
    }
    str->str_data = realloc(str->str_data, str->allocated);
  }
  for (uint32_t i = 0; i < to_append->length; i++) {
    str->str_data[str->length + i] = to_append->str_data[i];
  }
  str->length += to_append->length;
}

// Adds an old-school null-terminated C string to the end of a string
void string_add_c_str(String *str, char *to_append) {
  size_t append_len = strlen(to_append);
  if (str->length + append_len >= str->allocated) {
    while (str->length + append_len >= str->allocated) {
      str->allocated <<= 1;
    }
    str->str_data = realloc(str->str_data, str->allocated);
  }
  for (uint32_t i = 0; i < append_len; i++) {
    str->str_data[str->length + i] = to_append[i];
  }
  str->length += append_len;
}

int64_t string_find_char(String *str, char c) {
  for (uint32_t i = 0; i < str->length; i++) {
    if (str->str_data[i] == c)
      return i;
  }
  return -1;
}

int64_t string_find_str(String *str, String *to_find) {
  for (uint32_t i = 0; i + to_find->length <= str->length; i++) {
    if (strncmp(str->str_data + i, to_find->str_data, to_find->length) == 0)
      return i;
  }
  return -1;
}

void string_step_over(String *str, int64_t step_size) {
  if (step_size == 0) {
    fprintf(stderr, "Error! Step size of string select must be nonzero!\n");
    exit(1);
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

Item string_split(String *str, String *sep) {
  Item arr = make_array();
  Item cur_string = empty_string();

  for (uint32_t i = 0; i < str->length - sep->length + 1; i++) {
    if (strncmp(str->str_data + i, sep->str_data, sep->length) == 0) {
      array_push(&arr.arr_val, cur_string);
      cur_string = empty_string();
    }
    else {
      string_add_char(&cur_string.str_val, str->str_data[i]);
    }
  }
  array_push(&arr.arr_val, cur_string);
  return arr;
}

Item string_split_into_groups(String *str, int64_t group_size) {
  Item array = make_array();
  Item cur_string = empty_string();

  if (group_size < 0) {
    string_reverse(str);
    group_size *= -1;
  }
  else if (group_size == 0) {
    fprintf(stderr, "Error! Cannot split string into groups of size 0!\n");
    exit(1);
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

void string_sort(String *str) {
  uint32_t counts[256] = {0};
  for (uint32_t i = 0; i < str->length; i++) {
    counts[(unsigned char) str->str_data[i]]++;
  }
  uint32_t cur_index = 0;
  for (uint32_t i = 0; i < 256; i++) {
    while (counts[i] > 0) {
      str->str_data[cur_index++] = (char) i;
      counts[i]--;
    }
  }
}

void string_multiply(String *str, int64_t factor) {
  if (factor < 0) {
    fprintf(stderr, "Error! Cannot multiply array by a negative argument!\n");
    exit(1);
  }
  uint32_t new_len = str->length * factor;
  if (new_len > str->allocated) {
    while (new_len > str->allocated) {
      str->allocated <<= 1;
    }
    str->str_data = realloc(str->str_data, str->allocated);
  }
  uint32_t cur_len = str->length;
  while (factor-- > 0) {
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
    subtracted_chars[(unsigned char) to_subtract->str_data[i]] = true;
  }
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (subtracted_chars[(unsigned char) str->str_data[i]]) {
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
    present_chars[(unsigned char) to_and->str_data[i]] = true;
  }
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (present_chars[(unsigned char) str->str_data[i]]) {
      present_chars[(unsigned char) str->str_data[i]] = false;
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
    if (present_chars[(unsigned char) str->str_data[i]]) {
      chars_removed++;
    }
    else {
      present_chars[(unsigned char) str->str_data[i]] = true;
      str->str_data[i - chars_removed] = str->str_data[i];
    }
  }
  str->length -= chars_removed;
  for (uint32_t i = 0; i < to_or->length; i++) {
    if (!present_chars[(unsigned char) to_or->str_data[i]]) {
      present_chars[(unsigned char) to_or->str_data[i]] = true;
      string_add_char(str, to_or->str_data[i]);
    }
  }
}

// Replaces str with the setwise symmetric difference of str and to_xor
void string_setwise_xor(String *str, String *to_xor) {
  bool in_string1[256] = {0};
  bool in_string2[256] = {0};
  for (uint32_t i = 0; i < to_xor->length; i++) {
    in_string2[(unsigned char) to_xor->str_data[i]] = true;
  }
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (in_string2[(unsigned char) str->str_data[i]]) {
      in_string1[(unsigned char) str->str_data[i]] = true;
      chars_removed++;
    }
    else if (in_string1[(unsigned char) str->str_data[i]]) {
      chars_removed++;
    }
    else {
      in_string1[(unsigned char) str->str_data[i]] = true;
      str->str_data[i - chars_removed] = str->str_data[i];
    }
  }
  str->length -= chars_removed;
  for (uint32_t i = 0; i < to_xor->length; i++) {
    if (!in_string1[(unsigned char) to_xor->str_data[i]] &&
         in_string2[(unsigned char) to_xor->str_data[i]])
    {
      in_string2[(unsigned char) to_xor->str_data[i]] = false;
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
