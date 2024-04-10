#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 64

void parse_input(char *input, char **args) {
    char *token;
    int i = 0;

    token = strtok(input, " \t\n");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

// Change directory command
void change_directory(char *dir) {
    if (chdir(dir) != 0) {
        perror("chdir");
    }
}

// Main loop
void shell_loop() {
    char input[MAX_INPUT_LENGTH];
    char *args[MAX_ARGS];
    pid_t pid;
    int status;

    printf("Welcome to k0 shell\n");

    while (1) {
        printf("shell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        parse_input(input, args);

        // Check for built-in commands
        if (strcmp(args[0], "quit") == 0) {
            printf("Goodbye\n");
            break;
        } else if (strcmp(args[0], "cd") == 0) {
            // Change directory command
            if (args[1] == NULL) {
                fprintf(stderr, "cd: missing argument\n");
            } else {
                change_directory(args[1]);
            }
            continue;
        }

        pid = fork();

        if (pid < 0) {
            perror("fork");
        } else if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                perror("execvp");
            }
            exit(EXIT_FAILURE);
        } else {
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
}

int main() {
    shell_loop();
    return EXIT_SUCCESS;
}
