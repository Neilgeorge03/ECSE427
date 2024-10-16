#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shellmemory.h"
#include "shell.h"

struct memory_struct {
    char *var;
    char *value;
};

struct memory_struct shellmemory[MEM_SIZE];


// Helper functions
int match(char *model, char *var) {
    int i, len = strlen(var), matchCount = 0;
    for (i = 0; i < len; i++) {
        if (model[i] == var[i]) matchCount++;
    }
    if (matchCount == len) {
        return 1;
    } else return 0;
}

// Shell memory functions

void mem_init(){
    int i;
    for (i = 0; i < MEM_SIZE; i++){		
        shellmemory[i].var   = "none";
        shellmemory[i].value = "none";
    }
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++){
        if (strcmp(shellmemory[i].var, var_in) == 0){
            shellmemory[i].value = strdup(value_in);
            return;
        } 
    }

    //Value does not exist, need to find a free spot.
    for (i = 0; i < MEM_SIZE; i++){
        if (strcmp(shellmemory[i].var, "none") == 0){
            shellmemory[i].var   = strdup(var_in);
            shellmemory[i].value = strdup(value_in);
            return;
        } 
    }

    return;
}

//get value based on input key
char *mem_get_value(char *var_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++){
        if (strcmp(shellmemory[i].var, var_in) == 0){
            return strdup(shellmemory[i].value);
        } 
    }
    return "Variable does not exist";
}

int load_script_in_memory(FILE *fp, int pid) {
    char line[MAX_USER_INPUT];
    char key[15];
    int current_line_num = 0;
    memset(line, 0, MAX_USER_INPUT);

    while(fgets(line, MAX_USER_INPUT - 1, fp)) {
        sprintf(key, "%d_%d", pid, current_line_num);
        mem_set_value(key, line);

        memset(line, 0, MAX_USER_INPUT);
        memset(key, 0, sizeof(key));
        current_line_num++;

        if(feof(fp)) break;
    }
    return current_line_num;
}

// If successfully cleared => return 0. Otherwise return 1.
int clear_mem(int pid, int length) {
    char key[15];
    // to get the first occurence
    sprintf(key, "%d_%d", pid, 0);

    for (int i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, key) == 0) {
            for (int j = 0; j < length; j++) {
                shellmemory[i + j].var = "none";
                shellmemory[i + j].value = "none";
            }
            return 0;
        }
    }
    return 1;
}