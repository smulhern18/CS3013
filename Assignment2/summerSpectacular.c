//
// Created by Sullivan Mulhern on 2/13/21.
//

// What 

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
pthread_mutex_t stage[4] = {PTHREAD_MUTEX_INITIALIZER,
                            PTHREAD_MUTEX_INITIALIZER,
                            PTHREAD_MUTEX_INITIALIZER,
                            PTHREAD_MUTEX_INITIALIZER};

enum Style currentPerformance = EMPTY;
pthread_cond_t styleConditions[3] = {PTHREAD_COND_INITIALIZER,
                                     PTHREAD_COND_INITIALIZER,
                                     PTHREAD_COND_INITIALIZER};

int main(int argc, char** argv) {

    allPerformers = (struct Performer*) calloc(sizeof(struct Performer), 25);
    allThreads = (pthread_t*) calloc(sizeof(pthread_t), 25);

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
        pthread_create(&allThreads[i], NULL, (void *) runThread, &allPerformers[i]);
        printf("Created performer %d\n", i+1);
    }

    sleep(10);
    for (int i = 0; i < 25; i++) {
        pthread_join(allThreads[i], NULL);
        printf("Destroyed performer %d\n", i+1);
    }

    exit(0);
}

void runThread(struct Performer performer) {
    while(1){
        int stagePosition = checkStage(performer);
        if (stagePosition && checkStatus(performer)) {
            enter(performer, stagePosition);
            perform(performer);
            exitStage(performer, stagePosition);
        } else {
            int location = -1;
            switch (performer.style) {
                case SOLOIST:
                    location = 2;
                    break;
                case DANCER:
                    location = 0;
                    break;
                case JUGGLER:
                    location = 1;
                    break;
                case EMPTY:
                    location = 0;
                    break;
            }
		pthread_mutex_t mutexLocation = PTHREAD_MUTEX_INITIALIZER;
            pthread_cond_wait(&styleConditions[location], &mutexLocation);
        }
    }
}

int checkStage(struct Performer performer) {
    if (currentPerformance == performer.style) {
        if (1 != 2) {

        }
    }
    return 0;
}

int checkStatus(struct Performer performer) {

    return performer.ready;
}

void perform(struct Performer performer) {
    sleep(performer.performanceLength);
}

void enter(struct Performer performer, int stagePosition) {
    performer.currentLocation = 1;
    pthread_mutex_lock(&stage[stagePosition]);
}

void exitStage(struct Performer performer, int stagePosition) {
    performer.currentLocation = 0;
}
