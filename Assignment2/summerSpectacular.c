//
// Created by Sullivan Mulhern on 2/13/21.
//

#include "summerSpectacular.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

// Represents all the threads
struct Performer* allPerformers;
pthread_t* allThreads;

// represents the 4 places on the stage
pthread_mutex_t stage[4] = {0,0,0,0};

enum Style currentPerformance = EMPTY;

int main(int argc, char** argv) {

    allPerformers = (struct Performer*) calloc(sizeof(struct Performer), 25);
    allThreads = (pthread_t*) calloc(sizeof(pthread_t), 25);

    pthread_mutex_init(&stage[0], NULL);
    pthread_mutex_init(&stage[1], NULL);
    pthread_mutex_init(&stage[2], NULL);
    pthread_mutex_init(&stage[3], NULL);

    // Create all the performers in the array
    for (int i = 0; i < 25; i++) {
        struct Performer performer = {
            "wak",
            EMPTY,
            rand()%25,
            0,
            -1
        };

        char* name = (char*) calloc(sizeof(char), 32);
        if (i < 15) {
            sprintf(name, "Dancer %d", i+1);
            performer.name = name;
            performer.style = DANCER;
        } else if (i < 23) {
            sprintf(name, "Juggler %d", (i-14));
            performer.name = name;
            performer.style = JUGGLER;
        } else if (i < 25) {
            sprintf(name, "Soloist %d", (i-22));
            performer.name = name;
            performer.style = SOLOIST;
        }

        allPerformers[i] = performer;
    }

    for (int i = 0; i < 25; i++) {
        pthread_create(&allThreads[i], NULL, (void *) start, &allPerformers[i]);
        printf("Created performer %d\n", i+1);
    }

    sleep(120);
    for (int i = 0; i < 25; i++) {
        pthread_join(allThreads[i], NULL);
        printf("Destroyed performer %d\n", i+1);
    }

    exit(0);
}

void start(struct Performer performer) {
    checkStage(performer);
}

int checkStage(struct Performer performer) {

    return 0;
}

int checkStatus(struct Performer performer) {

    return 0;
}

void perform(struct Performer performer) {
    sleep(performer.performanceLength);
}

void enter(struct Performer performer) {

}

void exitStage(struct Performer performer) {

}