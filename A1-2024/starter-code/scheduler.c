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
    "AGING"
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

int execute_RR() {
    int errCode;
    struct PCB *copy_pcb;
    char key[100];
    do {
        copy_pcb = dequeue();
        // + 2 because in RR we run two instruction before switching
        int last_index = copy_pcb->pc + 2;

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