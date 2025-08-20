#ifndef RICHBUILD_H
#define RICHBUILD_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>
#include <stdarg.h>


// Common Packages

#define RAYLIB "-lraylib `pkg-config --libs --cflags raylib`"

// Declarations

typedef struct {
  char** items;
  size_t count;
  size_t capacity;
} Cmd;

typedef struct {
  char** items;
  size_t count;
  size_t capacity;
} ExludedFiles;

static ExludedFiles excluded = {0};

// Find if a string ends with a specfied suffix 
// Returns 1 if true and 0 if false
int str_ends_with(const char* string, const char* suffix);

// Find if a string starts with a specifed prefic
// Returns 1 if true and 0 if fase 
int str_starts_with(const char* string, const char* prefix); 

// return a string containing all the C files in the current 
// directory excluding the build script itself
char* get_files();

// check if a file has been excluded
int is_excluded(const ExludedFiles* excludes, const char *filename);

// remove files from the build
void exclude_files(int size, ...);

// add everything to the compile command and run it
void compile_files(char* compiler, const char* files, const char* cflags, char* executable_name, char* packages);

// command to string 
char* cmd_to_string(Cmd* cmd);
// run arbitrary commands
int run_command(Cmd* command);

// Macro Definitions

#define da_append(xs, x)\
  do { \
    if ((xs)->count >= (xs)->capacity) {\
      if ((xs)->capacity == 0) (xs)->capacity = 256;\
      else (xs)->capacity *= 2;\
      (xs)->items = realloc((xs)->items, (xs)->capacity*sizeof(*(xs)->items));\
    }\
    (xs)->items[(xs)->count++] = x;\
  } while (0)

#define da_append_many(xs, new_xs, new_xs_count)\
  do {\
    if (new_xs_count >= (xs)->capacity) {\
      if ((xs)->capacity == 0) (xs)->capacity = 256;\
      else (xs)->capacity *= 2;\
      (xs)->items = realloc((xs)->items, (xs)->capacity*sizeof(*(xs)->items));\
    }\
    memcpy((xs)->items+(xs)->count,(new_xs),(new_xs_count)*sizeof(*(xs)->items)); \
    (xs)->count += (new_xs_count);\
  } while(0)

#define Cmd_append(cmd, ...)\
  da_append_many(cmd, \
      ((const char*[]){__VA_ARGS__}), \
      (sizeof((const char*[]){__VA_ARGS__})/sizeof(const char*)))


#define READ_FILES() get_files()
#define CLEANUP() clean_up()
#define CMD_RUN(cmd) run_command(cmd);

#define COMPILE(compiler, files, cflags, executable_name, packages) \
  compile_files(compiler, files, cflags, executable_name, packages)

#define EXCLUDE(...) \
    exclude_files(sizeof((const char*[]){__VA_ARGS__})/sizeof(const char*), __VA_ARGS__)

#define INFO(msg) printf("[INFO] %s\n", msg)
#define CMD(cmd) printf("[CMD] %s\n", cmd)
#define DONE(msg, file) printf("[DONE] %s : %s\n", msg, file)

#endif // RICHBUILD_H


#ifdef RICHBUILD_IMPLEMENTATION

// top level constants
const size_t NUMBER_OF_FILES_TO_BUILD = 40;
const size_t FILENAME_SIZE_LIMIT = 256;

char* file_list = NULL;

int is_excluded(const ExludedFiles* excludes, const char *filename)
{
    for (size_t i = 0; i < excludes->count; i++) {
        if (strcmp(excludes->items[i], filename) == 0) {
            return 1; // match found
        }
    }
    return 0;
}


char* get_files() {
  DIR *directory;
  struct dirent *dir;

  // allocate memory for the file names
  file_list = (char*)malloc(sizeof(char) * FILENAME_SIZE_LIMIT * NUMBER_OF_FILES_TO_BUILD);

  directory = opendir(".");
  if (directory) {
    strcpy(file_list, "");

    while ((dir = readdir(directory)) != NULL) {
      if (str_ends_with(dir->d_name, ".c") && 
          !str_starts_with(dir->d_name, "richBuild") &&
          !is_excluded(&excluded, dir->d_name)) 
      {
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

void exclude_files(int size, ...) {
  if (size <= 0) return;

  va_list list_ptr;
  va_start(list_ptr, size);

  for (int i = 0; i < size; i++) {
    char* word = va_arg(list_ptr, char *);
    da_append(&excluded, word);
  }

  va_end(list_ptr);
}



char* cmd_to_string(Cmd* cmd) 
{
  if (cmd == NULL || cmd->count == 0)
  {
    char* result = malloc(1);
    if (result) result[0] = '\0';
    return result;
  }

  size_t total = 0;
  for (size_t i = 0; i < cmd->count; ++i)
  {
    total += strlen(cmd->items[i]) + 1;
  }

  char *result = malloc(total + 1);
  result[0] = '\0';

  for (size_t i = 0; i < cmd->count; ++i) 
  {
    strcat(result, cmd->items[i]);
    if (i < cmd->count - 1)
      strcat(result, " ");
  }
  return result;
}



int run_command(Cmd* cmd)
{
  FILE* p;
  int ch;

  char* command = cmd_to_string(cmd);
  p = popen(command, "r");
  CMD(command);

  if (p == NULL) 
  {
    printf("Unable to open process\n");
    return 1;
  }

  while ((ch = fgetc(p)) != EOF)
    putchar(ch);
  
  pclose(p);
  return 0;
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
