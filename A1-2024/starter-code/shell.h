#include <stdio.h>
#ifndef SHELL_H
#define SHELL_H

#define MAX_USER_INPUT 1000
#define MEM_SIZE 1000
#define MAX_SCRIPT_SIZE (MAX_USER_INPUT * 100)
#define KEY_SIZE 100
#define BACKING_STORE "backingStore"
#define FRAME_STORE_SIZE 30
#define FRAME_SIZE 3

struct pagingReturn {
    int numberLines;
    int pageTable[(FRAME_STORE_SIZE / FRAME_SIZE)];
};

int parseInput(char inp[]);

struct pagingReturn *loadScriptBackingStore(char* dirName, char* scriptName, FILE* fp);
#endif