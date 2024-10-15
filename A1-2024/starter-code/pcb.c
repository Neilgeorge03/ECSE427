#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"

struct PCB create_pcb(int pid, char *start, int number_of_lines, int current_line) {
    struct PCB pcb;

    pcb.pid = pid;
    pcb.start = start;
    pcb.number_of_lines = number_of_lines;
    pcb.pc = current_line;
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

