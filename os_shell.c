/**
 * myshell.c
 *
 * A minimal Linux shell implementation in C.
 * This program displays a prompt, reads a command line,
 * parses it into arguments, and executes the command
 * using fork() and execvp(). It also includes a
 * built-in 'exit' command.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Define constants for buffer sizes
#define MAX_LINE_LENGTH 1024 // Max length of a command line
#define MAX_ARGS 64          // Max number of arguments per command

int main() {
    char line[MAX_LINE_LENGTH]; // Buffer for user input
    char *args[MAX_ARGS];       // Array of pointers to argument strings
    int should_run = 1;         // Flag to control the main loop

    while (should_run) {
        // 1. Display the prompt
        printf("mysh> ");
        fflush(stdout); // Ensure the prompt is displayed immediately

        // Read a line of input from the user
        if (fgets(line, sizeof(line), stdin) == NULL) {
            // Handle EOF (Ctrl+D) as an exit command
            printf("\n");
            break;
        }

        // Remove the trailing newline character read by fgets
        line[strcspn(line, "\n")] = 0;

        // 2. Parse the input line into arguments
        int arg_count = 0;
        char *token = strtok(line, " \t\r\n\a"); // Tokenize by whitespace

        while (token != NULL && arg_count < MAX_ARGS - 1) {
            args[arg_count] = token;
            arg_count++;
            token = strtok(NULL, " \t\r\n\a");
        }
        
        // Terminate the argument list with NULL for execvp
        args[arg_count] = NULL;

        // If no command was entered, continue to the next prompt
        if (args[0] == NULL) {
            continue;
        }

        // 3. Check for the built-in 'exit' command
        if (strcmp(args[0], "exit") == 0) {
            should_run = 0; // Set the flag to terminate the loop
        } else {
            // 4. Execute the external command
            pid_t pid = fork();

            if (pid < 0) {
                // Fork failed
                perror("fork");
            } else if (pid == 0) {
                // --- This is the Child Process ---
                
                // Use execvp to find the command in the system's PATH
                // and replace the child process with it.
                if (execvp(args[0], args) == -1) {
                    // If execvp returns, an error occurred (e.g., command not found)
                    perror(args[0]);
                    exit(EXIT_FAILURE); // Exit the child process on failure
                }
            } else {
                // --- This is the Parent Process ---
                
                // Wait for the child process to complete
                wait(NULL);
            }
        }
    }

    return EXIT_SUCCESS;
}
