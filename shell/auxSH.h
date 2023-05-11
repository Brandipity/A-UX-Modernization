#ifndef _GOSH_H
#define _GOSH_H

#define BUILTIN_CD   1
#define BUILTIN_HELP   2
#define BUILTIN_EXIT 3

typedef struct no_pipe_t {
	char *in, *out, *err;
	char **tokens;
	int builtin;
} no_pipe;

typedef struct command_t {
	struct command_t *cmd1, *cmd2;
	no_pipe* litmus_clarus;
	char operator[2];
} command;

#endif

