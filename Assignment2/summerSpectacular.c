//
// Created by Sullivan Mulhern 2/13/21.
//

#include "summerSpectacular.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

// Represents all the threads
struct Performer* allPerformers;
pthread_t* allThreads;
pthread_mutex_t allMutexes[25];

int runThreads = 1;

// represents the 4 places on the stage
pthread_mutex_t stage[4];

int currentPerformers = 0;

pthread_mutex_t emptySpot;

enum Style currentPerformance = EMPTY;


pthread_cond_t styleConditions[3] = {PTHREAD_COND_INITIALIZER,
                                     PTHREAD_COND_INITIALIZER,
                                     PTHREAD_COND_INITIALIZER};

int main(int argc, char** argv) {

    allPerformers = (struct Performer*) calloc(sizeof(struct Performer), 25);
    allThreads = (pthread_t*) calloc(sizeof(pthread_t), 25);

    pthread_mutex_init(&stage[0], NULL);
    pthread_mutex_init(&stage[1], NULL);
    pthread_mutex_init(&stage[2], NULL);
    pthread_mutex_init(&stage[3], NULL);
    pthread_mutex_init(&emptySpot, NULL);
    for (int i = 0; i < 25; i++) {
        pthread_mutex_init(&allMutexes[i], NULL);
    }

    // Create all the performers in the array
    for (int i = 0; i < 25; i++) {
        strncpy(allPerformers[i].name, "wak", 4);
        allPerformers[i].style = EMPTY;
        allPerformers[i].performanceLength = rand()%5;
        allPerformers[i].ready = 0;
        allPerformers[i].currentLocation = -1;
        allPerformers[i].threadNumber = i;

        char* name = (char*) calloc(sizeof(char), 32);
        if (i < 15) {
            sprintf(allPerformers[i].name, "Dancer %d", i+1);
            allPerformers[i].style = DANCER;
        } else if (i < 23) {
            sprintf(allPerformers[i].name, "Juggler %d", (i-14));
            allPerformers[i].style = JUGGLER;
        } else if (i < 25) {
            sprintf(allPerformers[i].name, "Soloist %d", (i-22));
            allPerformers[i].style = SOLOIST;
        }
    }

    for (int i = 0; i < 25; i++) {
        pthread_create(&allThreads[i], NULL, (void*)&runThread, &(allPerformers[i]));
        printf("Created performer %d\n", i+1);
    }

    sleep(120);
    runThreads = 0;

    for (int i = 0; i < 25; i++) {
        pthread_exit(&allThreads[i]);
        printf("Destroyed performer %d\n", i+1);
        sleep(1);
    }

    exit(0);
}

void runThread(struct Performer* performer) {
    sleep(rand()%10);
    performer->ready = 1;
    while(runThreads){
        int stagePosition = checkStage(*performer);
        printf("%s stage status %d\n", performer->name, stagePosition);
        if ((stagePosition != -1) && checkStatus(*performer)) {
            performer->ready = 0;

            pthread_mutex_lock(&emptySpot);
            enterStage(*performer, stagePosition);
            pthread_mutex_unlock(&emptySpot);


            perform(*performer);

            pthread_mutex_lock(&emptySpot);
            exitStage(*performer, stagePosition);
            pthread_mutex_unlock(&emptySpot);

            int location = -1;
            switch (performer->style) {
                case DANCER:
                    location = 1;
                    break;
                case JUGGLER:
                    location = 2;
                    break;
                case SOLOIST:
                    location = 0;
                    break;
                case EMPTY:
                    location = 0;
                    break;
            }
            pthread_cond_signal(&styleConditions[location]);
            sleep(performer->performanceLength*3);
        } else {
            int location = -1;
            switch (performer->style) {
                case DANCER:
                    location = 0;
                    break;
                case JUGGLER:
                    location = 1;
                    break;
                case SOLOIST:
                    location = 2;
                    break;
                case EMPTY:
                    location = 2;
                    break;
            }
            pthread_cond_wait(&styleConditions[location], &allMutexes[performer->threadNumber]);
            pthread_mutex_unlock(&allMutexes[performer->threadNumber]);
        }
    }
}

int checkStage(struct Performer performer) {
        if (currentPerformance == performer.style || currentPerformance == EMPTY) {
            for (int i =0; i < 4; i++) {
                int status = pthread_mutex_trylock(&stage[i]);
                if (status == 0 || status == 35) {
                    return i;
                }
            }
        }
        return -1;
}

int checkStatus(struct Performer performer) {
    return performer.ready;
}

void perform(struct Performer performer) {
    sleep(performer.performanceLength);
    printf("%s is Performing!\n", performer.name);
}

void enterStage(struct Performer performer, int stagePosition) {
    performer.currentLocation = ON_STAGE;
    currentPerformers++;
    if (currentPerformers == 1) {
        currentPerformance = performer.style;
    }
    if (performer.style != SOLOIST) {
        int styleNotify;
        switch (performer.style) {
            case DANCER:
                styleNotify = 0;
                break;
            case JUGGLER:
                styleNotify = 1;
                break;
            case EMPTY:
            default:
                styleNotify = 2;
                break;
        }
        pthread_cond_broadcast(&styleConditions[styleNotify]);
    } else {
        pthread_mutex_lock(&stage[0]);
        pthread_mutex_lock(&stage[1]);
        pthread_mutex_lock(&stage[2]);
        pthread_mutex_lock(&stage[3]);
    }
    if (currentPerformance != performer.style) {
        currentPerformance = performer.style;
    }
    printf("%s is entering stage position %d\n", performer.name, stagePosition+1);
}

void exitStage(struct Performer performer, int stagePosition) {
    performer.currentLocation = OFF_STAGE;
    if (performer.style != SOLOIST) {
        pthread_mutex_unlock(&stage[stagePosition]);
    } else {
        pthread_mutex_unlock(&stage[0]);
        pthread_mutex_unlock(&stage[1]);
        pthread_mutex_unlock(&stage[2]);
        pthread_mutex_unlock(&stage[3]);
    }
    printf("%s is now exiting stage positon %d!\n", performer.name, stagePosition+1);

    currentPerformers--;
    if (currentPerformers == 0 || rand()%50 == 25) {
        switch (performer.style) {
            case DANCER:
                currentPerformance = JUGGLER;
                break;
            case JUGGLER:
                currentPerformance = SOLOIST;
                break;
            case SOLOIST:
                currentPerformance = DANCER;
                break;
        }
    }
}
