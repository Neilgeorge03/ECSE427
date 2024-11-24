#include <stdio.h>

#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H

#define MEM_SIZE 1000
#define MAX_SCRIPT_SIZE (MAX_USER_INPUT * 100)
#define KEY_SIZE 100
#define FRAME_STORE_SIZE 30
#define VARIABLE_STORE_SIZE 10
#define MAX_PROCESSES 100
#define FRAME_SIZE 3

void mem_init();
int getFreeFrame();
void loadPageFrameStore(int index, char* fileName)
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int loadScriptInMemory(FILE *fp, int pid);
int clearMemory(int pid, int length);
int loadScriptSharedMemory(char *scriptName);
void removeScriptSharedMemory(char *scriptName);
char* getLine(int frameIndex, int offset);


struct memory_struct {
    char *var;
    char *value;
};
struct sharedProcess {
    char *processName;
    int count;
};



#endif