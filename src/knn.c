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

typedef struct {
  size_t klass;
  Nob_String_View text;
} Sample;

typedef struct {
  Sample *items;
  size_t count;
  size_t capacity;
} Samples;

Samples parse_samples(Nob_String_View content) {
  size_t lines_count = 0;
  Samples samples = {0};
  for (; content.count > 0; ++lines_count) {
    Nob_String_View line = nob_sv_chop_by_delim(&content, '\n');

    if (lines_count == 0)
      continue; // Skip 1 line (header)
    Nob_String_View klass = nob_sv_chop_by_delim(&line, ',');
    size_t klass_index = *klass.data - '0' - 1;

    nob_da_append(&samples, ((Sample){
                                .klass = klass_index,
                                .text = line,
                            }));
  }
  return samples;
}

const char *klass_names[] = {"World", "Sports", "Business", "Sci/Tech"};

typedef struct {
  float distance;
  size_t klass;
} NCD;

typedef struct {
  NCD *items;
  size_t count;
  size_t capacity;
} NCDs;

float ncd(Nob_String_View a, Nob_String_View b) {
  Nob_String_View ab = nob_sv_from_cstr(
      nob_temp_sprintf(SV_Fmt " " SV_Fmt, SV_Arg(a), SV_Arg(b)));
  float ca = deflate_sv(a).count;
  float cb = deflate_sv(b).count;
  float cab = deflate_sv(ab).count;

  float mn = ca;
  if (mn > cb)
    mn = cb;
  float mx = ca;
  if (mx < cb)
    mx = cb;
  return (cab - mn) / mx;
}

size_t klassify_sample(Samples train, Nob_String_View text) {
  NCDs ncds = {0};

  for (size_t i = 0; i < train.count; ++i) {
    float distance = ncd(train.items[i].text, text);
    nob_temp_reset();
    nob_da_append(&ncds, ((NCD){
                             .distance = distance,
                             .klass = train.items[i].klass,
                         }));
  }

  nob_log(NOB_INFO, "Top cinco NCDs");
  for (size_t i = 0; i < 5 && i < ncds.count; ++i) {
    nob_log(NOB_INFO, "klass = %zu, distance = %f", ncds.items[i].klass,
            ncds.items[i].distance);
  }
  return 0;
}

int main(int argc, char **argv) {
  const char *program = nob_shift_args(&argc, &argv);

  if (argc <= 0) {
    nob_log(NOB_ERROR, "Usage: %s <train.csv> <test.csv>", program);
    nob_log(NOB_ERROR, "no train is provided");
    return 1;
  }

  const char *train_path = nob_shift_args(&argc, &argv);
  Nob_String_Builder train_content = {0};
  if (!nob_read_entire_file(train_path, &train_content))
    return 1;
  Samples train_samples = parse_samples(
      nob_sv_from_parts(train_content.items, train_content.count));

  if (argc <= 0) {
    nob_log(NOB_ERROR, "Usage: %s <train.csv> <test.csv>", program);
    nob_log(NOB_ERROR, "no test is provided");
    return 1;
  }

  const char *test_path = nob_shift_args(&argc, &argv);
  Nob_String_Builder test_content = {0};
  if (!nob_read_entire_file(test_path, &test_content))
    return 1;
  Samples test_samples =
      parse_samples(nob_sv_from_parts(test_content.items, test_content.count));

  Sample sample = test_samples.items[0];
  size_t predicted_class = klassify_sample(train_samples, sample.text);

  nob_log(NOB_INFO, "Text: " SV_Fmt, SV_Arg(sample.text));
  nob_log(NOB_INFO, "Predicted klass: %s", klass_names[predicted_class]);
  nob_log(NOB_INFO, "Actual klass: %s", klass_names[sample.klass]);

  return 0;
}
