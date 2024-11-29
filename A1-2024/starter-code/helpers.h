#include <dirent.h>
#include "pcb.h"

#ifndef HELPERS_H
#define HELPERS_H

int myLsFilter(const struct dirent *namelist);
int myLsSort(const struct dirent **namelist_one,
               const struct dirent **namelist_two);
int checkAlphanum(char *name);
int generatePID();
int executeInstruction(char *key);
int isProperPolicy(char *policy);
int executePagingInstruction(int index, int offset);
void loadPageOnDemand(struct pagingReturn *pageReturn, int pageNumber, char *scriptName);
struct PCB* handlePageFault(struct PCB *pcb, int pageNumber);
int evictPage();

#endif