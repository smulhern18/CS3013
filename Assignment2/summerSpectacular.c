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
int stage[4];

int currentPerformers = 0;

pthread_mutex_t stageLock;
pthread_mutex_t waitingLock;

enum Style currentPerformance = EMPTY;

pthread_cond_t styleConditions[3] = {PTHREAD_COND_INITIALIZER,
                                     PTHREAD_COND_INITIALIZER,
                                     PTHREAD_COND_INITIALIZER};

int jugglerWaiting = 0;
int dancerWaiting = 0;
int soloistWaiting = 0;

int main(int argc, char** argv) {

    allPerformers = (struct Performer*) calloc(sizeof(struct Performer), 25);
    allThreads = (pthread_t*) calloc(sizeof(pthread_t), 25);

    stage[0] = EMPTY_SPOT;
    stage[1] = EMPTY_SPOT;
    stage[2] = EMPTY_SPOT;
    stage[3] = EMPTY_SPOT;
    pthread_mutex_init(&stageLock, NULL);
    pthread_mutex_init(&waitingLock, NULL);

    for (int i = 0; i < 25; i++) {
        pthread_mutex_init(&allMutexes[i], NULL);
    }

    // Create all the performers in the array
    for (int i = 0; i < 25; i++) {
        strncpy(allPerformers[i].name, "wak", 4);
        allPerformers[i].style = EMPTY;
        allPerformers[i].performanceLength = rand()%5 + 2;
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

        pthread_mutex_lock(&stageLock);
        int stagePosition = enterStage(*performer);
        if (stagePosition != -1) {
            pthread_mutex_unlock(&stageLock);

            if (currentPerformers < 4) {
                int style = -1;
                switch (currentPerformance) {
                    case DANCER:
                        style = 0;
                        break;
                    case JUGGLER:
                        style = 1;
                        break;
                    case SOLOIST:
                        style = 2;
                        break;
                    case EMPTY:
                        //whatever
                        break;
                }
                pthread_cond_signal(&styleConditions[style]);
            }

            perform(*performer);

            pthread_mutex_lock(&stageLock);
            exitStage(*performer, stagePosition);
            pthread_mutex_unlock(&stageLock);

            if (currentPerformance == EMPTY) {
                pthread_mutex_lock(&stageLock);
                enum Style mostWaiting = max();
                currentPerformance = mostWaiting;
                int style = -1;
                switch (mostWaiting) {
                    case DANCER:
                        style = 0;
                        break;
                    case JUGGLER:
                        style = 1;
                        break;
                    case SOLOIST:
                        style = 2;
                        break;
                    case EMPTY:
                        //whatever
                        break;
                }
                pthread_cond_signal(&styleConditions[style]);
                pthread_mutex_unlock(&stageLock);
            }

            sleep(performer->performanceLength*6);
        } else {
            pthread_mutex_unlock(&stageLock);
            if (currentPerformers == 0) {
                enum Style mostWaiting = max();
                int style = -1;
                switch (performer->style) {
                    case DANCER:
                        style = 0;
                        break;
                    case JUGGLER:
                        style = 1;
                        break;
                    case SOLOIST:
                        style = 2;
                        break;
                    case EMPTY:
                        //whatever
                        break;
                }
                pthread_cond_broadcast(&styleConditions[style]);
            }
            pthread_mutex_lock(&waitingLock);
            int style = -1;
            switch (performer->style) {
                case DANCER:
                    style = 0;
                    dancerWaiting++;
                    break;
                case JUGGLER:
                    style = 1;
                    jugglerWaiting++;
                    break;
                case SOLOIST:
                    style = 2;
                    soloistWaiting++;
                    break;
                case EMPTY:
                    //whatever
                    break;
            }
            pthread_mutex_unlock(&waitingLock);
            pthread_cond_wait(&styleConditions[style], &allMutexes[performer->threadNumber]);
            pthread_mutex_lock(&waitingLock);
            switch (performer->style) {
                case DANCER:
                    dancerWaiting--;
                    break;
                case JUGGLER:
                    jugglerWaiting--;
                    break;
                case SOLOIST:
                    soloistWaiting--;
                    break;
                case EMPTY:
                    //whatever
                    break;
            }
            pthread_mutex_unlock(&waitingLock);
            pthread_mutex_unlock(&allMutexes[performer->threadNumber]);
        }
    }
}

int checkStage(struct Performer performer) {
        if ((currentPerformance == performer.style || currentPerformance == EMPTY) && currentPerformers < 4) {
            for (int i = 0; i < 4; i++) {
                if (!stage[i]) {
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
    printf("%s is Performing!\n", performer.name);
    sleep(performer.performanceLength);
}

int enterStage(struct Performer performer) {

    int openSpot = -1;

    if ((currentPerformance == performer.style || currentPerformance == EMPTY) && currentPerformers < 4) {
        for (int i = 0; i < 4; i++) {
            if (!stage[i]) {
                openSpot = i;
                break;
            }
        }
    }

    if (openSpot != -1) {
        currentPerformers++;
        if (currentPerformance == EMPTY) {
            currentPerformance = performer.style;
        }

        if (performer.style != SOLOIST) {
            stage[openSpot] = FULL_SPOT;
        } else {
            currentPerformers+=3;
            stage[0] = FULL_SPOT;
            stage[1] = FULL_SPOT;
            stage[2] = FULL_SPOT;
            stage[3] = FULL_SPOT;
        }
        printf("%s is entering stage position %d\n", performer.name, openSpot+1);

        return openSpot;
    } else {
        return -1;
    }
}

void exitStage(struct Performer performer, int stagePosition) {
    performer.currentLocation = OFF_STAGE;
    if (performer.style != SOLOIST) {
        stage[stagePosition] = EMPTY_SPOT;
    } else {
        stage[0] = EMPTY_SPOT;
        stage[1] = EMPTY_SPOT;
        stage[2] = EMPTY_SPOT;
        stage[3] = EMPTY_SPOT;
        currentPerformers-=3;
    }
    printf("%s is now exiting stage positon %d!\n", performer.name, stagePosition+1);

    currentPerformers--;
    if (currentPerformers == 0) {
        currentPerformance = EMPTY;
    } else {
        int style;
        switch (performer.style) {
            case DANCER:
                style = 0;
                break;
            case JUGGLER:
                style = 1;
                break;
            case EMPTY:
            default:
                style = 2;
                break;
        }
        pthread_cond_signal(&styleConditions[style]);
    }
}

enum Style max() {
    if (soloistWaiting >= 1) {
        return SOLOIST;
    } else if (jugglerWaiting > dancerWaiting/2 || jugglerWaiting == 8){
        return JUGGLER;
    } else {
        return DANCER;
    }
}
