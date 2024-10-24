#include "pcb.h"
#include "helpers.h"
#include "shellmemory.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

// this method creates the process control block for a script beginning with key
// {pid}_0 set by the method set_value in shellmemory.h.
// dynamically allocated pcb -> needs to be freed
struct PCB *instantiate_pcb(int pid, int number_of_lines) {
    struct PCB *pcb = (struct PCB *)malloc(sizeof(struct PCB));
    if (!pcb) {
        printf("Failed to allocate memory for PCB.\n");
        return NULL;
    }

    pcb->pid = pid;
    pcb->number_of_lines = number_of_lines;
    pcb->pc = 0;
    pcb->next = NULL;
    pcb->job_length_score = number_of_lines;
    enqueue(pcb);

    return pcb;
}

struct PCB *create_pcb(FILE *fp) {
    int pid = generate_pid();
    int number_of_lines = load_script_in_memory(fp, pid);
    return instantiate_pcb(pid, number_of_lines);
}

struct PCB *createEmptyPCB() {
    int pid = generate_pid();
    int number_of_lines = 0;
    return instantiate_pcb(pid, number_of_lines);
}

void addPCBForegroundCommand(char *commandString) {
    struct PCB *current = ready_queue.head;
    loadCommandInMemory(commandString, current->number_of_lines, current->pid);
    current->job_length_score++;
    current->number_of_lines++;
}

void free_pcb(struct PCB *pcb) {
    if (clear_mem(pcb->pid, pcb->number_of_lines) != 0) {
        perror("Unable to clear memory.");
        return;
    };
    free(pcb);
}