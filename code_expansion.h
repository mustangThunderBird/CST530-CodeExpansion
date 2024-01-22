// shell_commands.h
#ifndef CODE_EXPANSION_H
#define CODE_EXPANSION_H

#include <unistd.h> // For pid_t

#define TRUE 1
#define MAX_LENGTH 1024

// Function declarations
void type_prompt();
void read_command(char **command, char *parameters[], char *buffer, char **redirect_filename, int *redirect_value);
void redirect_input_output(int redirect_value, char *redirect_filename);
void handle_pipe(char *first_command[], char *second_command[]); 
int is_background_job(char *token);
int is_redirection(char *token);
int is_pipe(char *token);

#endif // CODE_EXPANSION_H
