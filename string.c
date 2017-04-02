#include <stdlib.h>
#include "golf.h"

#define STRING_INIT_SIZE 16

String new_string() {
  String str = {malloc(STRING_INIT_SIZE), 0, STRING_INIT_SIZE};
  str.str_data[0] = '\0';
  return str;
}

void string_add_char(String *str, char c) {
  if (str->length + 1 >= str->allocated) {
    str->allocated <<= 1;
    str->str_data = realloc(str->str_data, str->allocated);
  }
  str->str_data[str->length++] = c;
  str->str_data[str->length] = '\0';
}

String read_file_to_string(FILE *file) {
  String str = new_string();
  int c;

  while ((c = fgetc(file)) != EOF) {
    string_add_char(&str, c);
  }

  return str;
}
