#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"
#include "pcb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>     // For directory operations (opendir, readdir, closedir)
#include <sys/stat.h>   // For mkdir
#include <sys/types.h>  // For types used by mkdir and other file-related operations

int fileCount = 0;
struct pagingFileTracker pageTracker[10];
int parseInput(char ui[]);
int is_interactive_mode();

// Start of everything
int main(int argc, char *argv[]) {
    printf("Frame Store Size = %d; Variable Store Size = %d\n", FRAME_STORE_SIZE, VARIABLE_STORE_SIZE);
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
    initBackingStore();
    initFrameStore();
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
int parseInputFrameStore(char* line) {
    int errorCode;
    for (char *token = strtok(line, ";"); token != NULL;
         token = strtok(NULL, ";")) {
        errorCode = parseInput(token);
        if (errorCode == -1)
            exit(99);
    }
    return errorCode;
}


void initBackingStore(){
    DIR *dir = opendir(BACKING_STORE);
    if (dir){
        // means backing store exists
        struct dirent *dirEntry;
        char filePath[100];
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
struct pagingReturn *loadScriptBackingStore(char *dirName, char *scriptName, FILE *fp) {
    int page = 0;
    int lineCount = 0;
    int offset = 0;
    int frameIndex = -1;
    int pageTableIndex = 0;
    char line[MAX_USER_INPUT];
    char filePath[MAX_USER_INPUT];
    FILE *backingStoreFile = NULL;

    // Allocate memory for pagingReturn
    struct pagingReturn *pageReturn = malloc(sizeof(struct pagingReturn));
    if (pageReturn == NULL) {
        printf("Error: Memory allocation for pagingReturn failed\n");
        return NULL;
    }
    memset(pageReturn, 0, sizeof(struct pagingReturn));

    while (fgets(line, sizeof(line), fp)) {
        if (lineCount % FRAME_SIZE == 0) {  // New page logic
            offset = 0;
            frameIndex = getFreeFrame();
            int maxPages = MAX_PAGES;
            if (backingStoreFile != NULL) fclose(backingStoreFile);

            // Create a new backing store file for this page
            snprintf(filePath, sizeof(filePath), "%s/%s_page%d", dirName, scriptName, page);
            backingStoreFile = fopen(filePath, "w");
            if (backingStoreFile == NULL) {
                printf("Error: Failed to create backing store file\n");
                free(pageReturn); // Clean up allocated memory
                return NULL;
            }

            if (page >= maxPages) {  // Check bounds for page table
                printf("Error: Page table overflow\n");
                free(pageReturn);
                fclose(backingStoreFile);
                return NULL;
            }

//            if (frameIndex == -1) {
//                printf("No free frame available\n");
//                free(pageReturn); // Clean up allocated memory
//                if (backingStoreFile != NULL) fclose(backingStoreFile);
//                return NULL;
//            }
            if (page > 1 || frameIndex == -1) {
                pageReturn->pageTable[page] = -1;
            } else { // Add frameIndex to page table
                pageReturn->pageTable[page] = frameIndex;
            }
            page++;
        }

        // Write to backing store file
        fprintf(backingStoreFile, "%s", line);
        // Load the page into the frame store
        loadPageFrameStore(frameIndex * FRAME_SIZE + offset, filePath);
        if (page > 2){
            deleteFrame(frameIndex);
        } else if (offset == 0){
            addTailDemandQueue(frameIndex, filePath);
        }
        offset++;
        lineCount++;
    }
    // Close the final file
    if (backingStoreFile != NULL) fclose(backingStoreFile);
    // Populate the pagingReturn structure
    pageReturn->numberLines = lineCount;

    return pageReturn;
}


int findFileIndex(const char *filename) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (strcmp(pageTracker[i].filename, filename) == 0) {
            return i;  // Return the index if found
        }
    }
    return -1;  // File not found
}
struct pagingReturn* getPageInfo(int index) {
    return pageTracker[index].pageData;
}

int addFileToPagingArray(struct pagingReturn* page, char *filename) {
    if (fileCount >= MAX_FILES) {
        printf("Error: Maximum file limit reached.\n");
        return -1;
    }


    for (int i = 0; i < fileCount; i++){
        if (strcmp(pageTracker[i].filename, filename) == 0){
            return -1;
        }
    }
    strcpy(pageTracker[fileCount].filename, filename);
    pageTracker[fileCount].pageData = page;
    fileCount++;
    return 0;
}


void removePageInfo(char* filename, int removeIndex){
    int index = findFileIndex(filename);
    pageTracker[index].pageData->pageTable[removeIndex] = -1;
}

struct PCB* updatePageInfo(struct PCB* pcb, char* filename, int pageTableIndex, int frameStoreIndex){
    int index = findFileIndex(filename);
    char newFilename[256];
    pageTracker[index].pageData->pageTable[pageTableIndex] = frameStoreIndex;
    addTailDemandQueue(frameStoreIndex, filename);
    for (int offset = 0; offset < FRAME_SIZE && pcb->pc+offset < pcb->number_of_lines; offset++){
        snprintf(newFilename, sizeof(newFilename), "%s/%s_page%d", BACKING_STORE, filename, (pcb->pc/FRAME_SIZE));
        loadPageFrameStore(frameStoreIndex * FRAME_SIZE + offset, newFilename);
    }
    return updatePCB(pcb, filename);
}

struct PCB* updatePCB(struct PCB* pcb, char *filename){
    struct PCB* current = getPCBHead();
    int index = findFileIndex(filename);

    while(current != NULL){
        if (strcmp(filename, current->scriptName) == 0){
            memcpy(current->pageTable, &pageTracker[index], sizeof(pageTracker[index]));
            return current;
        }
        current = current->next;
    }
    memcpy(pcb->pageTable, pageTracker[index].pageData->pageTable, sizeof(pcb->pageTable));
    return pcb;
}