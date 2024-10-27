#include "scheduler.h"
#include "helpers.h"
#include "pcb.h"
#include "shell.h"
#include "shellmemory.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *ExecutionPolicy[] = {"FCFS", "SJF", "RR", "AGING", "RR30"};
// 0 => single thread
// 1 => multi thread (2 workers)
int isMultithreadingMode = 0;
int isBackgroundMode = 0;
pthread_t thread1, thread2;
pthread_mutex_t mutex;

void executeBackgroundTask(int count);

int executeFCFS() {
    int errCode;
    struct PCB *copy_pcb;
    char key[KEY_SIZE];

    do {
        copy_pcb = dequeue();

        int last_index = copy_pcb->number_of_lines;
        if (copy_pcb->pid == -100) {
            // 1000 is an arbitrary large number to ensure all instructions
            // possible are executed
            executeBackgroundTask(1000);
        }

        // pc here refers to "program counter"
        while (copy_pcb->pid != -100 && copy_pcb->pc < last_index) {
            sprintf(key, "%d_%d", copy_pcb->pid, copy_pcb->pc);
            errCode = executeInstruction(key);
            copy_pcb->pc++;
        }
        // important to free to remove from shell and computer memory
        freePCB(copy_pcb);
    } while (readyQueue.head != NULL);
    return errCode;
}

int executeAging() {
    int errCode;
    struct PCB *copy_pcb, *head;
    char key[KEY_SIZE];

    do {
        copy_pcb = dequeue();

        if (copy_pcb->pid == -100) {
            // 1000 is an arbitrary large number to ensure all instructions
            // possible are executed
            executeBackgroundTask(1000);
        }
        // To check if either head is bigger or we're done all the lines in the
        // pcb We have a ternary, basically if we're the last element in the
        // queue it'll be empty Thus causing an error, so we make head the same
        // as copy_pcb if that's the case
        head = (readyQueue.head != NULL) ? readyQueue.head : copy_pcb;
        int last_index = copy_pcb->number_of_lines;
        while (copy_pcb->pid != -100 && (copy_pcb->pc < last_index) &&
               (copy_pcb->job_length_score <= head->job_length_score)) {
            sprintf(key, "%d_%d", copy_pcb->pid, copy_pcb->pc);
            errCode = executeInstruction(key);
            copy_pcb->pc++;
            ageReadyQueue();
        }

        if (copy_pcb->pc == copy_pcb->number_of_lines) {
            freePCB(copy_pcb);
        } else {
            enqueue(copy_pcb);
            selectionSortQueue();
        }
    } while (readyQueue.head != NULL);
    return errCode;
}

void *worker_execute_RR(void *count_arg) {
    int count = *((int *)count_arg);
    struct PCB *copy_pcb;
    char key[KEY_SIZE];

    while (readyQueue.head != NULL) {
        pthread_mutex_lock(&mutex);
        copy_pcb = dequeue();
        pthread_mutex_unlock(&mutex);

        if (copy_pcb->pid == -100) {
            executeBackgroundTask(count);
        }

        int last_index = copy_pcb->pc + count;
        while (copy_pcb->pid != -100 && copy_pcb->pc < last_index &&
               copy_pcb->pc < copy_pcb->number_of_lines) {
            sprintf(key, "%d_%d", copy_pcb->pid, copy_pcb->pc);
            executeInstruction(key);
            copy_pcb->pc++;
        }

        pthread_mutex_lock(&mutex);
        if (copy_pcb->pid != 100 && copy_pcb->pc != copy_pcb->number_of_lines) {
            enqueue(copy_pcb);
        } else {
            // If the process block ends or if quit method is called
            // threads must be joined
            freePCB(copy_pcb);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void executeRR(int count) {
    if (isMultithreadingMode) {
        int *count_arg = &count;
        pthread_mutex_init(&mutex, NULL);
        pthread_create(&thread1, NULL, worker_execute_RR, count_arg);
        pthread_create(&thread2, NULL, worker_execute_RR, count_arg);

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
        pthread_mutex_destroy(&mutex);

    } else {
        struct PCB *copy_pcb;
        char key[KEY_SIZE];
        int errCode;

        while (readyQueue.head != NULL) {
            copy_pcb = dequeue();
            int last_index = copy_pcb->pc + count;

            if (copy_pcb->pid == -100) {
                executeBackgroundTask(count);
            }

            // The condition makes sure that 1 - only a predetermined amount of
            // instruction is run 2 - if it reaches the end, the loop stops and
            // 3 - not a background execution
            while (copy_pcb->pid != -100 && copy_pcb->pc < last_index &&
                   copy_pcb->pc < copy_pcb->number_of_lines) {
                sprintf(key, "%d_%d", copy_pcb->pid, copy_pcb->pc);
                executeInstruction(key);
                copy_pcb->pc++;
            }

            if (copy_pcb->pid != -100 &&
                copy_pcb->pc == copy_pcb->number_of_lines) {
                freePCB(copy_pcb);
            } else {
                enqueue(copy_pcb);
            }
        }
    }
}

void executeBackgroundTask(int count) {
    int i = 0;
    int errCode;
    char userInput[MAX_USER_INPUT];
    char *tok;
    while (i < count && fgets(userInput, MAX_USER_INPUT - 1, stdin) != NULL) {
        for (tok = strtok(userInput, ";"); tok != NULL;
             tok = strtok(NULL, ";")) {
            errCode = parseInput(tok);
            if (errCode == -1)
                return;
            memset(userInput, 0, sizeof(userInput));
        }
        i++;
    }
}