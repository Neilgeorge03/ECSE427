#include <stdio.h>

#ifndef PCB_H
#define PCB_H

struct PCB {
    int pid;
    // Length of SCRIPT based on how many lines
    int number_of_lines;
    // Program counter (pc). Keeps track of instruction to execute.
    // pc will always be smaller or equal to number_of_lines
    int pc;
    // Pointer to next PCB 
    struct PCB *next;
    // Score is equal to number_of_lines when the program starts, we decrement it in aging
    // can't go lower than 0 but it's used to sort the PCB
    int job_length_score;
    int pageTable[(FRAME_STORE_SIZE / FRAME_SIZE)];
    char scriptName[100];
};
struct pagingReturn {
    int numberLines;
    int pageTable[(FRAME_STORE_SIZE / FRAME_SIZE)];
};

// Linked-List data structure used as a queue
struct READY_QUEUE {
    struct PCB *head;
};

// Global ready_queue -> extern. Declared in pcb.c
extern struct READY_QUEUE readyQueue;
struct PCB *createBackgroundPCB();
struct PCB *createPCB(FILE *fp, struct pagingReturn *returnPage);
struct PCB *createDuplicatePCB(char* fileName);
struct PCB *instantiatePCB(int pid, struct pagingReturn *returnPage);
void enqueue(struct PCB *pcb);
struct PCB *dequeue();
void freePCB(struct PCB *pcb);
void selectionSortQueue();
void swap(struct PCB *min, struct PCB *current);
void ageReadyQueue();

#endif
