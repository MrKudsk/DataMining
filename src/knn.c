#include <stdio.h>
#include <zlib.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

Nob_String_View deflate_sv(Nob_String_View sv) {
  void *output = nob_temp_alloc(sv.count * 2);

  z_stream defstream = {0};
  defstream.avail_in =
      (uInt)sv.count; // strle(a)+1; // size of inpunt + terminator
  defstream.next_in = (Bytef *)sv.data; // input char array
  defstream.avail_out = (uInt)sv.count * 2;
  defstream.next_out = (Bytef *)output;

  deflateInit(&defstream, Z_BEST_COMPRESSION);
  deflate(&defstream, Z_FINISH);
  deflateEnd(&defstream);

  return nob_sv_from_parts(output, defstream.total_out);
}

int main(int argc, char **argv) {
  const char *program = nob_shift_args(&argc, &argv);

  if (argc <= 0) {
    nob_log(NOB_ERROR, "Usage: %s <input.csv>", program);
    nob_log(NOB_ERROR, "no input is provided");
    return 1;
  }

  const char *file_path = nob_shift_args(&argc, &argv);
  Nob_String_Builder sb = {0};
  if (!nob_read_entire_file(file_path, &sb)) {
    return 1;
  }
  Nob_String_View content = nob_sv_from_parts(sb.items, sb.count);
  size_t lines_count = 0;
  for (; content.count > 0; ++lines_count) {
    Nob_String_View line = nob_sv_chop_by_delim(&content, '\n');

    if (lines_count == 0)
      continue; // Skip 1 line (header)
    Nob_String_View klass = nob_sv_chop_by_delim(&line, ',');
    size_t klass_index = *klass.data - '0' - 1;

    Nob_String_View compressed_line = deflate_sv(line);
    /* nob_log(NOB_INFO,
            "%zu: klass = %zu, uncompresssed len = %zu, compressed len = %zu, "
            "compression ratio = %f ",
            lines_count, klass_index, line.count, compressed_line.count,
            1 - (float)compressed_line.count / line.count);
*/
    nob_temp_reset();
  }
  /*  nob_log(NOB_INFO, "Size of %s is %zu kilobytes", file_path, sb.count /
    1000); nob_log(NOB_INFO, "%s contains %zu lines", file_path, lines_count);
    for (size_t i = 0; i < NOB_ARRAY_LEN(klasses); ++i) {
      nob_log(NOB_INFO, "   %zu => %zu", i + 1, klasses[i]);
    }
    */
  return 0;
}
