#define NOB_IMPLEMENTATION
#include "./nob.h"

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, "cc");
  nob_cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb");
  nob_cmd_append(&cmd, "-I./lib/");
  nob_cmd_append(&cmd, "-o", "kmeans", "main.c");
  nob_cmd_append(&cmd, "-L./lib/");
  nob_cmd_append(&cmd, "-lraylib", "-lm");
  nob_cmd_append(&cmd, "-framework", "CoreVideo");
  nob_cmd_append(&cmd, "-framework", "IOKit");
  nob_cmd_append(&cmd, "-framework", "Cocoa");
  nob_cmd_append(&cmd, "-framework", "GLUT");
  nob_cmd_append(&cmd, "-framework", "OpenGL");
  if (!nob_cmd_run_sync(cmd))
    return 1;
  return 0;
}
