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
    stack_push(empty_string());
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
  map_set(&definitions, create_string("&"), make_builtin(builtin_ampersand));
  map_set(&definitions, create_string("*"), make_builtin(builtin_asterisk));
  map_set(&definitions, create_string("@"), make_builtin(builtin_at));
  map_set(&definitions, create_string("\\"), make_builtin(builtin_backslash));
  map_set(&definitions, create_string("`"), make_builtin(builtin_backtick));
  map_set(&definitions, create_string("|"), make_builtin(builtin_bar));
  map_set(&definitions, create_string("^"), make_builtin(builtin_caret));
  map_set(&definitions, create_string(","), make_builtin(builtin_comma));
  map_set(&definitions, create_string("$"), make_builtin(builtin_dollar_sign));
  map_set(&definitions, create_string("="), make_builtin(builtin_equal));
  map_set(&definitions, create_string("!"), make_builtin(builtin_exclamation));
  map_set(&definitions, create_string(">"), make_builtin(builtin_greater_than));
  map_set(&definitions, create_string("["), make_builtin(builtin_lbracket));
  map_set(&definitions, create_string("<"), make_builtin(builtin_less_than));
  map_set(&definitions, create_string("("), make_builtin(builtin_lparen));
  map_set(&definitions, create_string("-"), make_builtin(builtin_minus));
  map_set(&definitions, create_string("%"), make_builtin(builtin_percent));
  map_set(&definitions, create_string("."), make_builtin(builtin_period));
  map_set(&definitions, create_string("+"), make_builtin(builtin_plus));
  map_set(&definitions, create_string("?"), make_builtin(builtin_question));
  map_set(&definitions, create_string("]"), make_builtin(builtin_rbracket));
  map_set(&definitions, create_string(")"), make_builtin(builtin_rparen));
  map_set(&definitions, create_string(";"), make_builtin(builtin_semicolon));
  map_set(&definitions, create_string("/"), make_builtin(builtin_slash));
  map_set(&definitions, create_string("~"), make_builtin(builtin_tilde));
  map_set(&definitions, create_string("abs"), make_builtin(builtin_abs));
  map_set(&definitions, create_string("base"), make_builtin(builtin_base));
  map_set(&definitions, create_string("do"), make_builtin(builtin_do));
  map_set(&definitions, create_string("if"), make_builtin(builtin_if));
  map_set(&definitions, create_string("print"), make_builtin(builtin_print));
  map_set(&definitions, create_string("rand"), make_builtin(builtin_rand));
  map_set(&definitions, create_string("until"), make_builtin(builtin_until));
  map_set(&definitions, create_string("while"), make_builtin(builtin_while));
  map_set(&definitions, create_string("zip"), make_builtin(builtin_zip));

  map_set(&definitions, create_string("n"),
          make_block(create_string("\"\n\"")));

  map_set(&definitions, create_string("puts"),
          make_block(create_string("print n print")));

  map_set(&definitions, create_string("p"),
          make_block(create_string("`puts")));

  map_set(&definitions, create_string("and"),
          make_block(create_string("1$if")));

  map_set(&definitions, create_string("or"),
          make_block(create_string("1$\\if")));

  map_set(&definitions, create_string("xor"),
          make_block(create_string("\\!!{!}*")));

  // Initializes random number generator for the rand function
  init_rng();
}

void end_interpreter() {
  for (uint32_t i = 0; i < stack.length; i++) {
    output_item(&stack.items[i]);
    free_item(&stack.items[i]);
  }
  free_map(&definitions);
}

// Pushes an item to the stack
void stack_push(Item item) {
  array_push(&stack, item);
}

Item stack_pop() {
  if (stack.length == 0) {
    error("Cannot pop from empty stack!");
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

int hex_digit_val(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  else if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  else
    return -1;
}

String get_number(String *str, String *cur_tok, uint32_t *code_pos) {
  while (++(*code_pos) < str->length && isdigit(str->str_data[*code_pos])) {
    string_add_char(cur_tok, str->str_data[*code_pos]);
  }
  return *cur_tok;
}

String get_raw_string(String *str, String *cur_tok, uint32_t *code_pos) {
  while (++(*code_pos) < str->length) {
    char c = str->str_data[*code_pos];
    if (c == '\\' && *code_pos + 1 < str->length) {
      if (str->str_data[*code_pos + 1] == '\\' ||
          str->str_data[*code_pos + 1] == '\'')
      {
        c = str->str_data[++(*code_pos)];
      }
    }
    else if (c == '\'') {
      ++(*code_pos);
      return *cur_tok;
    }
    string_add_char(cur_tok, c);
  }

  error("Unmatched ' encountered in the code!");
}

String get_escaped_string(String *str, String *cur_tok, uint32_t *code_pos) {
  while (++(*code_pos) < str->length) {
    char c = str->str_data[*code_pos];
    if (c == '"') {
      ++*code_pos;
      return *cur_tok;
    }
    else if (c == '\\') {
      if (++(*code_pos) >= str->length) {
        break;
      }
      else {
        c = str->str_data[*code_pos];
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
          case 'x':
            if (++(*code_pos) >= str->length)
              error("Unmatched \" encountered in the code!");
            if (hex_digit_val(str->str_data[*code_pos]) < 0)
              error("Invalid hex literal!");
            c = hex_digit_val(str->str_data[*code_pos]);
            if (*code_pos + 1 < str->length &&
                hex_digit_val(str->str_data[*code_pos + 1]) >= 0)
            {
              c <<= 4;
              c += hex_digit_val(str->str_data[++(*code_pos)]);
            }
            break;
        }
      }
    }
    string_add_char(cur_tok, c);
  }

  error("Unmatched \" encountered in the code!");
}

String get_identifier(String *str, String *cur_tok, uint32_t *code_pos) {
  while (++(*code_pos) < str->length) {
    char c = str->str_data[*code_pos];
    if (isalnum(c) || c == '_')
      string_add_char(cur_tok, c);
    else
      break;
  }
  return *cur_tok;
}

String get_block(String *str, String *cur_tok, uint32_t *code_pos) {
  int brace_level = 1;
  while (++(*code_pos) < str->length && brace_level > 0) {
    char c = str->str_data[*code_pos];
    if (c == '{')
      brace_level++;
    else if (c == '}')
      brace_level--;
    if (brace_level > 0)
      string_add_char(cur_tok, str->str_data[*code_pos]);
  }

  if (*code_pos > str->length)
    error("Unmatched { encountered in the code!");

  return *cur_tok;
}

String get_comment(String *str, String *cur_tok, uint32_t *code_pos) {
  while (++(*code_pos) < str->length && str->str_data[*code_pos] != '\n') {
    string_add_char(cur_tok, str->str_data[*code_pos]);
  }
  return *cur_tok;
}

// Return the next token in the string from the given code position
String next_token(String *str, uint32_t *code_pos) {
  String token = new_string();

  if (*code_pos >= str->length)
    return token;

  char c = str->str_data[*code_pos];
  if (c == '"') {
    string_add_char(&token, c);
    return get_escaped_string(str, &token, code_pos);
  }
  else if (c == '\'') {
    string_add_char(&token, c);
    return get_raw_string(str, &token, code_pos);
  }
  else if (isdigit(c) || c == '-') {
    string_add_char(&token, c);
    return get_number(str, &token, code_pos);
  }
  else if (isalpha(c) || c == '_') {
    string_add_char(&token, c);
    return get_identifier(str, &token, code_pos);
  }
  else if (c == '{') {
    string_add_char(&token, c);
    return get_block(str, &token, code_pos);
  }
  else if (c == '#') {
    string_add_char(&token, c);
    return get_comment(str, &token, code_pos);
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
    Item *defined_item = map_get(&definitions, &tok);
    if (defined_item != NULL) {
      execute_item(defined_item);
    }
    else if (tok.str_data[0] == ':') {
      if (stack.length == 0) {
        error("Unable to define from empty stack!");
      }
      if (code_pos >= str->length) {
        error("No token to assign to!");
      }
      Item top_item = make_copy(&stack.items[stack.length - 1]);
      String to_define = next_token(str, &code_pos);
      map_set(&definitions, to_define, top_item);
    }
    else if (isdigit(tok.str_data[0]) ||
             (tok.str_data[0] == '-' && tok.length > 1))
    {
      stack_push(make_integer(string_to_int(&tok)));
    }
    else if (tok.str_data[0] == '"' || tok.str_data[0] == '\'') {
      string_remove_from_front(&tok, 1);
      stack_push(make_string(&tok));
    }
    else if (tok.str_data[0] == '{') {
      string_remove_from_front(&tok, 1);
      stack_push(make_block(copy_string(&tok)));
    }
    free(tok.str_data);
  }
}

void repeat_block(Item *block, int64_t times) {
  while (times-- > 0) {
    execute_string(&block->str_val);
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
