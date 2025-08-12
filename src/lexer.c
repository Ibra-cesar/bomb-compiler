#include "../include/lexer.h"
#include "../include/ast.h"
#include "parser/parser.bison.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// poss
static const char *current_pos = NULL;
extern char *input_buff;
extern int yylex(void);

void lexer_reset(void){
  current_pos = input_buff;
}

char *token_name(int type) {
  switch (type) {
  case TOKEN_EOF:
    return "TOKEN_EOF";
  case TOKEN_IDENTIFIER:
    return "TOKEN_IDENTIFIER";
  case TOKEN_INT_LIT:
    return "TOKEN_LITERAL_INT";
  case TOKEN_FLOAT_LIT:
    return "TOKEN_LITERAL_FLOAT";
  case TOKEN_KEYWORD_FN:
    return "TOKEN_KEYWORD_FN";
  case TOKEN_KEYWORD_INT:
    return "TOKEN_KEYWORD_INT";
  case TOKEN_KEYWORD_FLOAT:
    return "TOKEN_KEYWORD_FLOAT";
  case TOKEN_KEYWORD_RETURN:
    return "TOKEN_KEYWORD_RETURN";
  case TOKEN_OPERATOR_PLUS:
    return "TOKEN_OPERATOR_PLUS";
  case TOKEN_OPERATOR_MINUS:
    return "TOKEN_OPERATOR_MINUS";
  case TOKEN_OPERATOR_MULTIPLY:
    return "TOKEN_OPERATOR_MULTIPLY";
  case TOKEN_OPERATOR_DIVIDE:
    return "TOKEN_OPERATOR_DIVIDE";
  case TOKEN_OPERATOR_ASSIGN:
    return "TOKEN_OPERATOR_ASSIGN";
  case TOKEN_PAREN_L:
    return "TOKEN_PAREN_L";
  case TOKEN_PAREN_R:
    return "TOKEN_PAREN_R";
  case TOKEN_BRACKET_L:
    return "TOKEN_BRACE_L";
  case TOKEN_BRACKET_R:
    return "TOKEN_BRACE_R";
  case TOKEN_COLON:
    return "TOKEN_COLON";
  case TOKEN_SEMICOLON:
    return "TOKEN_SEMICOLON";
  case TOKEN_COMMA:
    return "TOKEN_COMA";
  default:
    return "UNKNOWN_TOKEN";
  }
}

static Token *create_token(int type, const char *val_start, size_t token_len) {
  Token *token = (Token *)malloc(sizeof(Token));
  if (token == NULL) {
    fprintf(stderr, "Lexer: Failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  token->type = type;

  token->value = (char *)malloc(token_len + 1);
  if (token->value == NULL) {
    fprintf(stderr, "Lexer: Failed to allocate memory fot token value");
    free(token);
    exit(EXIT_FAILURE);
  }

  strncpy(token->value, val_start, token_len);
  token->value[token_len] = '\0';

  return token;
}

void free_token(Token *token) {
  if (token) {
    free(token->value);
    free(token);
  }
}

static int check_keyword(const char *identifier) {
  if (strcmp(identifier, "fn") == 0)
    return TOKEN_KEYWORD_FN;
  if (strcmp(identifier, "int") == 0)
    return TOKEN_KEYWORD_INT;
  if (strcmp(identifier, "float") == 0)
    return TOKEN_KEYWORD_FLOAT;
  if (strcmp(identifier, "return") == 0)
    return TOKEN_KEYWORD_RETURN;
  if (strcmp(identifier, "main") == 0)
    return TOKEN_IDENTIFIER; // 'main' is a function identifier, not a keyword
  return TOKEN_IDENTIFIER;
}

int yylex(void) {
  if (current_pos == NULL) {
    current_pos = input_buff;
  }
  // Check for end of file
  if (*current_pos == '\0') {
   return TOKEN_EOF;
  }

  // Skip any whitespace
  while (isspace(*current_pos)) {
    current_pos++;
  }
  // Get the start of the token for later

  const char *token_start = current_pos;
  lexerState state = STATE_START;

  while (*current_pos != '\0') {
    char curr_char = *current_pos;

    switch (state) {
    case STATE_START:
      if (isalpha(curr_char) || curr_char == '_') {
        state = STATE_IDEN;
        current_pos++;
      } else if (isdigit(curr_char)) {
        state = STATE_INT_LIT;
        current_pos++;
      } else {
        current_pos++;
        switch (curr_char) {
        case '(':
          return TOKEN_PAREN_L;
        case ')':
          return TOKEN_PAREN_R;
        case '{':
          return TOKEN_BRACKET_L;
        case '}':
          return TOKEN_BRACKET_R;
        case ':':
          return TOKEN_COLON;
        case ';':
          return TOKEN_SEMICOLON;
        case ',':
          return TOKEN_COMMA;
        case '=':
          return TOKEN_OPERATOR_ASSIGN;
        case '+':
          yylval.bin_op = BINOP_ADD;
          return TOKEN_OPERATOR_PLUS;
        case '-':
          yylval.bin_op = BINOP_SUB;
          return TOKEN_OPERATOR_MINUS;
        case '*':
          yylval.bin_op = BINOP_MUL;
          return TOKEN_OPERATOR_MULTIPLY;
        case '/':
          yylval.bin_op = BINOP_DIV;
          return TOKEN_OPERATOR_DIVIDE;
        default:
          fprintf(stderr, "Lexer: Unrecognized character '%c'\n", curr_char);
          exit(EXIT_FAILURE);
        }
      }
      break;

    case STATE_IDEN:
      if (isalnum(curr_char) || curr_char == '_') {
        current_pos++;
      } else {
        size_t len = current_pos - token_start;
        char *identifier_str = strndup(token_start, len);
        int type = check_keyword(identifier_str);
        if (type == TOKEN_IDENTIFIER) {
          yylval.str = identifier_str;
        } else {
          free(identifier_str);
        }
        return type;
      }
      break;

    case STATE_INT_LIT:
      if (isdigit(curr_char)) {
        current_pos++;
      } else if (curr_char == '.') {
        state = STATE_FLOAT_LIT;
        current_pos++;
      } else {
        size_t len = current_pos - token_start;
        char *num = strndup(token_start, len);
        yylval.str = num;
        return TOKEN_INT_LIT;
      }
      break;
    case STATE_FLOAT_LIT:
      if (isdigit(curr_char)) {
        current_pos++;
      } else {
        size_t len = current_pos - token_start;
        char *dec = strndup(token_start, len);
        yylval.str = dec;
        return TOKEN_FLOAT_LIT;
      }
      break;
    default:
      fprintf(stderr, "Lexer: Invalid state\n");
      exit(EXIT_FAILURE);
    }
  }

  if (state == STATE_IDEN) {
    size_t len = current_pos - token_start;
    char *identifier_str = strndup(token_start, len);
    int type = check_keyword(identifier_str);
    if (type == TOKEN_IDENTIFIER) {
      yylval.str = identifier_str;
    } else {
      free(identifier_str);
    }
    return type;
  } else if (state == STATE_INT_LIT) {
    size_t len = current_pos - token_start;
    char *num = strndup(token_start, len);
    yylval.str = num;
    return TOKEN_INT_LIT;
  } else if (state == STATE_FLOAT_LIT) {
    size_t len = current_pos - token_start;
    char *dec = strndup(token_start, len);
    yylval.str = dec;
    return TOKEN_FLOAT_LIT;
  }

  return TOKEN_EOF;
}



/*THIS FUNCTION IS FOR DEBUGGING PORPOSES*/
Token *next_token(char *buffer) {
  if (buffer != NULL) {
    current_pos = buffer; // Initialize the buffer pointer on the first call
  }
  // Check for end of file
  if (*current_pos == '\0') {
    return create_token(TOKEN_EOF, "", 0);
  }
  // Skip any whitespace
  while (isspace(*current_pos)) {
    current_pos++;
  }
  // Get the start of the token for later

  const char *token_start = current_pos;
  lexerState state = STATE_START;

  while (*current_pos != '\0') {
    char curr_char = *current_pos;

    switch (state) {
    case STATE_START:
      if (isalpha(curr_char) || curr_char == '_') {
        state = STATE_IDEN;
        current_pos++;
      } else if (isdigit(curr_char)) {
        state = STATE_INT_LIT;
        current_pos++;
      } else {
        current_pos++;
        switch (curr_char) {
        case '(':
          return create_token(TOKEN_PAREN_L, token_start, 1);
        case ')':
          return create_token(TOKEN_PAREN_R, token_start, 1);
        case '{':
          return create_token(TOKEN_BRACKET_L, token_start, 1);
        case '}':
          return create_token(TOKEN_BRACKET_R, token_start, 1);
        case ':':
          return create_token(TOKEN_COLON, token_start, 1);
        case ';':
          return create_token(TOKEN_SEMICOLON, token_start, 1);
        case ',':
          return create_token(TOKEN_COMMA, token_start, 1);
        case '=':
          return create_token(TOKEN_OPERATOR_ASSIGN, token_start, 1);
        case '+':
          return create_token(TOKEN_OPERATOR_PLUS, token_start, 1);
        case '-':
          return create_token(TOKEN_OPERATOR_MINUS, token_start, 1);
        case '*':
          return create_token(TOKEN_OPERATOR_MULTIPLY, token_start, 1);
        case '/':
          return create_token(TOKEN_OPERATOR_DIVIDE, token_start, 1);
        default:
          fprintf(stderr, "Lexer: Unrecognized character '%c'\n", curr_char);
          return create_token(TOKEN_EOF, token_start, 1);
        }
      }
      break;

    case STATE_IDEN:
      if (isalnum(curr_char) || curr_char == '_') {
        current_pos++;
      } else {
        size_t len = current_pos - token_start;
        char *identifier_str = strndup(token_start, len);
        int type = check_keyword(identifier_str);
        free(identifier_str);
        return create_token(type, token_start, len);
      }
      break;

    case STATE_INT_LIT:
      if (isdigit(curr_char)) {
        current_pos++;
      } else if (curr_char == '.') {
        state = STATE_FLOAT_LIT;
        current_pos++;
      } else {
        size_t len = current_pos - token_start;
        return create_token(TOKEN_INT_LIT, token_start, len);
      }
      break;
    case STATE_FLOAT_LIT:
      if (isdigit(curr_char)) {
        current_pos++;
      } else {
        size_t len = current_pos - token_start;
        return create_token(TOKEN_FLOAT_LIT, token_start, len);
      }
      break;
    default:
      fprintf(stderr, "Lexer: Invalid state\n");
      exit(EXIT_FAILURE);
    }
  }

  if (state == STATE_IDEN) {
    size_t len = current_pos - token_start;
    char *identifier_str = strndup(token_start, len);
    int type = check_keyword(identifier_str);
    free(identifier_str);
    return create_token(type, token_start, len);
  } else if (state == STATE_INT_LIT) {
    size_t len = current_pos - token_start;
    return create_token(TOKEN_INT_LIT, token_start, len);
  }else if(state == STATE_FLOAT_LIT){
    size_t len = current_pos - token_start;
    return  create_token(TOKEN_FLOAT_LIT, token_start, len);
  }

  return create_token(TOKEN_EOF, "", 0);
}
