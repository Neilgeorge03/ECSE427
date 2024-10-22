#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "shellmemory.h"
#include "shell.h"
#include "helpers.h"
#include "pcb.h"
#include "scheduler.h"

int MAX_ARGS_SIZE = 1000;
char* CURRENT_LOCATION = ".";
struct stat s;

int help();
int quit();
int my_touch(char *filename);
int my_mkdir(char *folder);
int my_cd(char *folder);
int print(char *var);
int run(char *script);
int my_ls();
int badcommandFileDoesNotExist();
int echo(char *word);
int my_ls();
int exec(char *arguments[], int argumentSize);
int set(char *arguments[], int argumentSize);

int badcommand(){
    printf("Unknown Command\n");
    return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
    printf("Bad command: File not found\n");
    return 3;
}

// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size) {
    int i;

    if (args_size < 1 || args_size > MAX_ARGS_SIZE) {
        return badcommand();
    }

    for (i = 0; i < args_size; i++) { // terminate args at newlines
        command_args[i][strcspn(command_args[i], "\r\n")] = 0;
    }

    if (strcmp(command_args[0], "help") == 0){
        if (args_size != 1) return badcommand();
        return help();

    } else if (strcmp(command_args[0], "quit") == 0) {
        if (args_size != 1) return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "my_touch") == 0) {
        if (args_size != 2) return badcommand();
        return my_touch(command_args[1]);

    } else if (strcmp(command_args[0], "set") == 0) {
        //set
        if (args_size < 3) return badcommand();
            if (args_size > 7) {
            printf("Bad command: Too many tokens\n");
            return 3;
        }
        return set(command_args, args_size);

    } else if (strcmp(command_args[0], "print") == 0) {
        if (args_size != 2) return badcommand();
        return print(command_args[1]);
    
    } else if (strcmp(command_args[0], "run") == 0) {
        if (args_size != 2) return badcommand();
        return run(command_args[1]);

    } else if (strcmp(command_args[0], "echo") == 0){
        if (args_size != 2) return badcommand();	// Check if first character of string is a '$' sign
        return echo(command_args[1]);

    } else if (strcmp(command_args[0], "my_mkdir") == 0) {
        if (args_size != 2) return badcommand();
        return my_mkdir(command_args[1]);

    } else if (strcmp(command_args[0], "my_cd") == 0) {
        if (args_size != 2) return badcommand();
        return my_cd(command_args[1]);

    } else if (strcmp(command_args[0], "my_ls") == 0) {
        if (args_size != 1) return badcommand();
        return my_ls();
    
    } else if (strcmp(command_args[0], "exec") == 0) {
        if (args_size > 5) return badcommand();
        return exec(command_args, args_size);
    
    // If none of the valid commands are executed and more than 1 token -> Too many tokens
    } else if (args_size > 1) {
        printf("Bad command: Too many tokens\n");
        return 3;

    } else return badcommand();
}

int my_ls() {
    struct dirent **namelist;
    int no_of_entries, i; 

    no_of_entries = scandir(".", &namelist, my_ls_filter, my_ls_sort);
    if (no_of_entries < 0) {
        printf("scandir error");
        exit(EXIT_FAILURE);
    }

    i = 0;
    while(i < no_of_entries) {
        printf("%s\n", namelist[i] -> d_name);
        free(namelist[i]); 
        i++;
    }
    free(namelist);

    return 0;
}

int help() {

    // note the literal tab characters here for alignment
    char help_string[] ="COMMAND                 DESCRIPTION\n \
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
    if (check_alphanum(filename) != 0) {
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
    printf("Bye!\n");
    exit(0);
}

int my_mkdir(char *folder){
    if (folder[0] == '$') {
        for (int i = 0;folder[i] != '\0'; i++) {
            folder[i] = folder[i + 1];
        }

        folder = mem_get_value(folder);
        if (strcmp(folder, "Variable does not exist") == 0){
        	printf("Bad command: my_mkdir\n");
		return 3;
        }
    }

    // input validation
    if (check_alphanum(folder) != 0) {
        printf("Bad command: my_mkdir\n");
        return 3;
    }

    if (mkdir(folder, 0755) == 0) return 0;
    printf("Bad command: my_mkdir\n");
    return 3;

}

int my_cd(char *folder){
    // input validation 
    if (check_alphanum(folder) != 0) {
        printf("Bad command: my_cd\n");
        return 3;
    }

    if (chdir(folder) == 0) return 0;
    printf("Bad command: my_cd\n");
    return 9;
}

int echo(char *word){
    if (word[0] != '$'){
        printf("%s\n", word);
        return 0;
    }

    for (int i = 0;word[i] != '\0'; i++) {
        word[i] = word[i + 1];
    }
    char* ans = mem_get_value(word);

    if (strcmp(ans, "Variable does not exist") == 0){
        printf("\n");
        return 5;
    }
    printf("%s\n", ans);
    return 0;
}

int set(char* arguments[], int argumentSize) {
    char ans[100];
    strcpy(ans, arguments[2]);
    for (int i = 3; i < argumentSize; i++){
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
    char *policy = arguments[argumentSize - 1];
    if (is_proper_policy(policy) != 0) {
        printf("Not a proper policy.\n");
        return badcommand();
    }
    for (int i = 1; i < argumentSize - 1; i++) {
            FILE *fp = fopen(arguments[i], "rt");
            if (fp == NULL) {
                printf("Failed to open file.\n");
                return 3;
            }
            create_pcb(fp);
            fclose(fp);
    }
    if (strcmp(policy, "FCFS") == 0) {
       execute_FCFS();
    }
    if (strcmp(policy, "SJF") == 0) {
        selectionSortQueue();
        execute_FCFS();

    } else if (strcmp(policy,"RR") == 0) {
        for (int i = 1; i < argumentSize - 1; i++) {
            FILE *fp = fopen(arguments[i], "rt");
            if (fp == NULL) {
                printf("Failed to open file.\n");
                return 3;
            }
            create_pcb(fp);
            fclose(fp);
        }
        execute_RR();
    }
    return 0;
}


int run(char *script) {
    FILE *fp = fopen(script, "rt");
    if (fp == NULL) {
        printf("Failed to open file.\n");
        return 3;
    }

    struct PCB *pcb = create_pcb(fp); 
    fclose(fp);

    execute_FCFS(); 

    return 0;
}