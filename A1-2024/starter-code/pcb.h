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

extern struct READY_QUEUE ready_queue;

struct PCB *instantiate_pcb(int pid, int number_of_lines);
void enqueue(struct PCB *pcb);
struct PCB *create_pcb(FILE *fp);
struct PCB *dequeue();
void free_pcb(struct PCB *pcb);

#endif
