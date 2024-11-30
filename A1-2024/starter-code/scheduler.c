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
int isMultithreadingMode = 0;
pthread_t thread1, thread2;
pthread_mutex_t mutex;

void executeBackgroundInstruction(int count);

int executeFCFS() {
    int errCode;
    struct PCB *copyPCB;
    char key[KEY_SIZE];
    int frameIndex;
    int offset;
    int pageNumber;
    int LRUIndex;

    do {
        copyPCB = dequeue();

        if (copyPCB->pid == -100) {
            // 1000 is an arbitrary large number to ensure all instructions
            // possible in stdin are executed. If it reaches the eof, it'll
            // simply stop executing furhter.
            executeBackgroundInstruction(1000);
        }
        // pc here refers to "program counter"
        while (copyPCB->pid != -100 && copyPCB->pc < copyPCB->number_of_lines) {
            pageNumber = (copyPCB->pc / FRAME_SIZE);
            offset = (copyPCB->pc % FRAME_SIZE);
            if (copyPCB->pageTable[pageNumber] == -1) {
                copyPCB = handlePageFault(copyPCB, pageNumber);
//                break;
            }
            LRUIndex = removeDemandQueue(copyPCB->pageTable[pageNumber]);
            frameIndex = copyPCB->pageTable[pageNumber];
            executePagingInstruction(frameIndex, offset);
            addTailDemandQueue(LRUIndex, copyPCB->scriptName);
            copyPCB->pc++;
        }
        // Important to free to remove instrcutions from shell memory
        // and PCB from computer memory
    } while (readyQueue.head != NULL);
    return errCode;
}

int executeAging() {
    int errCode;
    struct PCB *copyPCB, *head;
    char key[KEY_SIZE];

    do {
        // Do the jobs
        copyPCB = dequeue();

        if (copyPCB->pid == -100) {
            // 1000 is an arbitrary large number to ensure all instructions
            // possible in stdin are executed. If it reaches the eof, it'll
            // simply stop executing furhter.
            executeBackgroundInstruction(1000);
        }
        // Check if head exists, if it does then we can make head as readyQueue
        // If it doesn't exist then we can use the current node as to compare
        // further down in the program
        head = (readyQueue.head != NULL) ? readyQueue.head : copyPCB;

        int last_index = copyPCB->number_of_lines;
        while (copyPCB->pid != -100 && (copyPCB->pc < last_index) &&
               (copyPCB->job_length_score <= head->job_length_score)) {
            // Itereate through the commands of the PCB
            // we check if the current head of the queue and the current
            // dequeued should switch by comparing the score
            sprintf(key, "%d_%d", copyPCB->pid, copyPCB->pc);
            // Execute code
            errCode = executeInstruction(key);
            copyPCB->pc++;
            // age the queue elements
            ageReadyQueue();
        }

        if (copyPCB->pc == copyPCB->number_of_lines) {
            // If completed we can free the PCB
            freePCB(copyPCB);
        } else {
            // We need to re-enqueue the PCB and sort it again to make sure it's
            // in ascending order
            enqueue(copyPCB);
            selectionSortQueue();
        }
    } while (readyQueue.head != NULL);
    return errCode;
}

// Multithreaded worker function for executeRR
void *workerExecuteRR(void *count_arg) {
    int count = *((int *)count_arg);
    struct PCB *copyPCB;
    char key[KEY_SIZE];

    while (readyQueue.head != NULL) {
        // readyQueue is a shared ressource -> make sure dequeue
        // enqueue happen once at a time otherwise may pick end up with
        // unexpected behaviour.
        pthread_mutex_lock(&mutex);
        copyPCB = dequeue();
        pthread_mutex_unlock(&mutex);

        if (copyPCB->pid == -100) {
            executeBackgroundInstruction(count);
        }

        int last_index = copyPCB->pc + count;
        while (copyPCB->pid != -100 && copyPCB->pc < last_index &&
               copyPCB->pc < copyPCB->number_of_lines) {
            sprintf(key, "%d_%d", copyPCB->pid, copyPCB->pc);
            executeInstruction(key);
            copyPCB->pc++;
        }

        pthread_mutex_lock(&mutex);
        if (copyPCB->pid != 100 && copyPCB->pc != copyPCB->number_of_lines) {
            enqueue(copyPCB);
        } else {
            freePCB(copyPCB);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void executeRR(int count) {
    struct PCB *copyPCB;
    int frameIndex;
    int offset;
    int pageNumber;
    int LRUIndex;

    if (isMultithreadingMode) {
        int *count_arg = &count;
        pthread_mutex_init(&mutex, NULL);
        pthread_create(&thread1, NULL, workerExecuteRR, count_arg);
        pthread_create(&thread2, NULL, workerExecuteRR, count_arg);

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
        pthread_mutex_destroy(&mutex);

        if (isQuitJoinThreads == 1) {
            // In case not all PCB has been cleared (such as the background PCB)
            while (readyQueue.head != NULL) {
                struct PCB *pcb = readyQueue.head;
                readyQueue.head = pcb->next;
                freePCB(pcb);
            }

            // Exit is called here because if all threads are terminated
            // and quit was called, it'll exit successfully here.
            exit(0);
        }
    }

    // Keep going until readyQueue is empty
    while (readyQueue.head != NULL) {
        copyPCB = dequeue();
        int last_index = copyPCB->pc + count;

        // Placeholder PCB reached -> execute background tasks
        if (copyPCB->pid == -100) {
            executeBackgroundInstruction(count);
        }

        // The condition makes sure that:
        // 1 - only a predetermined amount of instructions is run
        // 2 - if it reaches the end, the loop stops and
        // 3 - not a background PCB
        while (copyPCB->pid != -100 && copyPCB->pc < last_index &&
               copyPCB->pc < copyPCB->number_of_lines) {
            pageNumber = (copyPCB->pc / FRAME_SIZE);
            offset = (copyPCB->pc % FRAME_SIZE);
            if (copyPCB->pageTable[pageNumber] == -1) {
                copyPCB = handlePageFault(copyPCB, pageNumber);
                break;
            }
            // Does LRU page replacement, removes the page from the LRU queue
            // and executes the instruction, and re-adds the page to the queue
            // as most recently used and advances the pc. 
            LRUIndex = removeDemandQueue(copyPCB->pageTable[pageNumber]);
            frameIndex = copyPCB->pageTable[pageNumber];
            executePagingInstruction(frameIndex, offset);
            addTailDemandQueue(LRUIndex, copyPCB->scriptName);
            copyPCB->pc++;
        }

        if (copyPCB->pid != -100 && copyPCB->pc == copyPCB->number_of_lines) {
            freePCB(copyPCB);
        } else {
            enqueue(copyPCB);
        }
    }

    // In case not all PCBs has been cleared (such as the background PCB)
    while (readyQueue.head != NULL) {
        struct PCB *pcb = readyQueue.head;
        readyQueue.head = pcb->next;
        freePCB(pcb);
    }
}

// If the BackgroundPCB is being executed, then we want the commands
// to come from STDIN for the number of instructions the RR execution
// policy is asking.
void executeBackgroundInstruction(int count) {
    int errCode;
    char userInput[MAX_USER_INPUT];
    for (int i = 0; i < count; i++) {
        // If fgets returns NULL -> either no more lines, or EOF -> stop
        if (fgets(userInput, MAX_USER_INPUT - 1, stdin) == NULL)
            break;

        errCode = parseInput(userInput);
        if (errCode == -1)
            return;
    }
}