// string.c
// Contains functions for manipulating strings

#include <stdlib.h>
#include <string.h>
#include "golf.h"

#define STRING_INIT_SIZE 16

String new_string() {
  String str = {malloc(STRING_INIT_SIZE), 0, STRING_INIT_SIZE};
  str.str_data[0] = '\0';
  return str;
}

// Creates a String from a C string
String create_string(char *to_copy) {
  size_t old_len = strlen(to_copy);
  uint32_t new_len = STRING_INIT_SIZE;
  while (new_len - 1 < old_len) {
    new_len <<= 1;
  }
  String str = {malloc(new_len), old_len, new_len};
  strcpy(str.str_data, to_copy);
  return str;
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
  if (str->length + 1 >= str->allocated) {
    str->allocated <<= 1;
    str->str_data = realloc(str->str_data, str->allocated);
  }
  str->str_data[str->length++] = c;
  str->str_data[str->length] = '\0';
}

// Adds a C string to the end of a String
void string_add_str(String *str, char *to_append) {
  size_t append_len = strlen(to_append);
  if (str->length + append_len >= str->allocated) {
    while (str->length + append_len >= str->allocated) {
      str->allocated <<= 1;
    }
    str->str_data = realloc(str->str_data, str->allocated);
  }
  strcat(str->str_data + str->length, to_append);
  str->length += append_len;
}

void string_multiply(String *str, int64_t factor) {
  if (factor < 0) {
    fprintf(stderr, "Error! Cannot multiply array by a negative argument!\n");
    exit(1);
  }
  uint32_t new_len = str->length * factor;
  if (new_len + 1 > str->allocated) {
    while (new_len + 1 > str->allocated) {
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
  str->str_data[new_len] = '\0';
}

// Removes the characters in to_subtract from str
void string_subtract(String *str, String *to_subtract) {
  bool subtracted_chars[256] = {0};
  for (uint32_t i = 0; i < to_subtract->length; i++) {
    subtracted_chars[(unsigned) to_subtract->str_data[i]] = true;
  }
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (subtracted_chars[(unsigned) str->str_data[i]]) {
      chars_removed++;
    }
    else {
      str->str_data[i - chars_removed] = str->str_data[i];
    }
  }
  str->length -= chars_removed;
  str->str_data[str->length] = '\0';
}

// Removes all characters from str that aren't in to_and, and removes
// duplicate characters
void string_setwise_and(String *str, String *to_and) {
  bool present_chars[256] = {0};
  for (uint32_t i = 0; i < to_and->length; i++) {
    present_chars[(unsigned) to_and->str_data[i]] = true;
  }
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (present_chars[(unsigned) str->str_data[i]]) {
      present_chars[(unsigned) str->str_data[i]] = false;
      str->str_data[i - chars_removed] = str->str_data[i];
    }
    else {
      chars_removed++;
    }
  }
  str->length -= chars_removed;
  str->str_data[str->length] = '\0';
}

// Replaces str with the union of str and to_or
void string_setwise_or(String *str, String *to_or) {
  bool present_chars[256] = {0};
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (present_chars[(unsigned) str->str_data[i]]) {
      chars_removed++;
    }
    else {
      present_chars[(unsigned) str->str_data[i]] = true;
      str->str_data[i - chars_removed] = str->str_data[i];
    }
  }
  str->length -= chars_removed;
  str->str_data[str->length] = '\0';
  for (uint32_t i = 0; i < to_or->length; i++) {
    if (!present_chars[(unsigned) to_or->str_data[i]]) {
      present_chars[(unsigned) to_or->str_data[i]] = true;
      string_add_char(str, to_or->str_data[i]);
    }
  }
}

// Replaces str with the setwise symmetric difference of str and to_xor
void string_setwise_xor(String *str, String *to_xor) {
  bool in_string1[256] = {0};
  bool in_string2[256] = {0};
  for (uint32_t i = 0; i < to_xor->length; i++) {
    in_string2[(unsigned) to_xor->str_data[i]] = true;
  }
  uint32_t chars_removed = 0;
  for (uint32_t i = 0; i < str->length; i++) {
    if (in_string2[(unsigned) str->str_data[i]]) {
      in_string1[(unsigned) str->str_data[i]] = true;
      chars_removed++;
    }
    else if (in_string1[(unsigned) str->str_data[i]]) {
      chars_removed++;
    }
    else {
      in_string1[(unsigned) str->str_data[i]] = true;
      str->str_data[i - chars_removed] = str->str_data[i];
    }
  }
  str->length -= chars_removed;
  str->str_data[str->length] = 0;
  for (uint32_t i = 0; i < to_xor->length; i++) {
    if (!in_string1[(unsigned) to_xor->str_data[i]] &&
         in_string2[(unsigned) to_xor->str_data[i]])
    {
      in_string2[(unsigned) to_xor->str_data[i]] = false;
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

// Reads the complete contents of a file to a string
String read_file_to_string(FILE *file) {
  String str = new_string();
  int c;

  while ((c = fgetc(file)) != EOF) {
    string_add_char(&str, c);
  }

  return str;
}
