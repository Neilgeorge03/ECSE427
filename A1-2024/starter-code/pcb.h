#include <stdio.h>

#ifndef PCB_H
#define PCB_H

struct PCB {
    int pid;
    // Length of SCRIPT based on how many lines
    int number_of_lines;
    // Program counter (pc). Keeps track of instruction to execute.
    // pc <= number_of_lines
    int pc;
    // Next PCB
    struct PCB *next;
    // 1.2.4 Aging SJF job
    int job_length_score;
};

struct PCB *createEmptyPCB();
struct PCB *instantiate_pcb(int pid, int number_of_lines);
struct PCB *create_pcb(FILE *fp);
void free_pcb(struct PCB *pcb);
void swap(struct PCB *min, struct PCB *current);
void addPCBForegroundCommand(char *commandString);

#endif
