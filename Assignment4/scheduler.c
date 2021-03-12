#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"

char* policy;

int main(int argc, char **argv){

    if (argc != 4) {
        printf("Too few commandline arguments! %d were provided\n", (argc - 1));
        exit(0);
    }

    policy = argv[1];
    char* workloadFile = argv[2];
    int timeDuration = atoi(argv[3]);

    struct job* firstJob = (struct job*) malloc(sizeof(struct job));
    firstJob->id = 0;
    firstJob->length = -1;
    firstJob->next = NULL;
    firstJob->timeElapsed = 0;
    firstJob->jobStart = -1;
    firstJob->jobEnd = -1;

    FILE* workload = fopen(workloadFile, "r");
    if (workload == NULL) {
        printf("File not found!\n");
        exit(0);
    }

    int i = 0;
    struct job* currentJob = firstJob;
    char* line = (char*) calloc(sizeof(char), 32);

    while(fgets(line, 32, workload)) {
        if (i != 0) {
            struct job* nextJob = (struct job*) malloc(sizeof(struct job));
            nextJob->id = i;
            nextJob->length = atoi(line);
            nextJob->next = NULL;
            nextJob->timeElapsed = 0;
            nextJob->jobStart = -1;
            nextJob->jobEnd = -1;

            i ++;

            currentJob->next = nextJob;
            currentJob = nextJob;
        } else {
            currentJob->length = atoi(line);

            i++;
        }
    }

    printf("Execution trace with %s:\n", policy);
    int totalTime = 0;

    if (strcmp(policy, "FIFO") == 0) {
        int j = 0;
        currentJob = firstJob;
        while (currentJob != NULL) {
            currentJob->timeElapsed = currentJob->length;
            printf("Job %d ran for: %d\n", currentJob->id, currentJob->timeElapsed);
            currentJob->jobStart = totalTime;
            totalTime += currentJob->timeElapsed;
            currentJob->jobEnd = totalTime;
            currentJob = currentJob->next;
        }
    } else if (strcmp(policy, "SJF") == 0) {
        sort(firstJob);
        currentJob = firstJob;
        while (currentJob != NULL) {
            currentJob->timeElapsed = currentJob->length;
            printf("Job %d ran for: %d\n", currentJob->id, currentJob->timeElapsed);
            currentJob->jobStart = totalTime;
            totalTime += currentJob->timeElapsed;
            currentJob->jobEnd = totalTime;
            currentJob = currentJob->next;
        }
    } else if (strcmp(policy, "RR") == 0) {
        if (timeDuration <= 0) {
            printf("The timeslice duration cannot be less than or equal to zero!\n");
            exit(0);
        }
        short jobsNeedingWork = inProgressJobs(firstJob);
        currentJob = firstJob;
        while (jobsNeedingWork > 0) {
            int timeNeeded = currentJob->length - currentJob->timeElapsed;
            if (currentJob->jobStart == -1) {
                currentJob->jobStart = totalTime;
            }
            if (timeNeeded > timeDuration) {
                currentJob->timeElapsed+=timeDuration;
                printf("Job %d ran for: %d\n", currentJob->id, timeDuration);
                totalTime+=timeDuration;
            } else if (timeNeeded == 0) {
                // Do nothing
            } else if (timeNeeded <= timeDuration) {
                currentJob->timeElapsed = currentJob->length;
                printf("Job %d ran for: %d\n", currentJob->id, timeNeeded);
                jobsNeedingWork--;
                totalTime+=timeNeeded;
                currentJob->jobEnd = totalTime;
            }
            if (currentJob->next != NULL) {
                currentJob = currentJob->next;
            } else {
                currentJob = firstJob;
            }
        }
    } else {
        printf("Invalid policy! %s was supplied\n", policy);
        exit(0);
    }
    printf("End of execution with %s.\n", policy);
    analyze(firstJob);

    fclose(workload);
}

void sort(struct job* start) {
    int swapped;

    struct job* ptr;
    struct job* lptr = NULL;

    do {
        swapped = 0;
        ptr = start;

        while (ptr->next != lptr) {
            if (ptr->length > ptr->next->length) {
                int tempId = ptr->id;
                int tempLength = ptr->length;

                ptr->id = ptr->next->id;
                ptr->length = ptr->next->length;
                ptr->next->id = tempId;
                ptr->next->length = tempLength;
                swapped = 1;
            }
            ptr = ptr->next;
        }
        lptr = ptr;
    } while (swapped);
}

int inProgressJobs(struct job* start) {
    int count = 0;
    while (start != NULL) {
        count++;
        start = start->next;
    }
    return count;
}

void analyze(struct job* start) {
    printf("Begin analyzing %s:\n", policy);
    int count = 0;
    int responseTime = 0;
    int turnAroundTime = 0;
    int waitTime = 0;
    int responseTotal = 0;
    int turnTotal = 0;
    int waitTotal = 0;
    while(start != NULL) {
        responseTime = start->jobStart;
        turnAroundTime = start->jobEnd;
        waitTime = turnAroundTime - start->length;
        responseTotal += responseTime;
        turnTotal += turnAroundTime;
        waitTotal += waitTime;
        count++;
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", start->id, responseTime, turnAroundTime, waitTime);
        start = start->next;
    }
    double responseAvg = responseTotal/(double)count;
    double turnAvg = turnTotal/(double)count;
    double waitAvg = waitTotal/(double)count;

    printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\nEnd analyzing %s.\n", responseAvg, turnAvg, waitAvg, policy);
}