#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int parseInput(char ui[]);
int is_interactive_mode();

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





void initBackingStore(){
    char *filePath;
    DIR *dir = opendir(BACKING_STORE);
    if (dir){
        // means backing store exists
        struct dirent *dirEntry;
        while ((dirEntry = readdir(dir)) != NULL){
            if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0){
                snprintf(filePath, sizeof(filePath), "%s/%s", BACKING_STORE, dirEntry->d_name);
                remove(filePath);
            }
        }
        closedir(dir);
    } else {
        mkdir(BACKING_STORE, 0777);
    }
}

void delBackingStore(){
    char *filePath;
    DIR *dir = opendir(BACKING_STORE);
    if (dir){
        // means backing store exists
        struct dirent *dirEntry;
        while ((dirEntry = readdir(dir)) != NULL){
            if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0){
                snprintf(filePath, sizeof(filePath), "%s/%s", BACKING_STORE, dirEntry->d_name);
                remove(filePath);
            }
        }
        closedir(dir);
    }
    rmdir(BACKING_STORE);
}

// TODO
struct pagingReturn *loadScriptBackingStore(char* dirName, char* scriptName, FILE* fp){
    int page = 0;
    int lineCount = 0;
    char line[MAX_USER_INPUT];
    char filePath[MAX_USER_INPUT];
    char fileName[MAX_USER_INPUT];
    FILE* backingStoreFile = NULL;
    int offset;
    int frameIndex = -1;
    struct pagingReturn *pageReturn;
    int pageTableIndex = 0;

    while (fgets(line, MAX_USER_INPUT - 1, fp)) {
        if (lineCount % 3 == 0){
            offset = 0;
            frameIndex = getFreeFrame();

            if (frameIndex == -1){
                printf("No free frame available\n");
                return;
            }
            if (backingStoreFile != NULL){
                fclose(backingStoreFile);
            }

            sprintf(filePath, "%s/%s_page%d", dirName, scriptName, page);

            backingStoreFile = fopen(filePath, "w");

            if (backingStoreFile == NULL){
                printf("Error: failed to create backing store file");
                return;
            }

            pageReturn->pageTable[pageTableIndex++] = frameIndex;
            page++;
        }
        fprintf(backingStoreFile, "%s", line);
        loadPageFrameStore(frameIndex * FRAME_SIZE + offset++, filePath);
        lineCount++;

    }
    if (backingStoreFile != NULL) {
        fclose(backingStoreFile);
    }
    pageReturn->numberLines=lineCount;
    return pageReturn;
}
