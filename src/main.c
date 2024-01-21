#include "../lib/raylib.h"
#include <stdio.h>

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

int main(void) {

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "k-means");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(GetColor(MYBACKGROUNDCOLOR));
    Vector2 sample = {-10, -10};
    DrawCircleV(project_sample_to_screen(sample), 10, RED);
    DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 10, RED);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
