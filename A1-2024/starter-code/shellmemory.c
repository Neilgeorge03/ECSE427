#include "shellmemory.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int processCount = 0;


struct memory_struct shellmemory[MEM_SIZE];
struct memory_struct variableStore[VARIABLE_STORE_SIZE];
struct sharedProcess *processTable[MAX_PROCESSES];
char frameStore[FRAME_STORE_SIZE][100];

// Helper functions
int match(char *model, char *var) {
    int i, len = strlen(var), matchCount = 0;
    for (i = 0; i < len; i++) {
        if (model[i] == var[i])
            matchCount++;
    }
    if (matchCount == len) {
        return 1;
    } else
        return 0;
}

// Shell memory functions

void mem_init() {
    int i;
    for (i = 0; i < MEM_SIZE; i++) {
        shellmemory[i].var = "none";
        shellmemory[i].value = "none";
    }
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, var_in) == 0) {
            shellmemory[i].value = strdup(value_in);
            return;
        }
    }

    // Value does not exist, need to find a free spot.
    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, "none") == 0) {
            shellmemory[i].var = strdup(var_in);
            shellmemory[i].value = strdup(value_in);
            return;
        }
    }
    return;
}

void variable_set_value(char *var_in, char *value_in) {
    int i;

    for (i = 0; i < VARIABLE_STORE_SIZE ;
    i++) {
        if (strcmp(variableStore[i].var, var_in) == 0) {
            variableStore[i].value = strdup(value_in);
            return;
        }
    }

    // Value does not exist, need to find a free spot.
    for (i = 0; i < VARIABLE_STORE_SIZE ;
    i++) {
        if (strcmp(variableStore[i].var, "none") == 0) {
            variableStore[i].var = strdup(var_in);
            variableStore[i].value = strdup(value_in);
            return;
        }
    }
    return;
}


// get value based on input key
char *mem_get_value(char *var_in) {
    int i;
    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, var_in) == 0) {
            return strdup(shellmemory[i].value);
        }
    }
    return "Variable does not exist";
}

char *variable_get_value(char *var_in) {
    int i;
    for (i = 0; i < VARIABLE_STORE_SIZE; i++) {
        if (strcmp(variableStore[i].var, var_in) == 0) {
            return strdup(variableStore[i].value);
        }
    }
    return "Variable does not exist";
}

// Loads script (pointer at by fp) into shell memory
// returns the number of lines that it has stored using 
// the key "pid_%d" when %d is [0, num of instructions)
int loadScriptInMemory(FILE *fp, int pid) {
    char line[MAX_USER_INPUT];
    char key[KEY_SIZE];
    int current_line_num = 0;
    memset(line, 0, MAX_USER_INPUT);

    while (fgets(line, MAX_USER_INPUT - 1, fp)) {
        sprintf(key, "%d_%d", pid, current_line_num);
        variable_set_value(key, line);

        memset(line, 0, MAX_USER_INPUT);
        memset(key, 0, sizeof(key));
        current_line_num++;

        if (feof(fp))
            break;
    }
    return current_line_num;
}

// If successfully cleared => return 0. Otherwise return 1.
int clearMemory(int pid, int length) {
    char key[KEY_SIZE];
    char key1[KEY_SIZE];
    // to get the first occurence
    sprintf(key, "%d_%d", pid, 0);

    for (int i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, key) == 0) {
            // Suppose we have a variable between some of the PCB instructions
            // We keep track of the lineNumber and check if the var corresponds
            // to the actual command and not a variable Thus in the for loop we
            // check either if pointer is reached its limit or if we've reached
            // the end of memory space We can start at i since the first
            // instance is always first so we don't have to restart at 0 again
            int lineNumber = 0;
            for (int j = i; j < MEM_SIZE && lineNumber < length; j++) {
                sprintf(key1, "%d_%d", pid, lineNumber);
                if (strcmp(shellmemory[i].var, key1) == 0) {
                    shellmemory[j].var = "none";
                    shellmemory[j].value = "none";
                    lineNumber++;
                }
            }
            return 0;
        }
    }
    return 1;
}



int checkScriptLoaded(char *scriptName){
    for (int i = 0; i < processCount; i++){
        if (strcmp(processTable[i]->processName, scriptName) == 0){
            return i;
        }
    }
    return -1;
}

int loadScriptSharedMemory(char *scriptName){
    int index = checkScriptLoaded(scriptName);
    if (index != -1){
        processTable[index]->count++;
        return 1;
    }

    if (processCount > MAX_PROCESSES){
        printf("Process table is full, can't add any new processes.\n");
        return -1;
    }
    struct sharedProcess* newSharedProcess = malloc(sizeof(struct sharedProcess));
    if (newSharedProcess == NULL) {
        printf("Memory allocation failed for new process.\n");
        return -1;
    }
    strcpy(newSharedProcess->processName, scriptName);
    newSharedProcess->count = 1;

    processTable[processCount++] = newSharedProcess;
    return 0;
}

int removeScriptSharedMemory(char *scriptName){
    int index = checkScriptLoaded(scriptName);
    if (index == -1){
        printf("Can't remove script doesn't exist\n");
        return -1;
    }
    processTable[index]->count--;
//    if (processTable[index]->count == 0){
//        strcpy(processTable[index]->processName, "");
//        return 1;
//    }
    return 0;
}

void initFrameStore(){
    for (int i = 0; i < FRAME_STORE_SIZE; i++){
        strcpy(frameStore[i], "");
    }
}

int getFreeFrame(){
    for (int i = 0; i < (FRAME_STORE_SIZE/FRAME_SIZE); i++){
        if (strcmp(frameStore[i*FRAME_SIZE], "") == 0){
            return i;
        }
    }
    return -1;
}

void deleteFrame(int frameIndex){
    if (frameIndex < 0 || frameIndex > (FRAME_STORE_SIZE/FRAME_SIZE)){
        printf("Error: line index incorrect\n");
        return;
    }
    for (int i = 0; i < FRAME_SIZE; i++){
        if (frameStore[frameIndex * FRAME_SIZE + i] != NULL) {
            strcpy(frameStore[frameIndex * FRAME_SIZE + i], "");
        }
    }
}

char* getLine(int frameIndex, int offset){
    if (frameIndex < 0 || frameIndex > (FRAME_STORE_SIZE/FRAME_SIZE)){
        printf("Error: line index incorrect\n");
        return NULL;
    } else if (offset < 0 || offset > FRAME_SIZE){
        printf("Error: offset incorrect\n");
        return NULL;
    }
    return frameStore[frameIndex * FRAME_SIZE + offset];
}





// TODO
// Load page into frame store,
void loadPageFrameStore(int index, char* fileName){
    strcpy(frameStore[index], fileName);
}