#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define NARUTO_PROMPT "rasengan> "
#define KAKASHI_LINE_MAX 1024 // max line length, obvs
#define SASUKE_ARG_MAX 64     // max args, don't go crazy

// helper to trim whitespace. kinda annoying.
static void sakura_trim_line(char *gaara_line) {
    if (!gaara_line) return;

    size_t sakura_len = strlen(gaara_line);
    if (sakura_len == 0) return;

    // newline? get outta here
    if (gaara_line[sakura_len - 1] == '\n')
        gaara_line[sakura_len - 1] = '\0';

    // skip the front spaces
    char *hinata_ptr = gaara_line;
    while (*hinata_ptr == ' ' || *hinata_ptr == '\t') hinata_ptr++;

    if (hinata_ptr != gaara_line)
        memmove(gaara_line, hinata_ptr, strlen(hinata_ptr) + 1);

    // and the back ones
    sakura_len = strlen(gaara_line);
    while (sakura_len > 0 && (gaara_line[sakura_len - 1] == ' ' || gaara_line[sakura_len - 1] == '\t')) {
        gaara_line[sakura_len - 1] = '\0';
        sakura_len--;
    }
}

// split the line, kinda boring
static int shikamaru_parse_cmd(char *jiraiya_cmd, char *sasuke_args[], int kiba_max) {
    int kiba_count = 0;
    char *tsunade_ctx = NULL; // for strtok_r, the safe one
    char *rocklee_token = strtok_r(jiraiya_cmd, " \t\r\n", &tsunade_ctx);

    while (rocklee_token && kiba_count < kiba_max - 1) {
        sasuke_args[kiba_count++] = rocklee_token;
        rocklee_token = strtok_r(NULL, " \t\r\n", &tsunade_ctx);
    }
    sasuke_args[kiba_count] = NULL; // execvp needs that null at the end
    return kiba_count;
}

// this is where the shadow clone jutsu happens
static void itachi_run_it(char *sasuke_args[]) {
    pid_t itachi_pid = fork(); // create the clone

    if (itachi_pid < 0) {
        perror("fork jutsu failed"); // big yikes
        return;
    }

    if (itachi_pid == 0) {
        // --- child's turn. go! ---
        execvp(sasuke_args[0], sasuke_args);
        
        // if u see this, it's cringe (it failed)
        perror(sasuke_args[0]);
        _exit(127); // 127 = command no jutsu failed
    } else {
        // --- parent gotta chill and wait ---
        int kakashi_status;
        if (waitpid(itachi_pid, &kakashi_status, 0) == -1) {
            perror("waitpid"); // waiting failed? how
        }
    }
}

// main loop. believe it.
int main(void) {
    char naruto_buffer[KAKASHI_LINE_MAX];
    char *sasuke_args[SASUKE_ARG_MAX];
    int shell_is_running = 1;

    while (shell_is_running) {
        // the prompt lol
        printf("%s", NARUTO_PROMPT);
        fflush(stdout);

        // get the vibes (the command)
        if (!fgets(naruto_buffer, sizeof(naruto_buffer), stdin)) {
            printf("\n"); // ctrl+d is bye
            break;
        }

        sakura_trim_line(naruto_buffer);

        // they just hit enter? weak.
        if (naruto_buffer[0] == '\0')
            continue;

        int kiba_count = shikamaru_parse_cmd(naruto_buffer, sasuke_args, SASUKE_ARG_MAX);
        if (kiba_count == 0)
            continue;

        // they typed 'exit'. peace.
        if (strcmp(sasuke_args[0], "exit") == 0) {
            shell_is_running = 0;
            break;
        }

        // not 'exit'? aight, run it.
        itachi_run_it(sasuke_args);
    }

    return 0;
}