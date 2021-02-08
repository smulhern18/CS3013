//
// Created by sjmulhern on 2/7/2021.
//
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char** argv) {

    FILE *file = malloc(sizeof(FILE));

    switch (argv[1][0]) {
        case '1':
            file = fopen("seed_slug_1.txt", "r");
            break;
        case '2':
            file = fopen("seed_slug_2.txt", "r");
            break;
        case '3':
            file = fopen("seed_slug_3.txt", "r");
            break;
        case '4':
            file = fopen("seed_slug_4.txt", "r");
            break;
        default:
            printf("invalid commandline argument: %s\n", argv[1]);
            return(argc);
    }

    char *buff = (char *) calloc(sizeof(char), 256);
    fgets(buff, 256, file);

    // Convert seed from char array to integer
    int seed = atoi(buff);
    printf("The random seed value is: %d \n", seed);
    srand(seed);


    // Generate the random filepath
    int coinFlip = rand() % 2;
    int runTime = (rand() % 4) + 1;

    printf("I'll get to my task eventually!\n");

    sleep(runTime);

    if (coinFlip) {
        char** buffer = (char**) calloc(sizeof(calloc(sizeof(char), 16)), 4);
        buffer[0] = "last";
        buffer[1] = "-d";
        buffer[2] = "--fulltimes";
        buffer[3] = NULL;
        exit(execvp("last", buffer));
    } else {
        char** buffer = (char**) calloc(sizeof(calloc(sizeof(char), 16)), 3);
        buffer[0] = "id";
        buffer[1] = "-u";
        buffer[2] = NULL;
        exit(execvp("id", buffer));
    }
}

