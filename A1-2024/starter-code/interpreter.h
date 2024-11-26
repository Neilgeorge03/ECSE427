#ifndef INTERPRETER_H
#define INTERPRETER_H

#define BACKING_STORE "backingStore"

int interpreter(char *command_args[], int args_size);
int help();
void runBackground();

#endif