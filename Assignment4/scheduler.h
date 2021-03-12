//
// Created by sjmulhern on 3/12/2021.
//

#ifndef ASSIGNMENT4_SCHEDULER_H
#define ASSIGNMENT4_SCHEDULER_H

struct job {
    int id;
    int length;
    int timeElapsed;
    struct job* next;
    short jobStart;
    short jobEnd;

};

void sort(struct job* start);

int inProgressJobs(struct job* start);

void analyze(struct job* start);

#endif //ASSIGNMENT4_SCHEDULER_H
