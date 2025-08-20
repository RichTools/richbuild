# RICHBUILD
A Header only library to make compiling C projects at least a tiny bit easier. Is this one of those projects where in hindsight it probably would have been quicker to just compile things yourself? Probably.

## What it offers?
The primary goal for RichBuild was to remove to need to keep adding files into your compile command.
Picture this, you start with a `main.c` file so you add that to your build script (I normall use a shell file).
You keep working and oh you need to add another C file to link with, so you have to update your script etc...

RichBuild remove that step by automatically getting all the C files in the current directory and adding them onto your compile command.
All you have to do is write a quick `richBuild.c` file where you define your config. From there the only file you need to compile and run is the richBuild file, and then your actual program executable. 

## How to use it?

1. Download the `richBuild.h` header and place it in your project.
2. Make a `richBuild.c` file and use the below example.

```c
#define RICHBUILD_IMPLEMENTATION
#include "richBuild.h"

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
```
You are required to:
- Define the Implementation (Line 1).
- Include the rich build header (Line 2).
- Define your cflags if you don't want any define it as an empty string ("").
- Choose the name of your executable.
- Feel free to name the build project function in any way you want.
- Use the READ_FILES, COMPILE and CLEANUP macros
- Remember to call your function.

3. Compile the richBuild.c file in any way you like. 
4. Run the richBuild executable
5. Run your program.

## Packages/ Linking 
Sometimes you may want to add packages to the compile command. 
You can add those as the final argument of the COMPILE macro.

```c
#define cflags "-Wall"
#define executable_name "main"
#define packages "-lm"

void BUILD_PROJECT() {
  const char* files = READ_FILES();
  COMPILE("gcc", files, cflags, executable_name, packages);
  CLEANUP();
}
```
**Currently Builtin Package Configs**
- Raylib

```c
COMPILE("gcc", files, cflags, executable_name, RAYLIB);
```
*__NOTE:__*
There is currently no way to add builtin packages along with others so you will have to compose those yourself via the package argument <3

## Excluding Files
Excluding files is as simple as using the `EXCLUDE` macro.

```c
#define RICHBUILD_IMPLEMENTATION
#include "richBuild.h"

#define cflags "-Wall"
#define executable_name "main"

void BUILD_PROJECT() {
  EXCLUDE("file.c", "anotherfile.c");
  const char* files = READ_FILES();
  COMPILE("gcc", files, cflags, executable_name, NULL);
  CLEANUP();
}

int main() {
  BUILD_PROJECT();
  return 0;
}
```

## Arbitrary Shell Commands 
We can run arbitrary shell commands using `Cmd`

```c
#define RICHBUILD_IMPLEMENTATION
#include "richBuild.h"

#define cflags "-Wall"
#define executable_name "main"

void BUILD_PROJECT() {
  EXCLUDE("file.c", "anotherfile.c");
  const char* files = READ_FILES();
  COMPILE("gcc", files, cflags, executable_name, NULL);
    
  // initialise, append, run
  Cmd cmd = {0};
  Cmd_append(&cmd, "./main");

  CMD_RUN(&cmd);
 
  CLEANUP();
}

int main() {
  BUILD_PROJECT();
  return 0;
}
```

## How it works.
The Library is an STB style single header library so you are required to define the implementation.
Example and explaination of STB style libraries [here](https://github.com/nothings/stb#how-do-i-use-these-libraries). The library is comprised of a set of macro definitions which call to other functions.

First we use the `READ_FILES` macro to read the .c files in the file system (current directory). This will ignore any files with the name `richBuild.c` so make sure that is the name of your config file for RichBuild. The names of all the C files will be appended onto the compile command for you. 

Next calling the compile macro we pass the **name of the compiler** we want to use, the list of files, name of your executable, the final argument are any packages you would like to add such as math via the "-lm" flag. 

Finally we call the `CLEANUP` macro because yes `richBuild` allocates memory to store the name of your files, so we need to free it after.

## A Word about Windows
I tested, using wine and wingw-w64 and it broke completely, so I have now removed the code for interfacing the windows API, I don't want to fix it. Use a C compiler from WSL to run.

## References
The main inspiration here was Tsoding more specifically (https://github.com/tsoding/nob.h) as much as RichBuild isn't anything like Nob, Nob was where the inspiration came from. Tsodings streams have always been and always will be my biggest source of motivation and inspiration.
