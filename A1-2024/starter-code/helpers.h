#include <dirent.h>

int my_ls_filter(const struct dirent *namelist);
int my_ls_sort(const struct dirent **namelist_one, const struct dirent **namelist_two);
int check_alphanum(char *name);
int generate_pid();
int execute_instruction(char *key);
