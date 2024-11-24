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
struct PCB *instantiatePCB(int pid, struct pagingReturn *returnPage) {
    struct PCB *pcb = (struct PCB *)malloc(sizeof(struct PCB));
    if (!pcb) {
        printf("Failed to allocate memory for PCB.\n");
        return NULL;
    }

    pcb->pid = pid;
    pcb->number_of_lines = number_of_lines;
    pcb->pc = 0;
    pcb->next = NULL;
    pcb->job_length_score = returnPage->numberLines;
    pcb->pageTable = returnPage->pageTable;
    enqueue(pcb);

    return pcb;
}

struct PCB *createPCB(FILE *fp, struct pagingReturn *returnPage) {
    int pid = generatePID();
    return instantiatePCB(pid, returnPage);
}

struct PCB *createDuplicatePCB(char* fileName) {
    int pid = generatePID();
    PCB* current = readyQueue.head;

    while (current != NULL){
        if (strcmp(current->scriptName, fileName) == 0){
            break;
        }
        current = current->next;
    }
    if (current == NULL){
        return;
    }
    struct pagingReturn *returnPage;
    returnPage->numberLines = current->number_of_lines;
    returnPage->pageTable = current->pageTable;



    return instantiatePCB(pid, returnPage);
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
    // Ensure that added job's (the tail's) next pointer
    // is NULL to avoid cycles. 
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

// Used for background execution as stdin commands
// get priority in readyQueue -> enqueue in head.
void enqueueHead(struct PCB *pcb) {
    if (readyQueue.head == NULL) {
        readyQueue.head = pcb;
    } else {
        pcb->next = readyQueue.head;
        readyQueue.head = pcb;
    }
}

// Dequeues from the head.
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
    removeScriptSharedMemory(pcb->scriptName);
    // If variable has already been cleared, no need to clear again.
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
    // Min nodes gets the values of the current node (iterates from the head to the end)
    min->pid = current->pid;
    min->job_length_score = current->job_length_score;
    min->number_of_lines = current->number_of_lines;
    min->pc = current->pc;

    // Use temp data to put back inside
    // Current node gets value from the min node
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
        min = current; // min is the left most element
        // If current != head it means that we know the elements left of current is smaller
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
        current = current->next;    // iterate to the next node since we know current
                                    // is sorted, so we can move to the
                                    // next one
    }
}

void ageReadyQueue() {
    if (readyQueue.head == NULL)
        return;
    struct PCB *current = readyQueue.head;
    // Since we dequeued the head we know we can age every element in the queue
    // We iterate through the queue and decrement each job_length_score by 1
    // If the score is = 0, we can keep it as is and iterate to the next node
    while (current != NULL) {
        if (current->job_length_score != 0) {
            current->job_length_score--;
        }
        current = current->next;
    }
}