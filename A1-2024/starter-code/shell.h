#ifndef SHELL_H
#define SHELL_H

#define MAX_USER_INPUT 1000
#define Director

int parseInput(char inp[]);
int clearBackingStoreDirectory(const char *dirLocation);
int createBackingStoreDirectory(const char *dirLocation);
int deleteBackingStoreDirectory(const char *dirLocation);

#endif