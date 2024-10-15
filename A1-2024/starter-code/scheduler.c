#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "shellmemory.h"
#include "shell.h"
#include "helpers.h"
#include "pcb.h"


int execute_FCFS() {
    int errCode;
    struct PCB *copy_pcb;

    do {
        copy_pcb = ready_queue.head;
        char *line = strtok(copy_pcb->start, "\n;");
        while(line != NULL) {
            //printf("%s\n", line);
            // try blankline.txt and it fails. problem in blanklines.
            errCode = parseInput(line);
            line = strtok(NULL, "\n;");
        } 
        
        ready_queue.head = copy_pcb->next;
        // erase from shell memory
    } while(ready_queue.head != NULL); 
    return errCode;
}