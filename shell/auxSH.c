/* A simple, lightweight Unix-like shell.
 * goSH, isn't it great?
 * I designed this program to use as little memory as possible.
 * As a side effect of this, goSH is able to run on ancient hardware.
 * It even works on my Macintosh SE/30 running A/UX (with a measly 8 MB of memory).
 * Created by Brandon Winston */

/* As a side note, I elected not to use the example skeleton
 * at all. I found that I often spend more time trying to conform to a
 * "template" than I do actually creating what makes sense to me. */

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "auxSH.h"

#define MAX_DIRNAME 100 //Maximum dir length
#define MAX_COMMAND 4096 //As defined in go/SH instructions (change to 1024 for Motorola 68K)
#define MAX_TOKEN 100 //Maximum number of operators (tokens)

int execute_nopipe_command(no_pipe *cmd);
int execute_piped_command(command *cmd);
int execute_cd(char** dir_ch);
int execute_auxillary(no_pipe *s);
int is_pipe(char *token);
int is_builtin(char *token);
int is_relative(char* path);
int contains_token(char **tokens);
void parse_line(char *line, char **tokens);
int extract_redirections(char** tokens, no_pipe* cmd);
command* construct_command(char** tokens);
void release_command(command *cmd);
void print_command(command *cmd, int level);

int main(int argc, char** argv) {
    /* Allocate memory for current working directory and inputs
   using dynamic memory allocation */
    char *cwd = (char *)malloc(sizeof(char) * MAX_DIRNAME); /* Current working directory */
    char *command_line = (char *)malloc(sizeof(char) * MAX_COMMAND); /* The command */
    char **tokens = (char **)malloc(sizeof(char *) * MAX_TOKEN); /* Command tokens (program name, parameters, pipe, etc.) */
    int i;
    for (i = 0; i < MAX_TOKEN; i++) {
        tokens[i] = (char *)malloc(sizeof(char) * MAX_COMMAND);
    }

    while (1) {
        //Print prompt
        getcwd(cwd, MAX_DIRNAME-1);
        printf("%s: goSH> ", cwd);

        //Read the command line
        fgets(command_line, MAX_COMMAND, stdin);
        //Strip the new line character
        if (command_line[strlen(command_line) - 1] == '\n') {
            command_line[strlen(command_line) - 1] = '\0';
        }
        //Parse the command into tokens
        parse_line(command_line, tokens);
        //Check for empty input
        if (!(*tokens)) {
            continue;
        }
        //Create chain of command
        command *cmd = construct_command(tokens);
        int exit = 0;
        if (cmd->litmus_clarus) {
            exit = execute_nopipe_command(cmd->litmus_clarus);
            if (exit == -1) {
                break;
            }
        }
        else {
            exit = execute_piped_command(cmd);
            if (exit == -1) {
                break;
            }
        }
        release_command(cmd);
    }
    //Free dynamically allocated memory
    free(cwd);
    free(command_line);
    for (i = 0; i < MAX_TOKEN; i++) {
        free(tokens[i]);
    }
    free(tokens);

    return 0;
}

//Changes current working directory to dir specified in dir_ch[1]
int execute_cd(char** dir_ch) {
    char *new_dir;

    //Check possible errors of the command.
    if (dir_ch == NULL || dir_ch[0] == NULL || strcmp(dir_ch[0], "cd") != 0){
        return EXIT_FAILURE;
    }
    if (dir_ch[1] == NULL){
        fprintf(stderr, "cd: Please enter a directory\n");
        return EXIT_FAILURE;
    }

    //Determine relativity of path
    //If relative, append
    if(is_relative(dir_ch[1])){
        if((new_dir = malloc(MAX_DIRNAME)) != NULL){
            getcwd(new_dir, MAX_DIRNAME);
            strcat(new_dir,"/");
            strcat(new_dir, dir_ch[1]);
            if (chdir(new_dir) == -1){
                perror(dir_ch[1]);
                return EXIT_FAILURE;
            }
        }
        else {
            fprintf(stderr,"malloc failed!\n");
        }
    }
    //If not relative, simply change
    else {
        if (chdir(dir_ch[1]) == -1){
            perror(dir_ch[1]);
            return EXIT_FAILURE;
        }
    }

    return 0;
}


//Executes command (as outlined by token array)
int execute_command(char **tokens) {
    char *name = tokens[0];

    if (execvp(name, tokens) == -1){
        perror(name);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


//Executes a user-provided command (it isn't cd, help, or exit)
int execute_auxillary(no_pipe *s) {
    // Check if the in field is not NULL.
    if(s->in != NULL){
        int fd1;

        // Open a new file descriptor.
        if ((fd1 = open(s->in, O_RDONLY, 0777)) == -1){
            perror("Cannot open input file\n");
            return EXIT_FAILURE;
        }

        // Redirect stdin/stdout/stderr to the corresponding file.
        if(dup2(fd1, STDIN_FILENO) == -1){
            perror ("dup error 1");
            return EXIT_FAILURE;
        }

        // Close the newly opened file descriptor.
        if (close(fd1) == -1){
            perror ("close error 1");
            return EXIT_FAILURE;
        }

        // Check if the out field is not NULL.
    } else if (s->out != NULL) {
        int fd2;

        // Open a new file descriptor.
        if ((fd2 = open(s->out, O_WRONLY|O_TRUNC|O_CREAT, 0777)) == -1){
            perror("Cannot open output file\n");
            return EXIT_FAILURE;
        }

        // Redirect stdin/stdout/stderr to the corresponding file
        if(dup2(fd2, STDOUT_FILENO) == -1){
            perror ("dup error 2");
            return EXIT_FAILURE;
        }

        // Close the newly opened file descriptor
        if (close(fd2) == -1){
            perror ("close error 2");
            return EXIT_FAILURE;
        }

        // Check if the err field is not NULL.
    } else if (s->err != NULL) {
        int fd3;

        if ((fd3 = open(s->err, O_WRONLY|O_TRUNC|O_CREAT, 0777)) == -1){
            perror("Cannot open output file\n");
            return EXIT_FAILURE;
        }

        if(dup2(fd3, STDERR_FILENO) == -1){
            perror ("dup error 3");
            return EXIT_FAILURE;
        }

        if (close(fd3) == -1){
            perror ("close error 3");
            return EXIT_FAILURE;
        }
    }
    return execute_command(s->tokens);

}

int execute_nopipe_command(no_pipe *cmd) {

    int status, exitcode;
    // Check if the command is builtin
    if(cmd->builtin != 0){
        if (cmd->builtin == BUILTIN_CD) {
            exitcode = execute_cd(cmd->tokens);
        }
        else if (cmd->builtin == BUILTIN_HELP){
            printf("goSH, version 0.1 (x86)\n");
            printf("These shell commands are defined internally.  Type `help' to see this list.\n");
            printf("  cd [DIRECTORY]      Change the current working directory\n");
            printf("  help                Display this help message\n");
            printf("  exit                Exit the program\n");
            printf("Created by Brandon Winston for CS1730\n");
        }
        else if (cmd->builtin == BUILTIN_EXIT){
            exit(EXIT_SUCCESS);
        }
    }
    else {
        //Checks whether fork() was successful
        switch (fork()){
            case -1:
                perror ("fork");
                return EXIT_FAILURE;
            case 0:
                exit(execute_auxillary(cmd));
            default:
                wait(&status);
        }

    }
    return exitcode;
}

int execute_piped_command(command *c) {

    int exitcode;
    // Check if this is a simple command.
    // If this is a simple command.
    if(c->litmus_clarus != NULL){
        exitcode = execute_nopipe_command(c->litmus_clarus);
    }

    else {
        if (!strcmp(c->operator, "|")) {
            int pfd[2];
            if (pipe(pfd) == -1){
                perror ("pipe");
                return EXIT_FAILURE;
            }

            switch (fork()){
                case -1:
                    perror ("fork");
                    return EXIT_FAILURE;

                case 0:
                    if (close(pfd[0]) == -1){
                        perror ("close read pipe");
                        exit(EXIT_FAILURE);
                    }
                    if(close(STDOUT_FILENO) == -1){
                        perror ("close stdout");
                        exit(EXIT_FAILURE);
                    }
                    if (pfd[1] != STDOUT_FILENO){
                        if(dup2(pfd[1], STDOUT_FILENO) == -1){
                            perror ("dup2 1");
                            exit(EXIT_FAILURE);
                        }
                    }
                    exitcode = execute_piped_command(c->cmd1);
                    exit(exitcode);
                default:
                    break;
            }

            switch (fork()){
                case -1:
                    perror ("fork");
                    return EXIT_FAILURE;
                case 0:
                    if (close(pfd[1]) == -1){
                        perror ("close write pipe");
                        exit(EXIT_FAILURE);
                    }
                    if(close(STDIN_FILENO) == -1){
                        perror ("close stdin");
                        exit(EXIT_FAILURE);
                    }
                    if (pfd[0] != STDIN_FILENO){
                        if(dup2(pfd[0], STDIN_FILENO) == -1){
                            perror ("dup2 2");
                            exit(EXIT_FAILURE);
                        }

                    }
                    //Recursive call to save memory
                    exitcode = execute_piped_command(c->cmd2);
                    exit(exitcode);
                //Fall through
                default:
                    break;
            }

            // Close both ends of the pipe
            if (close(pfd[0]) == -1){
                perror ("close pipe 0");
                return EXIT_FAILURE;
            }
            if (close(pfd[1]) == -1){
                perror ("close pipe 1");
                return EXIT_FAILURE;
            }
            //Wait for both children to finish.
            if (wait(NULL) == -1){
                perror ("wait 1");
                return EXIT_FAILURE;
            }
            if (wait(NULL) == -1){
                perror ("wait 2");
                return EXIT_FAILURE;
            }
        }
    }
    return exitcode;
}

int is_pipe(char *token) {
    return (strcmp(token, "|") == 0);
}

int is_builtin(char *token) {
    if (strcmp(token, "cd") == 0) {
        return BUILTIN_CD;
    }
    if (strcmp(token, "help") == 0) {

        return BUILTIN_HELP;
    }
    if (strcmp(token, "exit") == 0) {
        return BUILTIN_EXIT;
    }
    return 0;
}

int is_relative(char* path) {
    return (path[0] != '/');
}

int contains_token(char **tokens) {

    int i = 0;
    while(tokens[i]) {
        if (is_pipe(tokens[i]))
            return 1;
        i++;
    }
    return 0;
}

void parse_line(char *line, char **tokens) {

    while (*line != '\0') {
        //Replace all whitespace with \0
        while (*line == ' ' || *line == '\t' || *line == '\n') {
            *line++ = '\0';
        }

        if (*line == '\0') {
            break;
        }

        *tokens++ = line;

        while (*line != '\0' && *line != ' ' &&
               *line != '\t' && *line != '\n')  {
            line++;
        }
    }
    *tokens = '\0';
}

int extract_redirections(char** tokens, no_pipe* cmd) {

    int i = 0;
    int jump = 0;

    while(tokens[i]) {

        int skip = 0;
        if (!strcmp(tokens[i], ">")) {
            if(!tokens[i+1]) {
                return -1;
            }
            cmd->out = tokens[i+1];
            jump += 2;
            skip = 1;
        }
        if (!strcmp(tokens[i], "<")) {
            if(!tokens[i+1]) {
                return -1;
            }
            cmd->in = tokens[i+1];
            jump += 2;
            skip = 1;
        }
        if (!strcmp(tokens[i], "2>")) {
            if(!tokens[i+1]) {
                return -1;
            }
            cmd->err = tokens[i+1];
            jump += 2;
            skip = 1;
        }
        if (!strcmp(tokens[i], "&>")) {
            if(!tokens[i+1]) {
                return -1;
            }
            cmd->out = tokens[i+1];
            cmd->err = tokens[i+1];
            jump += 2;
            skip = 1;
        }

        if(skip){
            i++;
        }

        i++;
    }

    cmd->tokens = NULL;
    int j = 0;
    i = 0;
    while(tokens[i]) {
        if (!strcmp(tokens[i], "<") ||
            !strcmp(tokens[i], ">") ||
            !strcmp(tokens[i], "2>") ||
            !strcmp(tokens[i], "&>")) {
            i += 2;
        } else {
            //Use realloc to save memory
            cmd->tokens = (char**)realloc(cmd->tokens, sizeof(char*) * (j + 1));
            cmd->tokens[j++] = tokens[i++];
        }
    }
    //Use realloc to save memory
    cmd->tokens = (char**)realloc(cmd->tokens, sizeof(char*) * (j + 1));
    cmd->tokens[j] = NULL;

    return 0;
}

command* construct_command(char** tokens) {

    command *cmd = malloc(sizeof(command));
    cmd->cmd1 = NULL;
    cmd->cmd2 = NULL;
    cmd->litmus_clarus = NULL;

    if (!contains_token(tokens)) {

        cmd->litmus_clarus = malloc(sizeof(no_pipe));
        cmd->litmus_clarus->in = NULL;
        cmd->litmus_clarus->out = NULL;
        cmd->litmus_clarus->err = NULL;
        cmd->litmus_clarus->tokens = NULL;

        cmd->litmus_clarus->builtin = is_builtin(tokens[0]);

        int err = extract_redirections(tokens, cmd->litmus_clarus);
        if (err == -1) {
            printf("Redirections\n");
            return NULL;
        }
    }
    else {
        char **t1 = tokens, **t2;
        int i = 0;
        while(tokens[i]) {
            if(is_pipe(tokens[i])) {
                strncpy(cmd->operator, tokens[i], 2);
                tokens[i] = NULL;
                t2 = &(tokens[i+1]);
                break;
            }
            i++;
        }

        //Recursive call to save memory
        cmd->cmd1 = construct_command(t1);
        cmd->cmd2 = construct_command(t2);
    }

    return cmd;
}

void release_command(command *cmd) {

    if(cmd->litmus_clarus && cmd->litmus_clarus->tokens) {
        free(cmd->litmus_clarus->tokens);
    }
    if(cmd->cmd1) {
        release_command(cmd->cmd1);
    }
    if(cmd->cmd2) {
        release_command(cmd->cmd2);
    }
}

void print_command(command *cmd, int level) {

    int i;
    for(i = 0; i < level; i++) {
        printf("  ");
    }

    if(cmd->litmus_clarus) {

        i = 0;
        while(cmd->litmus_clarus->tokens[i]) {
            printf("%s ", cmd->litmus_clarus->tokens[i]);
            i++;
        }

        if(cmd->litmus_clarus->in) {
            printf("< %s ", cmd->litmus_clarus->in);
        }

        if(cmd->litmus_clarus->out) {
            printf("> %s ", cmd->litmus_clarus->out);
        }

        if(cmd->litmus_clarus->err) {
            printf("2> %s ", cmd->litmus_clarus->err);
        }

        printf("\n");
        return;
    }

    printf("Pipes:\n");

    if(cmd->cmd1) {
        print_command(cmd->cmd1, level+1);
    }

    if(cmd->cmd2) {
        print_command(cmd->cmd2, level+1);
    }

}