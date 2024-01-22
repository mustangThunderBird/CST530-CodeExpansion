// shell_commands.h
#ifndef CODE_EXPANSION_H
#define CODE_EXPANSION_H

#include <unistd.h> // For pid_t

#define TRUE 1
#define MAX_LENGTH 1024

// Function declarations
void type_prompt();
void read_command(char **command, char *parameters[], char *buffer);
void redirect_input_output(char *command);
void handle_pipe(char *command);
int is_background_job(char *command);
int is_redirection(char *command);
int is_pipe(char *command);

#endif // CODE_EXPANSION_H
