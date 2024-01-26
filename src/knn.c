#include <stdio.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

int main(void) {
  const char *file_path = "./data/Ag-News/train.csv";
  Nob_String_Builder sb = {0};
  if (!nob_read_entire_file(file_path, &sb)) {
    return 1;
  }
  Nob_String_View content = nob_sv_from_parts(sb.items, sb.count);
  while (content.count > 0) {
    Nob_String_View line = nob_sv_chop_by_delim(&content, '\n');
    nob_log(NOB_INFO, "|" SV_Fmt "|", SV_Arg(line));
  }
  nob_log(NOB_INFO, "Size of %s is %zu bytes", file_path, sb.count);

  return 0;
}
