#include <ctype.h>
#include <stdlib.h>
#include "golf.h"

Array stack;
Array bracket_stack;

static Map definitions;

void init_interpreter() {
  stack = new_array();
  bracket_stack = new_array();

  definitions = new_map();
  map_set(&definitions, "[", make_builtin(builtin_lbracket));
  map_set(&definitions, "]", make_builtin(builtin_rbracket));
  map_set(&definitions, ".", make_builtin(builtin_period));
  map_set(&definitions, ";", make_builtin(builtin_semicolon));
  map_set(&definitions, "~", make_builtin(builtin_tilde));
  map_set(&definitions, "\\", make_builtin(builtin_backslash));
  map_set(&definitions, "print", make_builtin(builtin_print));
  map_set(&definitions, "n", make_string("\"\n\""));
  map_set(&definitions, "puts", make_string("print n print"));
}

void stack_push(Item item) {
  array_push(&stack, item);
}

Item stack_pop() {
  if (stack.length == 0) {
    fprintf(stderr, "Error! Cannot pop from empty stack!\n");
    exit(1);
  }
  stack.length--;

  for (uint32_t i = 0; i < bracket_stack.length; i++) {
    if (stack.length < bracket_stack.items[i].int_val)
      bracket_stack.items[i].int_val = stack.length;
  }

  return stack.items[stack.length];
}

// Return the next token in the string from the given code position
String next_token(String *str, uint32_t *code_pos) {
  String token = new_string();

  if (*code_pos >= str->length)
    return token;

  char c = str->str_data[*code_pos];
  if (c == '"') {
    do {
      string_add_char(&token, c);
      c = str->str_data[++(*code_pos)];
    } while (c != '"' && *code_pos < str->length);

    *code_pos += 1;

    if (*code_pos > str->length) {
      fprintf(stderr, "Error! Unmatched \" encountered in the code!\n");
      exit(1);
    }
  }
  else if (c == '-') {
    string_add_char(&token, c);
    c = str->str_data[++(*code_pos)];
    while (isdigit(c)) {
      string_add_char(&token, c);
      c = str->str_data[++(*code_pos)];
    }
  }
  else if (isdigit(c)) {
    do {
      string_add_char(&token, c);
      c = str->str_data[++(*code_pos)];
    } while (isdigit(c));
  }
  else if (isalpha(c)) {
    do {
      string_add_char(&token, c);
      c = str->str_data[++(*code_pos)];
    } while (isalnum(c));
  }
  else if (c == '{') {
    int brace_level = 1;
    do {
      string_add_char(&token, c);
      c = str->str_data[++(*code_pos)];
      if (c == '{')
        brace_level++;
      else if (c == '}')
        brace_level--;
    } while (brace_level > 0 && *code_pos < str->length);

    *code_pos += 1;

    if (*code_pos > str->length) {
      fprintf(stderr, "Error! Unmatched { encountered in the code!\n");
      exit(1);
    }
  }
  else {
    string_add_char(&token, c);
    *code_pos += 1;
  }

  return token;
}

void execute_string(String *str) {
  uint32_t code_pos = 0;

  while (code_pos < str->length) {
    String tok = next_token(str, &code_pos);
    if (map_has(&definitions, tok.str_data)) {
      execute_item(map_get(&definitions, tok.str_data));
    }
    else if (isdigit(tok.str_data[0]) ||
             (tok.str_data[0] == '-' && tok.length > 1))
    {
      stack_push(make_integer(atoll(tok.str_data)));
    }
    else if (tok.str_data[0] == '"') {
      stack_push(make_string(tok.str_data + 1));
    }
    else if (tok.str_data[0] == '{') {
      stack_push(make_block(tok.str_data + 1));
    }
    free(tok.str_data);
  }
}

void execute_item(Item *item) {
  if (item->type == TYPE_FUNCTION) {
    item->function();
  }
  else if (item->type == TYPE_STRING || item->type == TYPE_BLOCK) {
    execute_string(&item->str_val);
  }
}

void output_final_stack() {
  for (uint32_t i = 0; i < stack.length; i++) {
    output_item(&stack.items[i]);
  }
}
