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
    // TODO: NEIL EXPLAIN
    int job_length_score;
};

// Linked-List data structure used as a queue
struct READY_QUEUE {
    struct PCB *head;
};

// Global ready_queue -> extern. Declared in pcb.c
extern struct READY_QUEUE readyQueue;
struct PCB *createBackgroundPCB();
struct PCB *createPCB(FILE *fp);
struct PCB *instantiatePCB(int pid, int number_of_lines);
void enqueue(struct PCB *pcb);
struct PCB *dequeue();
void freePCB(struct PCB *pcb);
void selectionSortQueue();
void swap(struct PCB *min, struct PCB *current);
void ageReadyQueue();

#endif
