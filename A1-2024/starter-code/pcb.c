#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include "shellmemory.h"
#include "helpers.h"

struct READY_QUEUE ready_queue = {NULL};

// this method creates the process control block for a script beginning with key
// {pid}_0 set by the method set_value in shellmemory.h.
struct PCB *instantiate_pcb(int pid, int number_of_lines) {
    struct PCB *pcb = (struct PCB*)malloc(sizeof(struct PCB));
    if(!pcb) {
        printf("Failed to allocate memory for PCB.\n");
        return NULL;
    }

    pcb->pid = pid;
    pcb->number_of_lines = number_of_lines;
    pcb->pc = 0;
    pcb->next = NULL;

    enqueue(pcb);

    return pcb;
}

struct PCB *create_pcb(FILE *fp) {
    int pid = generate_pid();
    int number_of_lines = load_script_in_memory(fp, pid);
    fclose(fp);
    return instantiate_pcb(pid, number_of_lines);
}


void enqueue(struct PCB *pcb) {
    if (ready_queue.head == NULL) {
        ready_queue.head = pcb;
    } else {
        struct PCB *copy_head = ready_queue.head;
        while(copy_head->next != NULL) {
            copy_head = copy_head->next;
        }
        copy_head->next = pcb;
    }
}

struct PCB *dequeue() {
    if (ready_queue.head == NULL) {
        return NULL;
    }

    struct PCB *copy_pcb = ready_queue.head;
    ready_queue.head = copy_pcb->next;
    return copy_pcb;
}

void free_pcb(struct PCB *pcb) {
    free(pcb);
}