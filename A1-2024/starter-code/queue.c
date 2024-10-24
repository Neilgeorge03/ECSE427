#include "pcb.h"
#include "helpers.h"
#include "shellmemory.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

struct READY_QUEUE ready_queue = {NULL};

void enqueue(struct PCB *pcb) {
    // make sure added job is in the tail
    pcb->next = NULL;
    if (ready_queue.head == NULL) {
        ready_queue.head = pcb;
    } else {
        struct PCB *copy_head = ready_queue.head;
        while (copy_head->next != NULL) {
            copy_head = copy_head->next;
        }
        copy_head->next = pcb;
    }
}

void enqueueHead(struct PCB *pcb) {
    if (ready_queue.head == NULL) {
        ready_queue.head = pcb;
    } else {
        pcb->next = ready_queue.head;
        ready_queue.head = pcb;
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

void ageReadyQueue() {
    if (ready_queue.head == NULL)
        return;
    struct PCB *current = ready_queue.head;

    while (current != NULL) {
        if (current->job_length_score != 0) {
            current->job_length_score--;
        }
        current = current->next;
    }
}

void selectionSortQueue() {
    // Selection sort because we have a singly linked list and I like selection
    // more than quick sort :>
    if (ready_queue.head == NULL || ready_queue.head->next == NULL)
        return; // We don't care if the queue is null
    struct PCB *current, *min, *next;
    current = ready_queue.head; // what we'll use to iterate through the queue

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
        current = current->next; // iterate to the next node after swap if it occured
    }
}