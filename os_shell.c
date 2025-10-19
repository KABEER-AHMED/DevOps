// myshell.c
// A tiny shell: displays prompt, runs external commands via fork+exec,
// supports "exit" to quit. No background jobs, no redirection, no piping.
// Written to be small, clear, and easy to read.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define PROMPT "mysh> "
#define MAX_LINE 1024     // max input length
#define MAX_ARGS 64       // max tokens per command

// trim trailing newline and leading/trailing spaces
static void trim(char *s) {
    if (!s) return;
    // remove trailing newline
    size_t len = strlen(s);
    if (len == 0) return;
    if (s[len - 1] == '\n') s[len - 1] = '\0';

    // trim leading spaces
    char *start = s;
    while (*start == ' ' || *start == '\t') start++;

    // if already at start, nothing to move
    if (start != s) memmove(s, start, strlen(start) + 1);

    // trim trailing spaces
    len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[len - 1] = '\0';
        len--;
    }
}

// parse the input line into argv-style array, returns arg count
static int parse_line(char *line, char *argv[], int max_args) {
    int argc = 0;
    char *saveptr = NULL;
    char *token = strtok_r(line, " \t\r\n", &saveptr);
    while (token != NULL && argc < max_args - 1) {
        argv[argc++] = token;
        token = strtok_r(NULL, " \t\r\n", &saveptr);
    }
    argv[argc] = NULL;
    return argc;
}

// launch a command using fork + execvp and wait for it to finish
static void launch_command(char *argv[]) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork"); // something went wrong
        return;
    }
    if (pid == 0) {
        // Child: replace the process image with the command
        // execvp searches PATH (convenient for this assignment)
        execvp(argv[0], argv);
        // If execvp returns, an error occurred
        perror(argv[0]);
        _exit(EXIT_FAILURE);
    } else {
        // Parent: wait for the child to finish
        int status = 0;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
        }
        // we don't need to interpret status for this assignment
    }
}

int main(void) {
    char line[MAX_LINE];
    char *argv[MAX_ARGS];

    while (1) {
        // show prompt
        if (printf("%s", PROMPT) < 0) {
            // if writing prompt fails, try to continue or exit gracefully
            break;
        }
        fflush(stdout);

        // read input
        if (fgets(line, sizeof(line), stdin) == NULL) {
            // EOF (Ctrl+D) or read error -> exit loop gracefully
            putchar('\n'); // keep terminal tidy
            break;
        }

        // strip newline and outer spaces
        trim(line);

        // if the user hit enter on an empty line, show prompt again
        if (line[0] == '\0') continue;

        // parse into argv[]
        int argc = parse_line(line, argv, MAX_ARGS);
        if (argc == 0) continue;

        // builtin: exit
        if (strcmp(argv[0], "exit") == 0) {
            break; // clean exit from shell
        }

        // launch external command (strictly using fork + execvp)
        launch_command(argv);
    }

    return EXIT_SUCCESS;
}
