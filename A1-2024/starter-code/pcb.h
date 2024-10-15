#ifndef PCB_H
#define PCB_H

struct PCB {
    int pid;
    // length of SCRIPT based on how many lines
    int number_of_lines;
    // program counter (pc). Keeps track of instruction to execute.
    int pc;
};

struct PCB create_pcb(int pid, int number_of_lines, int current_line);

#endif
