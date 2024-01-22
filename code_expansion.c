#include "code_expansion.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h> 

#define TRUE 1
#define MAX_LENGTH 1024


int is_background_job(char *command){
    return 0;
}

int is_redirection(char *token){
    int is_redirect_char = -1;
    if(strcmp(token, ">") == 0){
        is_redirect_char = 0;
    }else if(strcmp(token, "<") == 0){
        is_redirect_char = 1;
    }else if (strcmp(token, ">>") == 0){
        is_redirect_char = 2;
    }
    return is_redirect_char;
}

int is_pipe(char *token){
    //check if token is pipe character
    int character_is_pipe = -1;
    if(strcmp(token, "|") == 0){
        character_is_pipe = 1;
    }
    return character_is_pipe;
}

void type_prompt()
{
    printf("nicholas_shell> ");
}

void read_command(char **command, char *parameters[], char *buffer, char **redirect_filename, int *redirect_value)
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

        if(is_redirection(token) == -1){
            parameters[i++] = token;
        }else{
            *redirect_value = is_redirection(token);
            token = strtok(NULL, " ");
            *redirect_filename = token;
        }
        token = strtok(NULL, " ");
        if (i >= MAX_LENGTH - 1)
            break;
    }
    parameters[i] = NULL; // NULL terminate the parameters array
}

void redirect_input_output(int redirect_value, char *redirect_filename)
{
    if (redirect_value == 0)
    { // output
        int fd_out = open(redirect_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out == -1)
        {
            perror("open for output redirection");
            exit(EXIT_FAILURE);
        }
        dup2(fd_out, STDOUT_FILENO); // duplicates one file descriptor, making it a copy of another file descriptor. This allows us to redirect
        close(fd_out);
    }
    else if (redirect_value == 1)
    { //input
        int fd_in = open(redirect_filename, O_RDONLY);
        if (fd_in == -1)
        {
            perror("open for input redirection");
            exit(EXIT_FAILURE);
        }
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }else if(redirect_value == 2)
    { //append
        int fd_append = open(redirect_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd_append == -1)
        {
            perror("open for append redirection");
            exit(EXIT_FAILURE);
        }
        dup2(fd_append, STDOUT_FILENO); // Redirect stdout to fd_append
        close(fd_append);
    }
}

void execute_command(char *command, char *parameters[])
{
    execvp(command, parameters);
    // If execvp returns, an error occurred
    fprintf(stderr, "Error executing command '%s'\n", command);
    exit(EXIT_FAILURE);
}

void handle_pipe(char *first_command[], char *second_command[])
{
    int fds[2]; // file descriptors for the pipe
    if (pipe(fds) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Child process
        close(fds[0]);               // Close unused read end
        dup2(fds[1], STDOUT_FILENO); // Connect stdout to write end of pipe
        close(fds[1]);

        execute_command(first_command[0], first_command);
    }
    else
    {
        // Parent process
        close(fds[1]);              // Close unused write end
        dup2(fds[0], STDIN_FILENO); // Connect stdin to read end of pipe
        close(fds[0]);

        waitpid(pid, NULL, 0); // Wait for child to finish

        execute_command(second_command[0], second_command);
    }
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
        char *redirect_filename;
        int redirect_value = -1;

        type_prompt();
        read_command(&command, parameters, buffer, &redirect_filename, &redirect_value);

        if (command == NULL)
        {
            continue; // No command to process, start over
        }

        // check if the command is a shell built in command
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
        else // not a shell built in command
        {
            pid = fork();
            if (pid == 0)
            {
                // Child process
                if (redirect_value != -1)
                {
                    redirect_input_output(redirect_value, redirect_filename);
                }

                // Check for pipe and split commands if necessary
                char *first_command[MAX_LENGTH];
                char *second_command[MAX_LENGTH];
                int i = 0, j = 0, found_pipe = 0;
                for (i = 0; parameters[i] != NULL; i++)
                {
                    if (strcmp(parameters[i], "|") == 0)
                    {
                        found_pipe = 1;
                        first_command[i] = NULL;
                        i++;
                        break;
                    }
                    first_command[i] = parameters[i];
                }
                if (found_pipe)
                {
                    for (j = 0; parameters[i] != NULL; i++, j++)
                    {
                        second_command[j] = parameters[i];
                    }
                    second_command[j] = NULL;
                    handle_pipe(first_command, second_command);
                }
                else
                {
                    execute_command(command, parameters);
                }
            }
            else
            {
                // Parent process
                if (!is_background_job(command))
                {
                    waitpid(pid, &status, 0); // Wait for child to exit
                }
            }
        }
    }
    return 0;
}
