//
// Created by sjmulhern on 1/29/2021.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

#include "prolific.h"

int parentPID = -1;
int seed;
int randArray[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int i = 0;

int main(int argc, char** argv) {

    if (!amChild()) {

        // Set PID
        parentPID = getpid();

        // Get the seed value from the seed.txt file
        FILE *file = fopen("seed.txt", "r");
        char *buff = (char *) calloc(sizeof(char), 256);
        fgets(buff, 256, file);

        // Convert seed from char array to integer
        seed = atoi(buff);
        printf("The random seed value is: %d \n", seed);
        srand(seed);

        // Generate the number of children
        int childAmount = (rand() % 6) + 10;
        printf("I will have %d children!\n", childAmount);
        for (i = 0; i < childAmount; i++) {
            randArray[i] = rand();
        }
        printf("Created the array of random numbers!\n\n\n\n");
        pid_t childProcess;
        int status;
        for (i = 0; i < childAmount; i++) {

            // Fork the process off now
            childProcess = fork();

            // Check the PID to see if it is the parent
            if (parentPID == getpid()) {
                printf("[Parent]: I am waiting for PID %d to finish\n", (int) childProcess);
                waitpid(childProcess, &status, WUNTRACED | WCONTINUED);
                printf("[Parent]: Child %d finished with status code %d.\n", (int) childProcess, WEXITSTATUS(status));
            } else {
                // If the PID is not the parent's then do this
                int randNum = randArray[i];
                int pid = getpid();
                int exitCode = (randNum % 50) + 1;
                int waitTime = (randNum % 3) + 1;
                printf("    [Child, PID: %d]: I am child %d and I will wait %d seconds and exit with code %d.\n", pid, i, waitTime, exitCode);
                sleep(waitTime);
                printf("    [Child, PID: %d]: Now exiting.\n", pid);
                return(exitCode);
            }
        }
        return 0;
    }
}

bool amChild() {
    if (parentPID == -1) {
        return false;
    } else {
        return true;
    }
}