#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

typedef enum{
  STATE_START,

  STATE_INT_LIT,
  STATE_FLOAT_LIT,
  STATE_KEYWORD,

  STATE_IDEN,
  STATE_OPERATOR,

  STATE_PUNC,

  STATE_EOF,
  STATE_ERROR
}lexerState;

typedef enum {
  TOKEN_EOF,        // EOF (end of the file)
  TOKEN_IDENTIFIER, // (main, a, b , c bassically names)
  TOKEN_INT_LIT,     // (1 ,11 ,4 bassically numbers)
  TOKEN_FLOAT_LIT,
  TOKEN_UNKNOWN,

  TOKEN_KEYWORD_FN,     // fn
  TOKEN_KEYWORD_INT,    // int
  TOKEN_KEYWORD_RETURN, // return

  TOKEN_OPERATOR_PLUS,     // +
  TOKEN_OPERATOR_MINUS,    // -
  TOKEN_OPERATOR_MULTIPLY, // *
  TOKEN_OPERATOR_DIVIDE,   // /
  TOKEN_OPERATOR_ASSIGN,   // =

  TOKEN_PAREN_L,   //(
  TOKEN_PAREN_R,   //)
  TOKEN_BRACKET_L, //{
  TOKEN_BRACKET_R, //}
  TOKEN_COLON,     //:
  TOKEN_SEMICOLON, //;
  TOKEN_COMA       //,
} TokenType;

typedef struct{
  TokenType type;
  char *value;
} Token;

Token *next_token(char *buff);
void free_token(Token *token);

char *token_name(TokenType type);

#endif // !LEXER_H
