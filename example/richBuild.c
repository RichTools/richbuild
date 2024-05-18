#define RICHBUILD_IMPLEMENTATION
#include "../richBuild.h"

#define cflags "-Wall"
#define executable_name "main"

void BUILD_PROJECT() {
  char* files = READ_FILES();
  EXCLUDE(&files, "virus.c", "anotherVirus.c");
  COMPILE("gcc", files, cflags, executable_name, NULL);
  CLEANUP();
}

int main() {
  BUILD_PROJECT();
  return 0;
}
