#include "pcb.h"

struct PCB create_pcb(int pid, int number_of_lines, int current_line) {
    struct PCB pcb;
    pcb.pid = pid;
    pcb.number_of_lines = number_of_lines;
    pcb.pc = current_line;
    return pcb;
}