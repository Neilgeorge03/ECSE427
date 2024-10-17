#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>
#include "shellmemory.h"
#include "shell.h"
#include "scheduler.h"

int UNIQUE_PID = 0;

int my_ls_filter(const struct dirent *namelist) {
    const char *name = namelist->d_name; 
    // return 0 => exclude 
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        return 0;
    }
    return 1;
}

/*
 * Sorts strings based on following priorities:
 * - Alphabetically 
 * - Numbers before letters
 * - Uppercase before lowercase
 * 
 * namelist_one before namelist_two   -> return -1
 * namelist_two before namelist_one   -> return  1
 * namelist_one equal to namelist_two -> return  0
 */
int my_ls_sort(const struct dirent **namelist_one, const struct dirent **namelist_two) {
    const char *name_one = (*namelist_one)->d_name;
    const char *name_two = (*namelist_two)->d_name;

    while (*name_one && *name_two) {
        if (isdigit(*name_one) && !isdigit(*name_two)) {
            return -1;
        }
        
        if (!isdigit(*name_one) && isdigit(*name_two)) {
            return 1;
        }
        
        if (tolower(*name_one) != tolower(*name_two)) {
            return tolower(*name_one) - tolower(*name_two);
        }

        // char in name_one == char in name_two or both are digits => find difference
        if (*name_one != *name_two) {
            return *name_one - *name_two;
        }

        // check next character if first characters are identical 
        name_one++;
        name_two++;
    }

    return 0; 
}

int check_alphanum(char *name) {
    for (int i = 0; i < strlen(name); i++) {
        if(!isalnum(name[i])) return 3;
    } 
    return 0;
}

int generate_pid(){
    return UNIQUE_PID++;
}

int execute_instruction(char *key) {
    int errCode = 0;
    char *instruction = mem_get_value(key);
    char *tok;
    for (tok = strtok(instruction, ";"); tok != NULL; tok = strtok(NULL, ";")) {
        errCode = parseInput(tok);
        if (errCode == -1) return 99;
    }
    return 0; 
}


int is_proper_policy(char *policy) {
    for(int i = 0; i < 4; i++) {
        if(strcasecmp(policy, ExecutionPolicy[i]) == 0) {
            return 0;
        }
    }
    return 1;
}