#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "shell.h"
#include "pcb.h"
#include "shellmemory.h"
#include "helpers.h"

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
    } while(ready_queue.head != NULL); 
    return errCode;
}