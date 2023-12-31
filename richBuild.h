#ifndef RICHBUILD_H
#define RICHBUILD_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>

// Common Packages

#define RAYLIB "-lraylib `pkg-config --libs --cflags raylib`"

// Declarations

int str_ends_with(const char* string, const char* suffix);
int str_starts_with(const char* string, const char* prefix); 
char* get_files();
void compile_files(char* compiler, const char* files, const char* cflags, char* executable_name, char* packages);

// Macro Definitions

#define READ_FILES() get_files()
#define CLEANUP() clean_up()

#define COMPILE(compiler, files, cflags, executable_name, packages) \
  compile_files(compiler, files, cflags, executable_name, packages)

#define INFO(msg) printf("[INFO] %s\n", msg)
#define CMD(cmd) printf("[CMD] %s\n", cmd)
#define DONE(msg, file) printf("[DONE] %s : %s\n", msg, file)

#endif // RICHBUILD_H


#ifdef RICHBUILD_IMPLEMENTATION

const size_t number_of_files_to_build = 40;
const size_t filename_size_limit = 256;
char* file_list;

char* get_files() {
  DIR *directory;
  struct dirent *dir;
  file_list = (char*)malloc(sizeof(char) * filename_size_limit * number_of_files_to_build);

  directory = opendir(".");
  if (directory) {
    strcpy(file_list, "");

    while ((dir = readdir(directory)) != NULL) {
      if (str_ends_with(dir->d_name, ".c") && !str_starts_with(dir->d_name, "richBuild")) {
        strcat(file_list, dir->d_name);
        strcat(file_list, " ");
      }
    }
    closedir(directory);
  }
  return file_list;
}

int str_ends_with(const char* string, const char* suffix) {
  if (!string || !suffix) {
    return 0;
  }
  
  size_t string_len = strlen(string);
  size_t suffix_len = strlen(suffix);

  if (suffix_len > string_len) {
    return 0;
  }

  return strncmp(string + string_len - suffix_len, suffix, suffix_len) == 0;
}

int str_starts_with(const char* string, const char* prefix) {
  if (!prefix || !string) {
    return 0;
  }

  size_t prefix_length = strlen(prefix);

  if (prefix_length > strlen(string)) {
    return 0;
  }

  return strncmp(string, prefix, prefix_length) == 0;
}

void compile_files(char* compiler, const char* files, const char* cflags, char* executable_name, char* packages) {
  char compile_command[256];

  if (packages == NULL) {
    snprintf(compile_command, sizeof(compile_command),
        "%s %s -o %s %s", 
        compiler, cflags, executable_name, files);
  } else {
    snprintf(compile_command, sizeof(compile_command),
        "%s %s -o %s %s %s", 
        compiler, cflags, executable_name, files, packages);
  }

  INFO("Compiling Command:");
  CMD(compile_command);
  system(compile_command);
  DONE("Finshed Compiling", executable_name);
}

void clean_up() {
  free(file_list);
}

#endif // RICHBUILD_IMPLEMENTATION
