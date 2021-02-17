//
// Created by Sullivan Mulhern on 2/13/21.
//
#ifndef ASSIGNMENT2_FEDOOPS_H
#define ASSIGNMENT2_FEDOOPS_H

#include <pthread.h>
#include <semaphore.h>

enum StationName {
    WEIGH = 0,
    DANCER = 1,
    JUGGLER = 2,
    SOLOIST = 3
};

struct Team {
  char* color;
  int currentWorker;
  int size;
};

struct Station {
  sem_t status;
  pthread_t workerThread;
  char* name;
};
struct Package {
    int id;
    int numInstructions;
    int* instructions;
    int nextInstruction;
};
struct PackageQueue {
    struct Package* packages;
    int size;
    int top;
}



#endif //ASSIGNMENT2_FEDOOPS_H
