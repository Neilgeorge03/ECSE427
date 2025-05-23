#include "shell.h"
#include <stdio.h>

#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H

#ifndef VARIABLE_STORE_SIZE
#define VARIABLE_STORE_SIZE 10
#endif

#define MEM_SIZE 1000
#define MAX_SCRIPT_SIZE (MAX_USER_INPUT * 100)
#define KEY_SIZE 100
#define MAX_PROCESSES 100

void mem_init();
void initFrameStore();
int getFreeFrame();
void loadPageFrameStore(int index, char *fileName);
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
char *variable_get_value(char *var);
void variable_set_value(char *var, char *value);
int loadScriptInMemory(FILE *fp, int pid);
int clearMemory(int pid, int length);
int loadScriptSharedMemory(char *scriptName);
int removeScriptSharedMemory(char *scriptName);
char *getLine(int frameIndex, int offset);
void deleteFrame(int frameIndex);
void readDemandQueue();
int addTailDemandQueue(int index, char *fileName);
int removeDemandQueue(int index);
int removeDemandHead();
int checkScriptLoaded(char *scriptName);

extern char frameStore[FRAME_STORE_SIZE][100];

struct memory_struct {
    char *var;
    char *value;
};

struct sharedProcess {
    char processName[100];
    int count;
};

// This structure is used to keep track of relevant information
// concerning the frames themselves, and the frame before and after it.
// This is to implement the LRU, and the first frame (head) is the least
// recently used one, which has priority to be evicted. 
struct DemandPagingTracker {
    int frameIndex;
    char fileName[100];
    struct DemandPagingTracker *next;
    struct DemandPagingTracker *prev;
};

struct DemandPagingQueue {
    struct DemandPagingTracker *head;
};

#endif