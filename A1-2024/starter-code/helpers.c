#include "scheduler.h"
#include "shell.h"
#include "pcb.h"
#include "shellmemory.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

int UNIQUE_PID = 0;
int evictPage();

int myLsFilter(const struct dirent *namelist) {
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
int myLsSort(const struct dirent **namelist_one,
               const struct dirent **namelist_two) {
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

        // char in name_one == char in name_two or both are digits => find
        // difference
        if (*name_one != *name_two) {
            return *name_one - *name_two;
        }

        // check next character if first characters are identical
        name_one++;
        name_two++;
    }

    return 0;
}

int checkAlphanum(char *name) {
    for (int i = 0; i < strlen(name); i++) {
        if (!isalnum(name[i]))
            return 3;
    }
    return 0;
}

// Generate PID going from 0 to 2^(bit of system)
int generatePID() { return UNIQUE_PID++; }

int executeInstruction(char *key) {
    char *instruction = mem_get_value(key);
    int errCode = parseInput(instruction);
    if (errCode == -1) {
        printf("Fatal error during instruction execution occured.\n");
        return -1;
    }
    return 0;
}
int executePagingInstruction(int index, int offset) {
    char* fileName = getLine(index, offset);
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL){
        printf("Can't open up page\n");
        return -1;
    }
    char line[100]; // Buffer for the line
    int currentLine = 0;

    // Read lines sequentially
    while (fgets(line, sizeof(line), fp)) {
        if (currentLine == offset) {
            fclose(fp);  // Close the file before returning
            int errCode = parseInputFrameStore(line);
            if (errCode == -1) {
                printf("Fatal error during instruction execution occured.\n");
                return -1;
            }
            return 0;

        }
        currentLine++;
    }
    fclose(fp);
    printf("Couldn't find offset %d in file %s\n", offset, fileName);
    return -1;
}

int isProperPolicy(char *policy) {
    for (int i = 0; i < 5; i++) {
        if (strcasecmp(policy, ExecutionPolicy[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

void loadPageOnDemand(struct pagingReturn *pageReturn, int pageNumber, char *scriptName) {
    if (pageReturn->pageTable[pageNumber] != -1) {
        return; // Page is already loaded or invalid page number
    }

    int frameIndex = getFreeFrame();
    if (frameIndex == -1) {
        frameIndex = evictPage();
    }

    char filePath[MAX_USER_INPUT];
    snprintf(filePath, sizeof(filePath), "%s/%s_page%d", BACKING_STORE, scriptName, pageNumber);
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) {
        printf("Error: Cannot load page %d for %s.\n", pageNumber, scriptName);
        return;
    }

    char line[MAX_USER_INPUT];
    for (int i = 0; i < FRAME_SIZE && fgets(line, sizeof(line), fp); i++) {
        strcpy(frameStore[frameIndex * FRAME_SIZE + i], line);
    }
    fclose(fp);

    pageReturn->pageTable[pageNumber] = frameIndex; // Update page table
}

int evictPage() {
    // LRU Frame
    int victimFrame = removeDemandHead();
    printf("Page fault! Victim page contents:\n\n");
    FILE *fp = fopen(frameStore[victimFrame * FRAME_SIZE], "r");
    int i = 0;
    char line[MAX_USER_INPUT];
    while (fgets(line, sizeof(line), fp) && i < FRAME_SIZE) {
        printf("%s", line);
        strcpy(frameStore[victimFrame * FRAME_SIZE + i++], ""); // Clear frameStore
    }

    printf("\nEnd of victim page contents.\n");
    return victimFrame;
}

struct PCB* handlePageFault(struct PCB *pcb, int pageNumber) {
    int frameIndex = getFreeFrame();
    if (frameIndex == -1) { // No free frame; evict a page
        frameIndex = evictPage();
        removePageInfo(pcb->scriptName, pageNumber);
        return updatePageInfo(pcb, pcb->scriptName, pageNumber, frameIndex);
    }

    printf("Page fault!\n");

    return updatePageInfo(pcb, pcb->scriptName, pageNumber, frameIndex);
}
