#include "../include/lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// poss
static const char *current_pos = NULL;

char *token_name(TokenType type) {
  switch (type) {
  case TOKEN_EOF:
    return "TOKEN_EOF";
  case TOKEN_IDENTIFIER:
    return "TOKEN_IDENTIFIER";
  case TOKEN_INT_LIT:
    return "TOKEN_LITERAL_INT";
  case TOKEN_KEYWORD_FN:
    return "TOKEN_KEYWORD_FN";
  case TOKEN_KEYWORD_INT:
    return "TOKEN_KEYWORD_INT";
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
  case TOKEN_COMA:
    return "TOKEN_COMA";
  default:
    return "UNKNOWN_TOKEN";
  }
}

static Token *create_token(TokenType type, const char *val_start,
                           size_t token_len) {
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

static TokenType check_keyword(const char *identifier) {
  if (strcmp(identifier, "fn") == 0)
    return TOKEN_KEYWORD_FN;
  if (strcmp(identifier, "int") == 0)
    return TOKEN_KEYWORD_INT;
  if (strcmp(identifier, "return") == 0)
    return TOKEN_KEYWORD_RETURN;
  if (strcmp(identifier, "main") == 0)
    return TOKEN_IDENTIFIER; // 'main' is a function identifier, not a keyword
  return TOKEN_IDENTIFIER;
}

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
  if (*current_pos == '\0') {
    return create_token(TOKEN_EOF, "", 0);
  }

  // Get the start of the token for later
  const char *token_start = current_pos;

  // Handle single-character tokens with a switch statement
  switch (*current_pos) {
  case '(':
    current_pos++;
    return create_token(TOKEN_PAREN_L, token_start, 1);
  case ')':
    current_pos++;
    return create_token(TOKEN_PAREN_R, token_start, 1);
  case '{':
    current_pos++;
    return create_token(TOKEN_BRACKET_L, token_start, 1);
  case '}':
    current_pos++;
    return create_token(TOKEN_BRACKET_R, token_start, 1);
  case ':':
    current_pos++;
    return create_token(TOKEN_COLON, token_start, 1);
  case ';':
    current_pos++;
    return create_token(TOKEN_SEMICOLON, token_start, 1);
  case ',':
    current_pos++;
    return create_token(TOKEN_COMA, token_start, 1);
  case '=':
    current_pos++;
    return create_token(TOKEN_OPERATOR_ASSIGN, token_start, 1);
  case '+':
    current_pos++;
    return create_token(TOKEN_OPERATOR_PLUS, token_start, 1);
  case '-':
    current_pos++;
    return create_token(TOKEN_OPERATOR_MINUS, token_start, 1);
  case '*':
    current_pos++;
    return create_token(TOKEN_OPERATOR_MULTIPLY, token_start, 1);
  case '/':
    current_pos++;
    return create_token(TOKEN_OPERATOR_DIVIDE, token_start, 1);
  }

  // Handle identifiers and keywords
  if (isalpha(*current_pos) || *current_pos == '_') {
    while (isalnum(*current_pos) || *current_pos == '_') {
      current_pos++;
    }
    size_t length = current_pos - token_start;
    char *identifier_str = strndup(token_start, length);
    TokenType type = check_keyword(identifier_str);
    Token *token = create_token(type, token_start, length);
    free(identifier_str); // Free the temporary string
    return token;
  }

  // Handle integer literals
  if (isdigit(*current_pos)) {
    while (isdigit(*current_pos)) {
      current_pos++;
    }
    size_t length = current_pos - token_start;
    return create_token(TOKEN_INT_LIT, token_start, length);
  }

  // Unrecognized character
  fprintf(stderr, "Lexer: Unrecognized character '%c'\n", *current_pos);
  current_pos++;
  return next_token(NULL); // Continue to the next character
}
