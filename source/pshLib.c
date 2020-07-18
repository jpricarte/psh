#include "pshLib.h"

char* builtin_str[] = {
        "cd",
        "clear",
        "help",
        "exit"
};

int (*builtin_func[]) (char **) = {
        &psh_cd,
        &psh_clear,
        &psh_help,
        &psh_exit
};

void psh_loop() {
    char *line;
    char **args;
    int status=1;

    do {
        printf("PSH > ");
        line = psh_read_line();
        args = psh_split_line(line);
        status = psh_execute(args);

        free(line);

    } while (status);
}

char* psh_read_line(void) {
    int buffsize = PSH_RL_BUFFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffsize);
    int c;

    if(!buffer) {
        fprintf(stderr, "psh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (TRUE) {
        c = getchar();
        if(c==EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= buffsize) {
            buffsize += PSH_RL_BUFFSIZE;
            buffer = realloc(buffer, buffsize);
            if(!buffer) {
                fprintf(stderr, "psh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char** psh_split_line(char* line) {
    int buffsize = PSH_TOK_BUFFSIZE, position = 0;
    char **tokens = malloc(buffsize * sizeof(char*));
    char *token;

    if(!tokens) {
        fprintf(stderr, "psh: allocation error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, PSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (token >= buffsize) {
            buffsize += PSH_TOK_BUFFSIZE;
            tokens = realloc(tokens, buffsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "psh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, PSH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

int psh_launch(char **args) {
    pid_t pid, wpid;
    int status=0;

    pid = fork();
    if (pid == 0) {
        //Child process
        if (execvp(args[0], args) == -1) {
            perror("psh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("psh");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int psh_cd(char **args){
    if (args[1] == NULL) {
        fprintf(stderr, "psh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("psh");
        }
    }
    return 1;
}

int psh_clear(char **args){
    printf(CLEAR_SCREEN_ANSI);
    return 1;
}

int psh_help(char **args){
    int i;
    printf("the Pujol Shell - the PSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < psh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int psh_exit(char **args){
    return 0;
}

int psh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int psh_execute(char **args) {
    int i;
    if( args[0] == NULL) {
        return TRUE;
    }

    for(i=0; i < psh_num_builtins(); i++) {
        if(strcmp(args[0], builtin_str[i]) == 0) {
            return (builtin_func[i])(args);
        }
    }

    return psh_launch(args);
}
