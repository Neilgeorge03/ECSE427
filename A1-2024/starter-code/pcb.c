#include "pcb.h"
#include "helpers.h"
#include "shellmemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct READY_QUEUE readyQueue = {NULL};

void enqueueHead(struct PCB *pcb);

// this method creates the process control block for a script beginning with key
// {pid}_0 set by the method set_value in shellmemory.h.
// dynamically allocated pcb -> needs to be freed
struct PCB *instantiatePCB(int pid, int number_of_lines) {
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

struct PCB *createPCB(FILE *fp) {
    int pid = generatePID();
    int number_of_lines = loadScriptInMemory(fp, pid);
    return instantiatePCB(pid, number_of_lines);
}

struct PCB *createBackgroundPCB() {
    struct PCB *pcb = (struct PCB *)malloc(sizeof(struct PCB));
    if (!pcb) {
        printf("Failed to allocate memory for PCB.\n");
        return NULL;
    }

    // this is a placeholder to know this is a background pcb.
    pcb->pid = -100;
    pcb->next = NULL;
    enqueueHead(pcb);

    return pcb;
}

void enqueue(struct PCB *pcb) {
    // make sure added job is in the tail
    pcb->next = NULL;
    if (readyQueue.head == NULL) {
        readyQueue.head = pcb;
    } else {
        struct PCB *copy_head = readyQueue.head;
        while (copy_head->next != NULL) {
            copy_head = copy_head->next;
        }
        copy_head->next = pcb;
    }
}

void enqueueHead(struct PCB *pcb) {
    if (readyQueue.head == NULL) {
        readyQueue.head = pcb;
    } else {
        pcb->next = readyQueue.head;
        readyQueue.head = pcb;
    }
}

struct PCB *dequeue() {
    if (readyQueue.head == NULL) {
        return NULL;
    }

    struct PCB *copy_pcb = readyQueue.head;
    readyQueue.head = copy_pcb->next;
    return copy_pcb;
}

void freePCB(struct PCB *pcb) {
    char key[KEY_SIZE];
    sprintf(key, "%d_0", pcb->pid);
    char *checkValue = mem_get_value(key);

    if (strcmp(checkValue, "Variable does not exist") != 0) {
        if (clearMemory(pcb->pid, pcb->number_of_lines) != 0) {
            perror("Unable to clear memory.");
            return;
        };
        free(pcb);
    }
}

void swap(struct PCB *min, struct PCB *current) {
    // Swapping switches the values in each node but keeps the pointers as such
    // Simplifies the swap and we don't get errors due to pointers
    // Can reuse swap later so made a new function for it
    if (min == NULL || current == NULL)
        return; // Ensure neither pointer is null

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
    // Selection sort because we have a singly linked list and I like selection
    // more than quick sort :>
    if (readyQueue.head == NULL || readyQueue.head->next == NULL)
        return; // We don't care if the queue is null
    struct PCB *current, *min, *next;
    current = readyQueue.head; // what we'll use to iterate through the queue

    while (current != NULL) {
        min = current; // Initial min is the first element in the queue
        next = current->next;

        while (next != NULL) {
            // Finding the min value from current node to the end of the linked
            // list
            if (next->job_length_score < min->job_length_score) {
                min = next;
            }
            next = next->next;
        }
        if (min != current) { // switching the nodes if it's not in order
            swap(min, current);
        }
        current =
            current->next; // iterate to the next node after swap if it occured
    }
}

void ageReadyQueue() {
    if (readyQueue.head == NULL)
        return;
    struct PCB *current = readyQueue.head;

    while (current != NULL) {
        if (current->job_length_score != 0) {
            current->job_length_score--;
        }
        current = current->next;
    }
}