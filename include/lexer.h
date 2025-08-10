#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

typedef enum{
  STATE_START,

  STATE_INT_LIT,
  STATE_FLOAT_LIT,

  STATE_IDEN,
  STATE_OPERATOR,

  STATE_PUNC,

  STATE_EOF,
  STATE_ERROR
}lexerState;

typedef struct{
  int type;
  char *value;
} Token;

Token *next_token(char *buffer);
void free_token(Token *token);

char *token_name(int type);

#endif // !LEXER_H

