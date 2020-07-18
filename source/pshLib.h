#ifndef PSHLIB_H
#define PSHLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//general constants
#define TRUE 1
#define FALSE 0

//psh loop function
void psh_loop();

//psh_read_line constants
#define PSH_RL_BUFFSIZE 1024
char* psh_read_line(void);

#define PSH_TOK_BUFFSIZE 64
#define PSH_TOK_DELIM " \t\r\n\a"
char** psh_split_line(char *line);

int psh_launch(char **args);

//Pujol-Shell built-ins
int psh_cd(char **args);
int psh_help(char **args);
int psh_exit(char **args);

int psh_num_builtins();

int psh_execute(char **args);

#endif /* PSHLIB_H */