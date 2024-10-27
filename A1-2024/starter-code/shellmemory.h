#include <stdio.h>

#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H

#define MEM_SIZE 1000
#define MAX_SCRIPT_SIZE (MAX_USER_INPUT * 100)
#define KEY_SIZE 100

void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int loadScriptInMemory(FILE *fp, int pid);
int clearMemory(int pid, int length);

#endif