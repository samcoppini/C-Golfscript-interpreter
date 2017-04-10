#include <stdio.h>
#include <stdlib.h>
#include "golf.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    error("No golfscript file provided!");
  }

  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    error("Unable to open '%s'!", argv[1]);
  }

  String code = read_file_to_string(file);
  fclose(file);
  init_interpreter();
  execute_string(&code);
  end_interpreter();
  free(code.str_data);

  return 0;
}
