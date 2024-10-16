#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"

// this method creates the process control block for a script beginning with key
// {pid}_0 set by the method set_value in shellmemory.h.
struct PCB create_pcb(int pid, int number_of_lines) {
    struct PCB pcb;

    pcb.pid = pid;
    pcb.number_of_lines = number_of_lines;
    pcb.pc = 0;
    pcb.next = NULL;

    if (ready_queue.head == NULL) {
        ready_queue.head = &pcb;
    } else {
        struct PCB *copy_head = ready_queue.head;
        copy_head = ready_queue.head; 
        while(copy_head->next != NULL) {
            copy_head = copy_head->next;
        }
        copy_head->next = &pcb;
    }

    return pcb;
}