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

void selectionSortQueue() {
    if (ready_queue.head == NULL) return; // We don't care if the queue is null

    struct PCB *current, *min, *next, *prevMin, *prevNext, *prevCurrent;

    current = ready_queue.head;
    prevCurrent = ready_queue.head;
    while (current != NULL){
        min = current; // inital min is the first element in the queue
        next = current->next;
        prevNext = current;
        prevMin = NULL;
        printf("Current: %d\n", current->pid);

        while (next != NULL) {
            if (next->number_of_lines < min->number_of_lines) {
                min = next;
                prevMin = prevNext;
            }
            prevNext = next;
            next = next -> next;
            printf("next: %d\n", prevNext->pid);
        }
        printf("kjbijubiojub\n");
        if (min != current) { // switching the nodes if it's not in order
            printf("min pid: %d\n", min->pid);
            printf("current: %d\n", current->pid);
            printf("head: %d\n", ready_queue.head->pid);
            struct PCB *temp = current;


            min->next = current->next;
            prevMin->next = temp;
            if (current->pid == ready_queue.head->pid){ // When we have to swap to first element
                ready_queue.head = min;
            } else {
                prevCurrent->next = min;
            }
            prevCurrent = min;
        }

        current = prevCurrent->next;
    }
    current = ready_queue.head;
    for (int i = 0; i < 2; i++){
        printf("%d: ", current->pid);
        printf("%d\n", current->next->pid);
        current = current->next;
    }
}