//
// Created by Sullivan Mulhern on 2/13/21.
//

#ifndef ASSIGNMENT2_SUMMERSPECTACULAR_H
#define ASSIGNMENT2_SUMMERSPECTACULAR_H

#define ON_STAGE 1
#define OFF_STAGE 0
#define EMPTY_SPOT 0
#define FULL_SPOT 1

enum Style {
    EMPTY = 0,
    DANCER = 1,
    JUGGLER = 2,
    SOLOIST = 3
};

struct Performer {
    char name[32];
    enum Style style;
    int performanceLength;
    int ready;
    int currentLocation;
    int threadNumber;
};

void runThread(struct Performer* performer);
void perform(struct Performer performer);
int enterStage(struct Performer performer);
void exitStage(struct Performer performer, int stagePosition);
enum Style max();

#endif //ASSIGNMENT2_SUMMERSPECTACULAR_H
