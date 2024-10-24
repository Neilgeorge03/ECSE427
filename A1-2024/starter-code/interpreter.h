// Global variable to keep track of in both interpreter.c and scheduler.c
extern int isMultithreadingMode;

int interpreter(char *command_args[], int args_size);
int help();
void runBackground();