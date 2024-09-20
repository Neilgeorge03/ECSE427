#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <dirent.h>
#include "shellmemory.h"
#include "shell.h"
#include "helpers.h"

int MAX_ARGS_SIZE = 1000;

int badcommand(){
    printf("Unknown Command\n");
    return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
    printf("Bad command: File not found\n");
    return 3;
}

int help();
int quit();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int my_ls();
int badcommandFileDoesNotExist();

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
        //help
        if (args_size != 1) return badcommand();
        return help();
    
    } else if (strcmp(command_args[0], "quit") == 0) {
        //quit
        if (args_size != 1) return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "set") == 0) {
        //set
	if (args_size < 3) return badcommand();
        if (args_size > 7) {
		printf("Bad command: Too many tokens\n");
		return 3;
	
	};
	char ans[100];
	strcpy(ans, command_args[2]);
	for (int i = 3; i < args_size; i++){
		strcat(ans, " ");	
		strcat(ans, command_args[i]);

	}	
        return set(command_args[1], ans);
    
    } else if (strcmp(command_args[0], "print") == 0) {
        if (args_size != 2) return badcommand();
        return print(command_args[1]);
    
    } else if (strcmp(command_args[0], "run") == 0) {
        if (args_size != 2) return badcommand();
        return run(command_args[1]);
    
    } else if (strcmp(command_args[0], "my_ls") == 0) {
        if (args_size != 1) return badcommand();
        return my_ls();

    } else return badcommand();
}

int my_ls() {
    struct dirent **namelist;
    int n, i; 

    n = scandir(".", &namelist, my_ls_filter, my_ls_sort);
    if (n < 0) {
        printf("scandir error");
        exit(EXIT_FAILURE);
    }

    i = 0;
    while(i < n) {
        printf("%s\n", namelist[i] -> d_name);
        free(namelist[i]); 
        i++;
    }
    free(namelist);

    return 0;
}

int help() {

    // note the literal tab characters here for alignment
    char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
    printf("%s\n", help_string);
    return 0;
}

int quit() {
    printf("Bye!\n");
    exit(0);
}

int set(char *var, char *value) {
    char *link = "=";

    /* PART 1: You might want to write code that looks something like this.
         You should look up documentation for strcpy and strcat.

    char buffer[MAX_USER_INPUT];
    strcpy(buffer, var);
    strcat(buffer, link);
    strcat(buffer, value);
    */

    char ans[100];
    
    mem_set_value(var, value);

    return 0;
}

int print(char *var) {
    printf("%s\n", mem_get_value(var)); 
    return 0;
}

int run(char *script) {
    int errCode = 0;
    char line[MAX_USER_INPUT];
    FILE *p = fopen(script, "rt");  // the program is in a file

    if (p == NULL) {
        return badcommandFileDoesNotExist();
    }

    fgets(line, MAX_USER_INPUT-1, p);
    while (1) {
        errCode = parseInput(line);	// which calls interpreter()
        memset(line, 0, sizeof(line));

        if (feof(p)) {
            break;
        }
        fgets(line, MAX_USER_INPUT-1, p);
    }

    fclose(p);

    return errCode;
}
