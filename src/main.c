#include "../include/ast.h"
#include "../include/lexer.h"
#include "parser/parser.bison.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

extern AST *root;
char *input_buff = NULL;
extern int yyparse(void);
extern int yylex(void);
extern int yydebug;

extern void lexer_reset(void);

int parse_file(char *path) {
  FILE *bomb = fopen(path, "rb");
  if (bomb == NULL) {
    fprintf(stderr, "Source Code is not exist: <source.bmb>");
    return EXIT_FAILURE;
  }

  fseek(bomb, 0, SEEK_END);
  long file_size = ftell(bomb);
  rewind(bomb);

  char *buff = (char *)malloc(file_size);

  if (buff == NULL) {
    fprintf(stderr, "Error Aloccating Memory");
    return EXIT_FAILURE;
  }

  size_t bytes_read = fread(buff, 1, file_size, bomb);
  if (bytes_read != (size_t)file_size) {
    fprintf(stderr, "Failed Reading File");

    fclose(bomb);
    free(buff);
    return EXIT_FAILURE;
  }

  printf("%s", buff);
  printf("--- EOF ---\n");
  printf("Bytes read: %zu\n\n", bytes_read);

  printf("starting analysis\n\n");

  input_buff = buff;

  yydebug = 1;
  yyparse();

  printf("\nreaching EOF\n");
  free(buff);

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Invalid Usage, Usage: bomb <source.bmb>");
    return EXIT_FAILURE;
  }

  int result = parse_file(argv[1]);
  if (result != 0) {
    fprintf(stderr, "ERROR PARSING FILE...   %d", result);
    return EXIT_FAILURE;
  }

  ast_print(root, 2);

  return EXIT_SUCCESS;
}
