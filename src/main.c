#include "../include/lexer.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  Token *token;

  if (argc != 2) {
    fprintf(stderr, "Invalid Usage, Usage: bomb <source.bmb>");
    return EXIT_FAILURE;
  }
  FILE *bomb = fopen(argv[1], "rb");
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

  size_t read_bytes = fread(buff, 1, file_size, bomb);
  if (read_bytes != (size_t)file_size) {
    fprintf(stderr, "Failed Reading File");

    fclose(bomb);
    free(buff);
    return EXIT_FAILURE;
  }

  printf("%s", buff);
  printf("--- EOF ---\n");
  printf("Bytes read: %zu\n\n", read_bytes);

  printf("starting analysis\n\n");
  token = next_token(buff);

  while (token->type != TOKEN_EOF) {
    printf("Token: type=%s, value='%s'\n", token_name(token->type), token->value);

    free_token(token);
    token = next_token(NULL);
  }

  fclose(bomb);
  free(buff);

  return EXIT_SUCCESS;
}
