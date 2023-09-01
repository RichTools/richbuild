#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define CLEANUP() clean_up()

#define COMPILE(compiler, files, cflags, executable_name, packages) \
  compile_files(compiler, files, cflags, executable_name, packages)
#define READ_FILES() get_files_windows()

#define INFO(msg) printf("[INFO] %s\n", msg)
#define CMD(cmd) printf("[CMD] %s\n", cmd)
#define DONE(msg, file) printf("[DONE] %s : %s\n", msg, file)

int str_ends_with(const char* string, const char* suffix);
int str_starts_with(const char* string, const char* prefix); 
void get_files_windows();
void compile_files(char* compiler, const char* files, const char* cflags, char* executable_name, char* packages);

const size_t number_of_files_to_build = 40;
const size_t filename_size_limit = 256;
char* file_list;


int main() {
  
  READ_FILES();
  printf("Hello World \n");
  return 1;
}

void get_files_windows() {
  LPWIN32_FIND_DATAA find_file_data;
  HANDLE hFind;
  
  file_list = (char*)malloc(sizeof(char) * filename_size_limit * number_of_files_to_build);
  
  TCHAR buffer[MAX_PATH];
  DWORD dwRet;
  
  // This returns the wrong path right now!?
  // it returns gibberish, probably because of how i'm 
  // running it but I don't actually know
  dwRet = GetCurrentDirectory((TCHAR)MAX_PATH, buffer);
  if (dwRet == 0) {
    DWORD error = GetLastError();
    printf("Error getting current directory: %lu \n", error);
    return 1;
  }

  printf("%s", buffer);

  LPCSTR sPath[2048];
  sprintf(sPath, "%s\\*.*", buffer);
  
  hFind = FindFirstFile(sPath, find_file_data);
   
  if (hFind == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    printf("Error finding files %lu \n", error);
    return 1;
  }

  strcpy(file_list, "");
  
  
  do {
    if (!(find_file_data->dwFileAttributes && FILE_ATTRIBUTE_DIRECTORY)) {
      if (str_ends_with(find_file_data->cFileName, ".c") && !str_starts_with(find_file_data->cFileName, "richBuild")) {
        strcat(file_list, find_file_data->cFileName);
        strcat(file_list, " ");
        printf("File: %s\n", find_file_data->cFileName);
      }
    }
  } while (FindNextFile(hFind, find_file_data) != 0);
  

  FindClose(hFind);
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
