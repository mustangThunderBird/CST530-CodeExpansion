#include "code_expansion.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define MAX_LENGTH 1024


int is_background_job(char *command){
    return 0;
}

int is_redirection(char *command){
    return 0;
}

int is_pipe(char *command){
    return 0;
}

void type_prompt()
{
    printf("nicholas_shell> ");
}

void read_command(char **command, char *parameters[], char *buffer)
{
    char *line = fgets(buffer, MAX_LENGTH, stdin);
    if (line == NULL)
    {
        // Handle error or end of file
        *command = NULL; // Indicate no command to process
        return;
    }

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n')
    {
        buffer[length - 1] = '\0';
    }

    char *token = strtok(buffer, " ");
    *command = token;

    int i = 0;
    while (token != NULL)
    {
        parameters[i++] = token;
        token = strtok(NULL, " ");
        if (i >= MAX_LENGTH - 1)
            break;
    }
    parameters[i] = NULL; // NULL terminate the parameters array
}

void redirect_input_output(char *command)
{
    // Implementation for input and output redirection

}

void handle_pipe(char *command)
{
    // Implementation for handling pipes
}

int main()
{
    char buffer[MAX_LENGTH];
    char *command;
    char *parameters[MAX_LENGTH];
    int status;
    pid_t pid;

    while (TRUE)
    {
        type_prompt();
        read_command(&command, parameters, buffer);

        if (command == NULL)
        {
            continue; // No command to process, start over
        }

        //check if the command is a shell built in command
        if (strcmp(command, "cd") == 0)
        {
            // Handling 'cd' command
            if (parameters[1] != NULL)
            {
                if (chdir(parameters[1]) != 0)
                {
                    perror("chdir");
                }
            }
            else
            {
                fprintf(stderr, "Usage: cd <directory>\n");
            }
        }
        else //not a shell built in command
        {
            pid = fork();
            if (pid != 0)
            {
                // Parent process
                if (!is_background_job(command))
                {
                    waitpid(pid, &status, 0); // Wait for child to exit
                }
            }
            else
            {
                // Child process
                execvp(command, parameters); // Execute command
                // If execvp returns, an error occurred
                fprintf(stderr, "Error executing command '%s'\n", command);
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}
