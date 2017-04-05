// execute.c
// Contains functions for executing golfscript code, and for starting/ending
// the interpreter

#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "golf.h"

Array stack;
Array bracket_stack;

static Map definitions;

void init_interpreter() {
  // Initializes the program's stack
  stack = new_array();

  // Pushes the input onto the stack
  // If input is not being piped into the program, it pushes an empty string
  if (isatty(STDIN_FILENO)) {
    stack_push(make_string(""));
  }
  else {
    Item first_item = {TYPE_STRING, .str_val = read_file_to_string(stdin)};
    stack_push(first_item);
  }

  // Initializes the array used to keep track of the size of the stack
  // after a left bracket is executed
  bracket_stack = new_array();

  // Initializes the built-in functions
  definitions = new_map();
  map_set(&definitions, "&",     make_builtin(builtin_ampersand));
  map_set(&definitions, "*",     make_builtin(builtin_asterisk));
  map_set(&definitions, "@",     make_builtin(builtin_at));
  map_set(&definitions, "\\",    make_builtin(builtin_backslash));
  map_set(&definitions, "`",     make_builtin(builtin_backtick));
  map_set(&definitions, "|",     make_builtin(builtin_bar));
  map_set(&definitions, "^",     make_builtin(builtin_caret));
  map_set(&definitions, ",",     make_builtin(builtin_comma));
  map_set(&definitions, "!",     make_builtin(builtin_exclamation));
  map_set(&definitions, "[",     make_builtin(builtin_lbracket));
  map_set(&definitions, "(",     make_builtin(builtin_lparen));
  map_set(&definitions, "-",     make_builtin(builtin_minus));
  map_set(&definitions, ".",     make_builtin(builtin_period));
  map_set(&definitions, "+",     make_builtin(builtin_plus));
  map_set(&definitions, "]",     make_builtin(builtin_rbracket));
  map_set(&definitions, ")",     make_builtin(builtin_rparen));
  map_set(&definitions, ";",     make_builtin(builtin_semicolon));
  map_set(&definitions, "~",     make_builtin(builtin_tilde));
  map_set(&definitions, "abs",   make_builtin(builtin_abs));
  map_set(&definitions, "do",    make_builtin(builtin_do));
  map_set(&definitions, "if",    make_builtin(builtin_if));
  map_set(&definitions, "print", make_builtin(builtin_print));
  map_set(&definitions, "rand",  make_builtin(builtin_rand));
  map_set(&definitions, "until", make_builtin(builtin_until));
  map_set(&definitions, "while", make_builtin(builtin_while));

  map_set(&definitions, "n",    make_block("\"\n\""));
  map_set(&definitions, "puts", make_block("print n print"));
  map_set(&definitions, "p",    make_block("`puts"));

  // Initializes random number generator for the rand function
  srand(time(NULL));
}

// Pushes an item to the stack
void stack_push(Item item) {
  array_push(&stack, item);
}

Item stack_pop() {
  if (stack.length == 0) {
    fprintf(stderr, "Error! Cannot pop from empty stack!\n");
    exit(1);
  }
  stack.length--;

  // If the stack's size decreases below what they were when a opening
  // bracket was encountered, we have to move the bracket accordingly
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
      if (c == '\\') {
        c = str->str_data[++(*code_pos)];
        switch (c) {
          case 'a': c = '\a';   break;
          case 'b': c = '\b';   break;
          case 'e': c = '\x1b'; break;
          case 'f': c = '\f';   break;
          case 'n': c = '\n';   break;
          case 'r': c = '\r';   break;
          case 's': c =  ' ';   break;
          case 't': c = '\t';   break;
          case 'v': c = '\v';   break;
        }
      }
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
  else if (c == '#') {
    do {
      string_add_char(&token, c);
      c = str->str_data[++(*code_pos)];
    } while (c != '\n' && *code_pos < str->length);
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
    else if (tok.str_data[0] == ':') {
      if (stack.length == 0) {
        fprintf(stderr, "Unable to define from empty stack!\n");
        exit(1);
      }
      if (code_pos >= str->length) {
        fprintf(stderr, "Error! No token to assign to!\n");
        exit(1);
      }
      Item top_item = make_copy(&stack.items[stack.length - 1]);
      String to_define = next_token(str, &code_pos);
      map_set(&definitions, to_define.str_data, top_item);
      free(to_define.str_data);
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
  else if (item->type == TYPE_BLOCK) {
    execute_string(&item->str_val);
  }
  else {
    stack_push(make_copy(item));
  }
}

void output_final_stack() {
  for (uint32_t i = 0; i < stack.length; i++) {
    output_item(&stack.items[i]);
  }
}
