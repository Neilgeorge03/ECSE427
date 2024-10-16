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
    char key[15];

    do {
        copy_pcb = ready_queue.head;
        int last_index = copy_pcb->number_of_lines;
        // once one is executing, we can move the head to the next in the meantime.
        // (acts like a pop of somekind)
        ready_queue.head = copy_pcb->next;

        // pc here refers to "program counter"
        while (copy_pcb->pc < last_index){
            memset(key, 0, sizeof(key));
            sprintf(key, "%d_%d", copy_pcb->pid, copy_pcb->pc);
            errCode = execute_instruction(key); 
            copy_pcb->pc++;
        }
    } while(ready_queue.head != NULL); 
    return errCode;
}