//
// Created by sjmulhern on 1/29/2021.
//
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

#include "generation.h"

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

    // Generate the number of children
    descendentAmount = (rand() % 4) + 7;
    printf("I will have %d descendents!\n", descendentAmount);

    pid_t childProcess;
    int status;

        restart:
        parentId = (int) getpid();
        childProcess = fork();
        if (parentId == (int) getpid()) {
            printf("[Parent, PID: %d]: I am waiting for PID %d to finish\n", parentId, (int) childProcess);
            waitpid(childProcess, &status, WUNTRACED | WCONTINUED);
            printf("[Parent, PID: %d]: Child %d finished with status code %d. I can exit now\n", parentId, (int) childProcess, WEXITSTATUS(status));
            exit(WEXITSTATUS(status)+1);
        } else {
            sleep(1);
            printf("    [Child, PID: %d]: I was called with descendent count = %d. I'll have %d descendents\n", (int) getpid(), descendentAmount, descendentAmount-1);
            if (descendentAmount > 1) {
                descendentAmount--;
                goto restart;
            } else {
                exit(0);
            }
        }
}