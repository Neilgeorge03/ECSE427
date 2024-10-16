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
};

struct READY_QUEUE {
    struct PCB *head;
};

struct READY_QUEUE ready_queue;

struct PCB create_pcb(int pid, int number_of_lines);

#endif
