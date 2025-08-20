#define RICHBUILD_IMPLEMENTATION
#include "../richBuild.h"

#define cflags "-Wall"
#define executable_name "main"

void BUILD_PROJECT() {
  EXCLUDE("virus.c", "anotherVirus.c");
  char* files = READ_FILES();
  COMPILE("gcc", files, cflags, executable_name, NULL);

  Cmd cmd = {0};
  Cmd_append(&cmd, "./main");

  CMD_RUN(&cmd);
  CLEANUP();
}

int main() {
  BUILD_PROJECT();
  return 0;
}
