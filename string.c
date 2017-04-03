#include <stdlib.h>
#include <string.h>
#include "golf.h"

#define STRING_INIT_SIZE 16

String new_string() {
  String str = {malloc(STRING_INIT_SIZE), 0, STRING_INIT_SIZE};
  str.str_data[0] = '\0';
  return str;
}

String create_string(char *to_copy) {
  size_t old_len = strlen(to_copy);
  uint32_t new_len = STRING_INIT_SIZE;
  while (new_len + 1 < old_len) {
    new_len <<= 1;
  }
  String str = {malloc(new_len), old_len, new_len};
  strcpy(str.str_data, to_copy);
  return str;
}

void string_reverse(String *str) {
  for (uint32_t i = 0; i < str->length / 2; i++) {
    char temp = str->str_data[i];
    str->str_data[i] = str->str_data[str->length - i - 1];
    str->str_data[str->length - i - 1] = temp;
  }
}

void string_add_char(String *str, char c) {
  if (str->length + 1 >= str->allocated) {
    str->allocated <<= 1;
    str->str_data = realloc(str->str_data, str->allocated);
  }
  str->str_data[str->length++] = c;
  str->str_data[str->length] = '\0';
}

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

String read_file_to_string(FILE *file) {
  String str = new_string();
  int c;

  while ((c = fgetc(file)) != EOF) {
    string_add_char(&str, c);
  }

  return str;
}
