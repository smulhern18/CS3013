//
// Created by sjmulhern on 2/1/2021.
//
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

#include "explorer.h"

int seed;
int descendentAmount;
int parentId;

int main(int argc, char** argv) {

    // Get the seed value from the seed.txt file
    FILE *file = fopen("seed.txt", "r");
    char *buff = (char *) calloc(sizeof(char), 256);
    fgets(buff, 256, file);

    // Convert seed from char array to integer
    seed = atoi(buff);
    printf("The random seed value is: %d \n", seed);
    srand(seed);

    // Generate the random filepath
    int fileLocation = rand() % 6;
    printf("It's time to see the world!\n");

    for (int i = 0; i < 6; i++) {
        pid_t childProcess;
        int status;
        char* location = (char*) calloc(sizeof(char), 32);
        switch (fileLocation) {
            case 0:
                location = "/home";
                break;
            case 1:
                location = "/proc";
                break;
            case 2:
                location = "/proc/sys";
                break;
            case 3:
                location = "/usr";
                break;
            case 4:
                location = "/usr/bin";
                break;
            case 5:
                location = "/bin";
                break;
        }

        childProcess = fork();

        if (childProcess == 0) {
            chdir(location);
            printf("Current Reported Directory: %s\n", location);
            printf("    [Child, PID: %d]: Executing 'ls -alh' command...\n", (int) getpid());
            char** buffer = (char**) calloc(sizeof(calloc(sizeof(char), 8)), 3);
            buffer[0] = "ls";
            buffer[1] = "-alh";
            buffer[2] = NULL;
            execvp("ls", buffer);
            exit(0);
        } else {
            int status = -1;
            printf("[Parent]: I am waiting for Child %d to finish\n", (int) childProcess);
            waitpid(childProcess, &status, WUNTRACED | WCONTINUED);
            printf("[Parent, PID: %d]: Child %d finished with status code %d. I can exit now\n", (int) getpid(), (int) childProcess, WEXITSTATUS(status));
        }

        fileLocation = rand() % 6;
    }

    return 0;

}