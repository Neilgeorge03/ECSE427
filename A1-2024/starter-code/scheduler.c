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
        // When doing "load_script_in_memory", each line ends with ';'. Thus
        // easy to find when each line end using this.
        char *line = strtok(copy_pcb->start, ";");
        while(line != NULL) {
            errCode = parseInput(line);
            line = strtok(NULL, ";");
        } 
        
        ready_queue.head = copy_pcb->next;
    } while(ready_queue.head != NULL); 
    return errCode;
}