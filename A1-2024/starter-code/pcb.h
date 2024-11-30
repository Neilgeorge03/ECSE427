#include "shell.h"
#include <stdio.h>

#ifndef PCB_H
#define PCB_H

#define FRAME_SIZE 3
#ifndef FRAME_STORE_SIZE
#define FRAME_STORE_SIZE 3
#endif

struct PCB {
    int pid;
    // Length of SCRIPT based on how many lines
    int number_of_lines;
    // Program counter (pc). Keeps track of instruction to execute.
    // pc will always be smaller or equal to number_of_lines
    int pc;
    // Pointer to next PCB
    struct PCB *next;
    // Score is equal to number_of_lines when the program starts, we decrement
    // it in aging can't go lower than 0 but it's used to sort the PCB
    int job_length_score;
    int pageTable[MAX_PAGES];
    char scriptName[100];
};

// Linked-List data structure used as a queue
struct READY_QUEUE {
    struct PCB *head;
};

// Global ready_queue -> extern. Declared in pcb.c
extern struct READY_QUEUE readyQueue;
struct PCB *createBackgroundPCB();
struct PCB *createPCB(FILE *fp);
struct PCB *createFramePCB(FILE *fp, struct pagingReturn *returnPage, char *fileName);
struct PCB *createDuplicatePCB(char *fileName);
struct PCB *instantiateFramePCB(int pid, struct pagingReturn *returnPage, char *scriptName);
struct PCB *instantiatePCB(int pid, int numberLines);
void enqueue(struct PCB *pcb);
struct PCB *dequeue();
struct PCB *getPCBHead();
void freePCB(struct PCB *pcb);
void selectionSortQueue();
void addScriptName(struct PCB *pcb, char *scriptName);
void swap(struct PCB *min, struct PCB *current);
void ageReadyQueue();

#endif
