#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "shell.h"
#include "pcb.h"
#include "shellmemory.h"
#include "helpers.h"

const char *ExecutionPolicy[] = {
    "FCFS",
    "SJF",
    "RR",
    "AGING",
    "RR30"
};

int execute_FCFS() {
    int errCode;
    struct PCB *copy_pcb;
    char key[100];

    do {
        copy_pcb = dequeue();

        int last_index = copy_pcb->number_of_lines;

        // pc here refers to "program counter"
        while (copy_pcb->pc < last_index){
            sprintf(key, "%d_%d", copy_pcb->pid, copy_pcb->pc);
            errCode = execute_instruction(key);
            copy_pcb->pc++;
        }
        // important to free to remove from shell and computer memory
        free_pcb(copy_pcb);
    } while(ready_queue.head != NULL); 
    return errCode;
}

int execute_AGING() {
    int errCode;
    struct PCB *copy_pcb, *head;
    char key[100];

    do {
        copy_pcb = dequeue();
        // To check if either head is bigger or we're done all the lines in the pcb
        // We have a ternary, basically if we're the last element in the queue it'll be empty
        // Thus causing an error, so we make head the same as copy_pcb if that's the case
        head = (ready_queue.head != NULL) ? ready_queue.head : copy_pcb;
        int last_index = copy_pcb->number_of_lines;
        while ((copy_pcb->pc < last_index) && (copy_pcb->job_length_score <= head->job_length_score)){
            sprintf(key, "%d_%d", copy_pcb->pid, copy_pcb->pc);
            errCode = execute_instruction(key);
            copy_pcb->pc++;
            ageReadyQueue();
        }

        if (copy_pcb->pc == copy_pcb->number_of_lines) {
            free_pcb(copy_pcb);
        } else {
            enqueue(copy_pcb);
            selectionSortQueue();
        }
    } while(ready_queue.head != NULL);
    return errCode;
}

int execute_RR(int count) {
    int errCode;
    struct PCB *copy_pcb;
    char key[100];
    do {
        copy_pcb = dequeue();
        // + 2 because in RR we run two instruction before switching
        int last_index = copy_pcb->pc + count;

        while (copy_pcb->pc < last_index && copy_pcb->pc < copy_pcb->number_of_lines){
            sprintf(key, "%d_%d", copy_pcb->pid, copy_pcb->pc);
            errCode = execute_instruction(key); 
            copy_pcb->pc++;
        }

        if (copy_pcb->pc == copy_pcb->number_of_lines) {
            free_pcb(copy_pcb);
        } 
        else {
            enqueue(copy_pcb);
        }

    } while(ready_queue.head != NULL); 
    return errCode;
}