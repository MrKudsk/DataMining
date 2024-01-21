#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/raylib.h"

#define NOB_IMPLEMENTATION
#include "nob.h"

#define SAMPLE_RADIUS 4.0f
#define MIN_X -20.0
#define MAX_X 20.0
#define MIN_Y -20.0
#define MAX_Y 20.0

#define MYBACKGROUNDCOLOR 0x181818AA

Vector2 project_sample_to_screen(Vector2 sample) {
  // -20..20 => 0..40 => 0..1
  float w = GetScreenWidth();
  float h = GetScreenHeight();
  return CLITERAL(Vector2){
      .x = (sample.x - MIN_X) / (MAX_X - MIN_X) * w,
      .y = h - (sample.y - MIN_Y) / (MAX_Y - MIN_Y) * h,
  };
}

typedef struct {
  Vector2 *items;
  size_t count;
  size_t capacity;
} Samples;

float rand_float(void) { return (float)rand() / RAND_MAX; }

void generate_cluster(Vector2 center, float radius, size_t count,
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

int main(void) {
  srand(time(0));
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "k-means");
  Samples set = {0};
  generate_cluster(CLITERAL(Vector2){0}, 10, 100, &set);
  generate_cluster(CLITERAL(Vector2){MIN_X * 0.5f, MAX_Y * 0.5f}, 5, 50, &set);
  generate_cluster(CLITERAL(Vector2){MAX_X * 0.5f, MAX_Y * 0.5f}, 5, 50, &set);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_R)) {
      set.count = 0;
      generate_cluster(CLITERAL(Vector2){0}, 10, 100, &set);
      generate_cluster(CLITERAL(Vector2){MIN_X * 0.5f, MAX_Y * 0.5f}, 5, 50,
                       &set);
      generate_cluster(CLITERAL(Vector2){MAX_X * 0.5f, MAX_Y * 0.5f}, 5, 50,
                       &set);
    }
    BeginDrawing();
    ClearBackground(GetColor(MYBACKGROUNDCOLOR));
    for (size_t i = 0; i < set.count; ++i) {
      Vector2 it = set.items[i];
      DrawCircleV(project_sample_to_screen(it), SAMPLE_RADIUS, RED);
    }
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
