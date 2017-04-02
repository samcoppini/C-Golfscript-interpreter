#include <stdio.h>
#include <stdlib.h>
#include "golf.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Error! No golfscript file provided!\n");
    exit(1);
  }

  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    fprintf(stderr, "Error! Unable to open '%s'!\n", argv[1]);
    exit(1);
  }

  String code = read_file_to_string(file);
  fclose(file);
  init_stack();
  execute_string(&code);
  output_final_stack();
  free(code.str_data);

  return 0;
}
