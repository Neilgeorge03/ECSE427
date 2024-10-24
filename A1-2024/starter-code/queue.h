#ifndef QUEUE_H
#define QUEUE_H

struct READY_QUEUE {
    struct PCB *head;
};

extern struct READY_QUEUE ready_queue;

void enqueue(struct PCB *pcb);
struct PCB *dequeue();
void ageReadyQueue();
void selectionSortQueue();

#endif