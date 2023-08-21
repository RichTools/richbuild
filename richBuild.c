#define RICHBUILD_IMPLEMENTATION
#include "./richBuild.h"

#define cflags "-Wall"
#define executable_name "main"

void BUILD_PROJECT() {
  const char* files = READ_FILES();
  COMPILE("gcc", files, cflags, executable_name, NULL);
  CLEANUP();
}

int main() {
  BUILD_PROJECT();
  return 0;
}
