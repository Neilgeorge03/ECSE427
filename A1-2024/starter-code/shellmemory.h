#define MEM_SIZE 1000
#define MAX_SCRIPT_SIZE  (MAX_USER_INPUT * 100)

void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int load_script_in_memory(FILE *fp, int pid);
int clear_mem(int pid, int length);
void printMemory();
void loadCommandInMemory(char *commandString, int lineNumber, int pid);