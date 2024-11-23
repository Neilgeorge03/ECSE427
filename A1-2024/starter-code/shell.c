#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int parseInput(char ui[]);
int is_interactive_mode();


int clearBackingStoreDirectory(const char *dirLocation) {
    if (chdir(dirLocation) != 0) {
        perror("Unable to change directory");
        return -1;
    }

    struct dirent **namelist;
    int no_of_entries, i;

    no_of_entries = scandir(".", &namelist, NULL, alphasort);
    if (no_of_entries < 0) {
        printf("scandir error");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < no_of_entries; i++) {
        // Skip "." and ".." entries
        if (strcmp(namelist[i]->d_name, ".") == 0 || strcmp(namelist[i]->d_name, "..") == 0) {
            free(namelist[i]);
            continue;
        }

        // Attempt to remove the file
        if (remove(namelist[i]->d_name) == 0) {
            printf("Removed: %s\n", namelist[i]->d_name);
        } else {
            printf("scandir error");
            return -1;
        }
        free(namelist[i]); // Free memory allocated by scandir
    }

    free(namelist); // Free the array of directory entries

    return 0;
}

// Function to create the backing store directory
int createBackingStoreDirectory(const char *dirLocation) {
    // Check if the directory already exists
    struct stat st = {0};
    if (stat(dirLocation, &st) == -1) {
        if (mkdir(dirLocation, 0777) == 0) {
            return 0;
        } else {
            return -1;
        }
    } else {
        // In case directory exists, we need to erase its contents
        clearBackingStoreDirectory(dir_path);
        return 0;
    }
}

// Delete the backing store when quitting
int deleteBackingStoreDirectory(const char *dirLocation) {
    if (clearBackingStoreDirectory(dirLocation) == -1){
        return -1;
    }
    if (rmdir(dirLocation) == 0) {
        return 0;
    } else {
        return -1;
    }
}






// Start of everything
int main(int argc, char *argv[]) {
    printf("Shell version 1.3 created September 2024\n\n");
    // help();

    char prompt = '$';              // Shell prompt
    char userInput[MAX_USER_INPUT]; // user's input stored here
    int errorCode = 0;              // zero means no error, default

    // init user input
    for (int i = 0; i < MAX_USER_INPUT; i++) {
        userInput[i] = '\0';
    }

    // init shell memory
    mem_init();
    createBackingStoreDirectory()
    while (1) {
        if (is_interactive_mode()) {
            printf("%c ", prompt);
        }

        // If no "quit" command is found in batch mode, check for EOF and exit
        if (fgets(userInput, MAX_USER_INPUT - 1, stdin) == NULL &&
            feof(stdin)) {
            exit(0);
        }

        // strcpy(userInput, "exec prog1.txt prog2.txt prog3.txt RR MT");
        for (char *token = strtok(userInput, ";"); token != NULL;
             token = strtok(NULL, ";")) {
            errorCode = parseInput(token);
            if (errorCode == -1)
                exit(99);
        }

        memset(userInput, 0, sizeof(userInput));
    }

    return 0;
}

int is_interactive_mode() {
    int file_desc_no = isatty(STDIN_FILENO);
    if (file_desc_no != 0 && file_desc_no != 1) {
        printf("Batch mode unavailable, ERRNO: %d\n", file_desc_no);
        exit(file_desc_no);
    }
    return file_desc_no;
}

int wordEnding(char c) {
    // You may want to add ';' to this at some point,
    // or you may want to find a different way to implement chains.
    return c == '\0' || c == '\n' || c == ' ';
}

int parseInput(char inp[]) {
    char tmp[200], *words[100];
    int ix = 0, w = 0;
    int wordlen;
    int errorCode;
    for (ix = 0; inp[ix] == ' ' && ix < 1000; ix++)
        ; // skip white spaces
    while (inp[ix] != '\n' && inp[ix] != '\0' && ix < 1000) {
        // extract a word
        for (wordlen = 0; !wordEnding(inp[ix]) && ix < 1000; ix++, wordlen++) {
            tmp[wordlen] = inp[ix];
        }
        tmp[wordlen] = '\0';
        words[w] = strdup(tmp);
        w++;
        if (inp[ix] == '\0')
            break;
        ix++;
    }
    errorCode = interpreter(words, w);

    // Freeing memory to avoid memory leaks
     for (int i = 0; i < w; i++) {
        free(words[i]);
    }

    return errorCode;
}
