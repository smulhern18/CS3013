//
// Created by Sullivan Mulhern on 2/13/21.
//
/*
TODO:
Make teams not fixed to stations

*/
#include "fedOops.h"
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

FILE *seedFile;
int seed;
int packageIdCounter;

struct Station {
  sem_t status;
  pthread_t workerThread;
  char* name;
};

char stationLookup[4][7] = {"Weigh", "Barcode", "X-Ray", "Jostle"};
char teamLookup[4][6] = {"Red", "Blue", "Green", "Yellow"};
sem_t stationsStatus[4];

pthread_t stations[4];
sem_t packageQueueStatus;

sem_t conveyorStatus[6];
struct Package* conveyorContent[6];

sem_t workerStatus[4];
struct Package* workerContent[4];

struct PackageQueue pendingPackages;
struct Team teams[4];

int nextWorker(struct Team* team){
  if(team->currentWorker == team->size - 1){ //wrap around workers' queue
    team->currentWorker = 0;
  } else{
    team->currentWorker++;
  }
  return team->currentWorker;
}
// gives the list of indexes in conveyorStatus and conveyorContent
// which correspond to station
void getConveyors(int station, int* outputLoc){
  int list[3];
  int idx = 0;

  for(int i = 0; i < 3; i++){
    for(int j = 1; j < 4; j++){
      if((i == station || j == station) && i != j){
        if(i==0){
          list[idx] = i+j-1;
        } else{
          list[idx] = i+j;
        }
        idx++;
      }
    }
  }
  outputLoc = list;
}
// gives the list of indexes in workerStatus and workerContent
// which care not the worker of station
void getWaitingWorkers(int station, int* outputLoc) {
  int list[3];
  int idx = 0;
  for(int i = 0; i < 4; i++){
      if(i != station){
        list[idx] = i;
        idx++;
      }
  }
  outputLoc = list;
}


void* processPackage(void* args){
  struct Package* package = NULL;
  int i = *((int*)args); //station id

  //lock station (worker is busy)
  sem_wait(&stationsStatus[i]);
  //check incoming conveyors
  int conveyors[3];
  int idx;
  getConveyors(i, conveyors);
  for(int j = 0; j < 3; j++){
    idx = conveyors[j];
    sem_wait(&conveyorStatus[idx]);
    if(conveyorContent[idx] != NULL && conveyorContent[idx]->nextInstruction == i){ //at least one incoming package
      package = conveyorContent[idx];
      conveyorContent[idx] = NULL;
      sem_post(&conveyorStatus[idx]);
      break;
    }
    sem_post(&conveyorStatus[idx]);
  } //check waiting workers
  if(!package){
    int workers[3];
    int idx;
    getWaitingWorkers(i, workers);
    for(int j = 0; j < 3; j++){
      idx = workers[j];
      sem_wait(&workerStatus[idx]);
      if(workerContent[idx] != NULL && workerContent[idx]->nextInstruction == i){ //at least one incoming package
        package = workerContent[idx];
        workerContent[idx] = NULL;
        sem_post(&workerStatus[idx]);
        break;
      }
      sem_post(&workerStatus[idx]);
    }
  }

  //check to see if done
  while(pendingPackages.size > 0 || package){
    if(!package){ //incoming package from other station
      sem_wait(&packageQueueStatus); //get in line for package
      //pop package from queue
      package = &pendingPackages.packages[pendingPackages.top];
      pendingPackages.top++;
      pendingPackages.size--;
      sem_post(&packageQueueStatus); //get out of line
      printf("Worker %s_%d picks up package %d.\n", teams[i].color, teams[i].currentWorker, package->id);
      if(package->instructions[package->nextInstruction] != i){
        sem_wait(&workerStatus[i]);
        workerContent[i] = package;
        sem_post(&workerStatus[i]);
        package = NULL;
        int sem_value;
        sem_getvalue(&workerStatus[i], &sem_value);
        while (sem_value > 0) { //wait until package is delivered to proper location
            sem_getvalue(&workerStatus[i], &sem_value);
        }
        printf("Worker %s_%d delivered package %d to %s station.\n", teams[i].color, teams[i].currentWorker, package->id, stationLookup[i]);
        sem_post(&stationsStatus[i]); //unlock station
        printf("Worker %s_%d is now tagging out from %s station.\n", teams[i].color, teams[i].currentWorker, stationLookup[i]);
        nextWorker(&teams[i]);
        printf("Worker %s_%d is now tagging in to %s station.\n", teams[i].color, teams[i].currentWorker, stationLookup[i]);
        continue;
      }
      printf("Worker %s_%d returns to %s station with package %d.\n", teams[i].color, teams[i].currentWorker, stationLookup[i], package->id);
    }
    printf("Worker %s_%d picks up package %d.\n", teams[i].color, teams[i].currentWorker, package->id);
    printf("Worker %s_%d executes %s on package %d.\n", teams[i].color, teams[i].currentWorker, stationLookup[i], package->id);
    sleep(2);
    package->nextInstruction++;
    if (package->nextInstruction >= package->numInstructions){
      printf("All instructions completed on package %d.\n", package->id);
      printf("Worker %s_%d put package %d on the delivery truck.", teams[i].color, teams[i].currentWorker, package->id);
    }
    else{
      //gets the proper index between this station and next station
      int conveyorIdx;
      conveyorIdx = i + package->instructions[package->nextInstruction];
      if(i == 0 || package->instructions[package->nextInstruction] == 0){
        conveyorIdx--;
      }
      sem_wait(&conveyorStatus[i]);
      conveyorContent[i] = package;
      sem_post(&conveyorStatus[i]);
      package = NULL;
      int sem_value;
      sem_getvalue(&conveyorStatus[i], &sem_value);
      while (sem_value > 0) { //wait until package is delivered to proper location
          sem_getvalue(&conveyorStatus[i], &sem_value);
      }
      printf("Worker %s_%d put package %d on the conveyor belt to %s.", teams[i].color, teams[i].currentWorker, package->id, stationLookup[i]);

    }
    sem_post(&stationsStatus[i]); //unlock station

  }
}

int main(){

  seedFile = fopen("seed.txt", "r");
  fscanf(seedFile, "%d", &seed);
  srand(seed);
  packageIdCounter = 1;
  int n = 10;

  pendingPackages.size = n;
  pendingPackages.top = 0;
  pendingPackages.packages = (struct Package *) malloc (n * sizeof(struct Package));
  //generate random sets of instructions
  for (int i = 0; i < n; i++){
    int len = rand() % 4 + 1; //get number of instructions

    //generate randomly ordered instructions list (6 swaps)
    int seqInstructions[4] = {0, 1, 2, 4};
    for (int j = 0; j < rand() % 6; j++){
      int k = rand() % 4;
      int l = rand() % 4;
      int tmp = seqInstructions[k];
      seqInstructions[k] = seqInstructions[l];
      seqInstructions[l] = tmp;
    }
    int instructions[len]; //setup instructions list
    for(int j = 0; j < len; j++){
      instructions[j] = seqInstructions[j];
    }

    struct Package* p = & (struct Package) {.id = packageIdCounter, .numInstructions = len, .nextInstruction = 0, .instructions = instructions};

    pendingPackages.packages[i] = *p;
    packageIdCounter++;
  }


  for(int i = 0; i < 4; i++){ //init teams
    teams[i].color = teamLookup[i];
    teams[i].size = 10;
    teams[i].currentWorker = 0;
  }
  sem_init(&packageQueueStatus, 0, 1);
  printf("Starting shift...");
  for(int i = 0; i < 4; i++){ //init stations
    sem_init(&stationsStatus[i], 0, 1);
    printf("%s team assigned to %s station", teams[i].color, stationLookup[i]);  //TODO change so that team isnt fixed to station
  }
  for(int i = 0; i < 4; i++){//init threads
    void *p = &i;
    pthread_create(&stations[i], NULL, processPackage, p);
  }
  for(int i = 0; i < 4; i++){
    pthread_join(stations[i], NULL);
  }
  for(int i = 0; i < 4; i++){
  sem_destroy(&stationsStatus[i]);
  sem_destroy(&workerStatus[i]);
  }
  for(int i = 0; i < 6; i++){
  sem_destroy(&conveyorStatus[i]);
  }
  sem_destroy(&packageQueueStatus);

  free(pendingPackages.packages);

  return 0;
}
