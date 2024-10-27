#include <pthread.h>

// Global variables to keep track of execution policy or if in MT mode.
extern const char *ExecutionPolicy[];
extern int isMultithreadingMode;
extern int isBackgroundMode;

// Global variables related to threads
extern pthread_t thread1;
extern pthread_t thread2;
extern pthread_mutex_t mutex; 

int execute_FCFS();
void execute_RR(int count);
int execute_AGING();