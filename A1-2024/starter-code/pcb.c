#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include "shellmemory.h"
#include "helpers.h"

struct READY_QUEUE ready_queue = {NULL};

// this method creates the process control block for a script beginning with key
// {pid}_0 set by the method set_value in shellmemory.h.
// dynamically allocated pcb -> needs to be freed
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
    pcb->job_length_score = number_of_lines;
    enqueue(pcb);

    return pcb;
}

struct PCB *create_pcb(FILE *fp) {
    int pid = generate_pid();
    int number_of_lines = load_script_in_memory(fp, pid);
    return instantiate_pcb(pid, number_of_lines);
}

void enqueue(struct PCB *pcb) {
    // make sure added job is in the tail
    pcb->next = NULL;
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
    if (clear_mem(pcb->pid, pcb->number_of_lines) != 0){
        perror("Unable to clear memory.");
        return;
    };
    free(pcb);
}
void swap(struct PCB *min, struct PCB *current) {
    // Can reuse swap later so made a new function for it
    if (min == NULL || current == NULL) return; // Ensure neither pointer is null

    // Temporary variables to hold data for swapping
    int tempPID = min->pid;
    int tempScore = min->job_length_score;
    int tempNOL = min->number_of_lines;
    int tempPC = min->pc;

    // Perform the swap
    min->pid = current->pid;
    min->job_length_score = current->job_length_score;
    min->number_of_lines = current->number_of_lines;
    min->pc = current->pc;

    // Use temp data to put back inside
    current->pid = tempPID;
    current->job_length_score = tempScore;
    current->number_of_lines = tempNOL;
    current->pc = tempPC;
}

void selectionSortQueue() {
    if (ready_queue.head == NULL) return; // We don't care if the queue is null

    struct PCB *current, *min, *next;
    current = ready_queue.head;

    while (current != NULL){
        min = current; // Initial min is the first element in the queue
        next = current->next;

        while (next != NULL) {
            if (next->number_of_lines < min->number_of_lines) {
                min = next;
            }
            next = next -> next;
        }
        if (min != current) { // switching the nodes if it's not in order
            swap(min, current);
        }
        current = current->next;
    }
}