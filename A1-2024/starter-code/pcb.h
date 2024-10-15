#ifndef PCB_H
#define PCB_H

struct PCB {
    int pid;
    // Where the start of script is stored
    char *start;
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

struct PCB create_pcb(int pid, char *start, int number_of_lines, int current_line);

#endif
