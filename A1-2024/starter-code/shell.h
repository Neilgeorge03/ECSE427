#include <stdio.h>
#ifndef SHELL_H
#define SHELL_H

#define MAX_USER_INPUT 1000
#define MEM_SIZE 1000
#define MAX_SCRIPT_SIZE (MAX_USER_INPUT * 100)
#define KEY_SIZE 100
#define BACKING_STORE "backingStore"

#ifndef FRAME_STORE_SIZE
#define FRAME_STORE_SIZE 30
#endif

#define FRAME_SIZE 3
#define MAX_PAGES 15
#define MAX_FILES 100

// This structure is used as a way to encapsulate information and return 
// more than one information. 
struct pagingReturn {
    int numberLines;
    int pageTable[MAX_PAGES];
};

struct pagingFileTracker {
    char filename[MAX_USER_INPUT]; // Stores the filename
    struct pagingReturn *pageData; // The paging return data for this file
};

int parseInput(char inp[]);
void initBackingStore();
int addFileToPagingArray(struct pagingReturn *page, char *filename);
int findFileIndex(const char *filename);
void delBackingStore();
void updatePCB2(char *filename);
int parseInputFrameStore(char inp[]);
struct pagingReturn *getPageInfo(int index);
struct pagingReturn *loadScriptBackingStore(char *dirName, char *scriptName,
                                            FILE *fp);
void removePageInfo(char *filename, int removeIndex);
struct PCB *updatePageInfo(struct PCB *pcb, char *filename, int pageTableIndex,
                           int frameStoreIndex);
struct PCB *updatePCB(struct PCB *pcb, char *filename);
#endif