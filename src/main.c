#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/raylib.h"
#include "../lib/raymath.h"

#define NOB_IMPLEMENTATION
#include "nob.h"

#define SAMPLE_RADIUS 4.0f
#define MEAN_RADIUS (SAMPLE_RADIUS * 2)
#define K 3

#define MYBACKGROUNDCOLOR 0x181818AA

static Vector2 project_sample_to_screen(Vector2 sample, float min_x,
                                        float max_x, float min_y, float max_y) {
  // -20..20 => 0..40 => 0..1
  float w = GetScreenWidth();
  float h = GetScreenHeight();
  return CLITERAL(Vector2){
      .x = (sample.x - min_x) / (max_x - min_x) * w,
      .y = h - (sample.y - min_y) / (max_y - min_y) * h,
  };
}

typedef struct {
  Vector2 *items;
  size_t count;
  size_t capacity;
} Samples;

static float rand_float(void) { return (float)rand() / RAND_MAX; }

static void generate_cluster(Vector2 center, float radius, size_t count,
                             Samples *samples) {
  for (size_t i = 0; i < count; ++i) {
    float angle = rand_float() * 2 * PI;
    float mag = rand_float();
    Vector2 sample = {
        .x = center.x + cosf(angle) * mag * radius,
        .y = center.y + sinf(angle) * mag * radius,
    };
    nob_da_append(samples, sample);
  }
}

static Color colors[] = {
    GOLD, PINK, MAROON, LIME, SKYBLUE, PURPLE, VIOLET,
};

#define colors_count NOB_ARRAY_LEN(colors)

static Samples set = {0};
static Samples clusters[K] = {0};
static Vector2 means[K] = {0};

void generate_new_state(float min_x, float max_x, float min_y, float max_y) {
  // Generate the Samples data
#if 0
  set.count = 0;
  generate_cluster(CLITERAL(Vector2){0}, 10, 100, &set);
  generate_cluster(CLITERAL(Vector2){min_x * 0.5f, max_y * 0.5f}, 5, 50, &set);
  generate_cluster(CLITERAL(Vector2){max_x * 0.5f, max_y * 0.5f}, 5, 50, &set);
#endif

  // Calculation of the K-means
  for (size_t i = 0; i < K; ++i) {
    means[i].x = Lerp(min_x, max_x, rand_float());
    means[i].y = Lerp(min_y, max_y, rand_float());
  }
}

void recluster_state(void) {
  for (size_t j = 0; j < K; ++j) {
    clusters[j].count = 0;
  }
  for (size_t i = 0; i < set.count; ++i) {
    Vector2 p = set.items[i];
    int k = -1;
    float s = FLT_MAX;
    for (size_t j = 0; j < K; ++j) {
      Vector2 m = means[j];
      float sm = Vector2LengthSqr(Vector2Subtract(p, m));
      if (sm < s) {
        s = sm;
        k = j;
      }
    }
    nob_da_append(&clusters[k], p);
  }
}

void update_means(float min_x, float max_x, float min_y, float max_y) {
  for (size_t i = 0; i < K; ++i) {
    if (clusters[i].count > 0) {
      means[i] = Vector2Zero();
      for (size_t j = 0; j < clusters[i].count; ++j) {
        means[i] = Vector2Add(means[i], clusters[i].items[j]);
      }
      means[i].x /= clusters[i].count;
      means[i].y /= clusters[i].count;
    } else {
      means[i].x = Lerp(min_x, max_x, rand_float());
      means[i].y = Lerp(min_y, max_y, rand_float());
    }
  }
}

typedef enum {
  LEAF_CLASS = 0,
  LEAF_SPECIMEN_NUMBER,
  LEAF_ECCENTRICITY,
  LEAF_ASPECT_RATIO,
  LEAF_ELONGATION,
  LEAF_SOLIDITY,
  LEAF_STOCHASTIC_CONVEXITY,
  LEAF_ISOPERIMETRIC_FACTOR,
  LEAF_MAXIMAL_INDENTATION_DEPTH,
  LEAF_LOBEDNESS,
  LEAF_AVERAGE_INTENSITY,
  LEAF_AVERAGE_CONTRAST,
  LEAF_SMOOTHNESS,
  LEAF_THIRD_MOMENT,
  LEAF_UNIFORMITY,
  LEAF_ENTROPY,
} Leaf_Attr;

int main(void) {
  const char *leaf_path = "data/leaf.csv";
  Nob_String_Builder sb = {0};
  if (!nob_read_entire_file(leaf_path, &sb))
    return 1;

  float min_x = FLT_MAX;
  float max_x = FLT_MIN;
  float min_y = FLT_MAX;
  float max_y = FLT_MIN;

  Nob_String_View content = nob_sv_from_parts(sb.items, sb.count);
  while (content.count > 0) {
    Nob_String_View line = nob_sv_chop_by_delim(&content, '\n');
    Vector2 p = {0};
    for (size_t i = 0; line.count > 0; ++i) {
      Nob_String_View attr = nob_sv_chop_by_delim(&line, ',');
      float value = strtof(nob_temp_sprintf(SV_Fmt, SV_Arg(attr)), NULL);
      switch (i) {
      case LEAF_CLASS:
        p.x = value;
        break;
      case LEAF_ENTROPY:
        p.y = value;
        break;
      default: {
      }
      }
    }
    nob_da_append(&set, p);
    if (p.x < min_x)
      min_x = p.x;
    if (p.x > max_x)
      max_x = p.x;
    if (p.y < min_y)
      min_y = p.y;
    if (p.y > max_y)
      max_y = p.y;
  }
  nob_temp_reset();
  nob_log(NOB_INFO, "x = %f .. %f)", min_x, max_x);
  nob_log(NOB_INFO, "y = %f .. %f)", min_y, max_y);

  srand(time(0));
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "K-means");

  generate_new_state(min_x, max_x, min_y, max_y);
  recluster_state();

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_R)) {
      generate_new_state(min_x, max_x, min_y, max_y);
      recluster_state();
    }
    if (IsKeyPressed(KEY_SPACE)) {
      update_means(min_x, max_x, min_y, max_y);
      recluster_state();
    }
    BeginDrawing();
    ClearBackground(GetColor(MYBACKGROUNDCOLOR));

    for (size_t i = 0; i < K; ++i) {
      Color color = colors[i % colors_count];

      for (size_t j = 0; j < clusters[i].count; ++j) {
        Vector2 it = clusters[i].items[j];
        DrawCircleV(project_sample_to_screen(it, min_x, max_x, min_y, max_y),
                    SAMPLE_RADIUS, color);
      }

      Vector2 it = means[i];
      DrawCircleV(project_sample_to_screen(it, min_x, max_x, min_y, max_y),
                  MEAN_RADIUS, color);
    }
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
