#include <pthread.h>

#ifndef SCHEDULER_H
#define SCHEDULER_H

// Global variables to keep track of execution policy or if in MT mode.
extern const char *ExecutionPolicy[];
extern int isMultithreadingMode;
extern int isQuitJoinThreads;

// Global variables related to threads
extern pthread_t thread1;
extern pthread_t thread2;
extern pthread_mutex_t mutex;

int executeFCFS();
void executeRR(int count);
int executeAging();

#endif