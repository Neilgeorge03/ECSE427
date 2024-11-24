#include "interpreter.h"
#include "helpers.h"
#include "pcb.h"
#include "scheduler.h"
#include "shell.h"
#include "shellmemory.h"
#include <dirent.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int MAX_ARGS_SIZE = 1000;
struct stat s;
char *policy;
int isQuitJoinThreads = 0;

int help();
int quit();
int my_touch(char *filename);
int my_mkdir(char *folder);
int my_cd(char *folder);
int print(char *var);
int run(char *script);
int my_ls();
int echo(char *word);
int my_ls();
int exec(char *arguments[], int argumentSize);
int set(char *arguments[], int argumentSize);
void addBackgroundCommands(char *command_args[], int argsLength);

int badcommand() {
    printf("Unknown Command\n");
    return 1;
}

// For run command only
int badcommandFileDoesNotExist() {
    printf("Bad command: File not found\n");
    return 3;
}

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size) {
    int i;
    if (args_size < 1 || args_size > MAX_ARGS_SIZE) {
        return badcommand();
    }

    for (i = 0; i < args_size; i++) { // terminate args at newlines
        command_args[i][strcspn(command_args[i], "\r\n")] = 0;
    }

    if (strcmp(command_args[0], "help") == 0) {
        if (args_size != 1)
            return badcommand();
        return help();

    } else if (strcmp(command_args[0], "quit") == 0) {
        if (args_size != 1)
            return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "my_touch") == 0) {
        if (args_size != 2)
            return badcommand();
        return my_touch(command_args[1]);

    } else if (strcmp(command_args[0], "set") == 0) {
        // set
        if (args_size < 3)
            return badcommand();
        if (args_size > 7) {
            printf("Bad command: Too many tokens\n");
            return 3;
        }
        return set(command_args, args_size);

    } else if (strcmp(command_args[0], "print") == 0) {
        if (args_size != 2)
            return badcommand();
        return print(command_args[1]);

    } else if (strcmp(command_args[0], "run") == 0) {
        if (args_size != 2)
            return badcommand();
        return run(command_args[1]);

    } else if (strcmp(command_args[0], "echo") == 0) {
        if (args_size != 2)
            return badcommand(); // Check if first character of string is a '$'
                                 // sign
        return echo(command_args[1]);

    } else if (strcmp(command_args[0], "my_mkdir") == 0) {
        if (args_size != 2)
            return badcommand();
        return my_mkdir(command_args[1]);

    } else if (strcmp(command_args[0], "my_cd") == 0) {
        if (args_size != 2)
            return badcommand();
        return my_cd(command_args[1]);

    } else if (strcmp(command_args[0], "my_ls") == 0) {
        if (args_size != 1)
            return badcommand();
        return my_ls();

    } else if (strcmp(command_args[0], "exec") == 0) {
        if (args_size > 6)
            return badcommand();

        return exec(command_args, args_size);

        // If none of the valid commands are executed and more than 1 token ->
        // Too many tokens
    } else if (args_size > 1) {
        printf("Bad command: Too many tokens\n");
        return 3;

    } else
        return badcommand();
}

int my_ls() {
    struct dirent **namelist;
    int no_of_entries, i;

    no_of_entries = scandir(".", &namelist, myLsFilter, myLsSort);
    if (no_of_entries < 0) {
        printf("scandir error");
        exit(EXIT_FAILURE);
    }

    i = 0;
    while (i < no_of_entries) {
        printf("%s\n", namelist[i]->d_name);
        free(namelist[i]);
        i++;
    }
    free(namelist);

    return 0;
}

int help() {

    // note the literal tab characters here for alignment
    char help_string[] = "COMMAND                 DESCRIPTION\n \
help\t\t\tDisplays all the commands\n \
quit\t\t\tExits / terminates the shell with “Bye!”\n \
set VAR STRING         Assigns a value to shell memory\n \
print VAR              Displays the STRING assigned to VAR\n \
run SCRIPT.TXT         Executes the file SCRIPT.TXT\n ";
    printf("%s\n", help_string);
    return 0;
}

int my_touch(char *filename) {

    // input validation
    if (checkAlphanum(filename) != 0) {
        printf("Bad command: my_touch\n");
        return 3;
    }

    FILE *pfile = fopen(filename, "w");
    if (pfile == NULL) {
        printf("Failed to open file\n");
        return -2;
    }

    fclose(pfile);
    return 0;
}

int quit() {
    // If it's in MT mode, and a "quit" has been called while
    // ready_queue is not empty => join the threads. The join and
    // the exit will be executed in the scheduler.c using the global
    // variable isQuitJoinThreads because if a pthread_join is called
    // here, it may create a deadlock between both join locations.
    if (isMultithreadingMode && readyQueue.head != NULL) {
        printf("Bye!\n");
        isQuitJoinThreads = 1; 
        return 0;
    }

    printf("Bye!\n");
    exit(0);
}

int my_mkdir(char *folder) {
    if (folder[0] == '$') {
        for (int i = 0; folder[i] != '\0'; i++) {
            folder[i] = folder[i + 1];
        }

        folder = mem_get_value(folder);
        if (strcmp(folder, "Variable does not exist") == 0) {
            printf("Bad command: my_mkdir\n");
            return 3;
        }
    }

    // input validation
    if (checkAlphanum(folder) != 0) {
        printf("Bad command: my_mkdir\n");
        return 3;
    }

    if (mkdir(folder, 0755) == 0)
        return 0;
    printf("Bad command: my_mkdir\n");
    return 3;
}

int my_cd(char *folder) {
    // input validation
    if (checkAlphanum(folder) != 0) {
        printf("Bad command: my_cd\n");
        return 3;
    }

    if (chdir(folder) == 0)
        return 0;
    printf("Bad command: my_cd\n");
    return 9;
}

int echo(char *word) {
    if (word[0] != '$') {
        printf("%s\n", word);
        return 0;
    }

    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = word[i + 1];
    }
    char *ans = mem_get_value(word);

    if (strcmp(ans, "Variable does not exist") == 0) {
        printf("\n");
        return 5;
    }
    printf("%s\n", ans);
    return 0;
}

int set(char *arguments[], int argumentSize) {
    char ans[100];
    strcpy(ans, arguments[2]);
    for (int i = 3; i < argumentSize; i++) {
        strcat(ans, " ");
        strcat(ans, arguments[i]);
    }
    mem_set_value(arguments[1], ans);

    return 0;
}

int print(char *var) {
    printf("%s\n", mem_get_value(var));
    return 0;
}

int exec(char *arguments[], int argumentSize) {
    // numOfOptionalSettings start at 1 and not 0 due to zero indexing. No other reason.
    int numOfOptionalSettings = 1;

    // MT [optional] will always be the last argument. If it is, then in cases
    // when there is another optional argument ("#"), we need to keep track of
    // the fact that there already exists an optional argument, and thus "#"
    // not the last one
    if (strcmp(arguments[argumentSize - numOfOptionalSettings], "MT") == 0) {
        isMultithreadingMode = 1;
        numOfOptionalSettings++;
    }

    if (strcmp(arguments[argumentSize - numOfOptionalSettings], "#") == 0) {
        // Creating empty PCB. This is a placeholder type of PCB that when
        // the the readyQueue reaches this background PCB with PID = -100 
        // (not possible to get -100 PID otherwise), it'll execute instructions
        // from stdin
        createBackgroundPCB();
        numOfOptionalSettings++;
    }

    policy = arguments[argumentSize - numOfOptionalSettings];
    if (isProperPolicy(policy) != 0) {
        printf("Not a proper policy.\n");
        return badcommand();
    }

    // Load files into Shell memory and create PCBs
    for (int i = 1; i < argumentSize - numOfOptionalSettings; i++) {
        FILE *fp = fopen(arguments[i], "rt");
        if (fp == NULL) {
            printf("Failed to open file.\n");
            return 3;
        }
        if (loadScriptSharedMemory(arguments[i]) == 1){
            createDuplicatePCB(arguments[i]);
        }else {
            struct pagingReturn *returnPage = loadScriptBackingStore(BACKING_STORE, arguments[i], fp);
            createPCB(fp, returnPage);
        }
        fclose(fp);
    }

    if (strcmp(policy, "FCFS") == 0) {
        executeFCFS();
    }

    // Main difference between FCFS and SJF is SJF has their nodes sorted while regular FCFS doesn't.
    // Once sorted they share the same logic
    else if (strcmp(policy, "SJF") == 0) {
        selectionSortQueue();
        executeFCFS();
    }

    else if (strcmp(policy, "AGING") == 0) {
        selectionSortQueue();
        executeAging();
    }

    // 2 -> Round Robin algorithm switching after 2 instructions
    else if (strcmp(policy, "RR") == 0) {
        executeRR(2);
    }

    // 30 -> Round Robin algorithm switching after 30 instructions
    else if (strcmp(policy, "RR30") == 0) {
        executeRR(30);
    }

    return 0;
}

int run(char *script) {
    FILE *fp = fopen(script, "rt");
    if (fp == NULL) {
        printf("Failed to open file.\n");
        return 3;
    }

    struct PCB *pcb = createPCB(fp);
    fclose(fp);

    // Just need to execute everything sequentially, thus FCFS is sufficient
    executeFCFS();

    return 0;
}