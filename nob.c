#define NOB_IMPLEMENTATION
#include "./src/nob.h"

Nob_Proc build_program(const char *source_path, const char *output_path) {
  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, "cc");
  nob_cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb");
  nob_cmd_append(&cmd, "-I./lib/");
  nob_cmd_append(&cmd, "-o", output_path, source_path);
  nob_cmd_append(&cmd, "-L./lib/");
  nob_cmd_append(&cmd, "-lraylib", "-lm");
  nob_cmd_append(&cmd, "-framework", "CoreVideo");
  nob_cmd_append(&cmd, "-framework", "IOKit");
  nob_cmd_append(&cmd, "-framework", "Cocoa");
  nob_cmd_append(&cmd, "-framework", "GLUT");
  nob_cmd_append(&cmd, "-framework", "OpenGL");
  return nob_cmd_run_async(cmd);
}

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  if (!nob_mkdir_if_not_exists("./build/"))
    return 1;
  Nob_Procs procs = {0};
  nob_da_append(&procs, build_program("./src/main.c", "./build/kmeans"));
  if (!nob_procs_wait(procs)) {
    return 1;
  }
  return 0;
}
