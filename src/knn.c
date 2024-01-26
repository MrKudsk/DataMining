#include <stdio.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

int main(void) {
  const char *file_path = "./data/Ag-News/train.csv";
  Nob_String_Builder content = {0};
  if (!nob_read_entire_file(file_path, &content)) {
    return 1;
  }

  nob_log(NOB_INFO, "Size of %s is %zu bytes", file_path, content.count);

  return 0;
}
