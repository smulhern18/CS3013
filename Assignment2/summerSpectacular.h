//
// Created by Sullivan Mulhern on 2/13/21.
//

#ifndef ASSIGNMENT2_SUMMERSPECTACULAR_H
#define ASSIGNMENT2_SUMMERSPECTACULAR_H

enum Style {
    EMPTY = 0,
    DANCER = 1,
    JUGGLER = 2,
    SOLOIST = 3
};

struct Performer {
    char* name;
    enum Style style;
    int performanceLength;
    int ready;
    int currentLocation;
};

void runThread(struct Performer performer);
int checkStage(struct Performer performer);
int checkStatus(struct Performer performer);
void perform(struct Performer performer);
void enter(struct Performer performer, int stagePosition);
void exitStage(struct Performer performer, int stagePosition);

#endif //ASSIGNMENT2_SUMMERSPECTACULAR_H
