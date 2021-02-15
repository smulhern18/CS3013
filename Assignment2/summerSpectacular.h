//
// Created by Sullivan Mulhern on 2/13/21.
//

#ifndef ASSIGNMENT2_SUMMERSPECTACULAR_H
#define ASSIGNMENT2_SUMMERSPECTACULAR_H

enum Style {
    EMPTY,
    DANCER,
    JUGGLER,
    SOLOIST
};

struct Performer {
    char* name;
    enum Style style;
    int performanceLength;
    int ready;
    int currentLocation;
};

void start(struct Performer performer);
int checkStage(struct Performer performer);
int checkStatus(struct Performer performer);
void perform(struct Performer performer);
void enter(struct Performer performer);
void exitStage(struct Performer performer);

#endif //ASSIGNMENT2_SUMMERSPECTACULAR_H
