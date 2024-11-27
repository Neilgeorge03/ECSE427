#include <stdio.h>
#include "shell.h"
#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H

#define MEM_SIZE 1000
#define MAX_SCRIPT_SIZE (MAX_USER_INPUT * 100)
#define KEY_SIZE 100
#define VARIABLE_STORE_SIZE 30
#define MAX_PROCESSES 100

void mem_init();
void initFrameStore();
int getFreeFrame();
void loadPageFrameStore(int index, char* fileName);
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
char *variable_get_value(char *var);
void variable_set_value(char *var, char *value);
int loadScriptInMemory(FILE *fp, int pid);
int clearMemory(int pid, int length);
int loadScriptSharedMemory(char *scriptName);
int removeScriptSharedMemory(char *scriptName);
char* getLine(int frameIndex, int offset);
void deleteFrame(int frameIndex);
int checkScriptLoaded(char *scriptName);

struct memory_struct {
    char *var;
    char *value;
};
struct sharedProcess {
    char processName[100];
    int count;
};



#endif