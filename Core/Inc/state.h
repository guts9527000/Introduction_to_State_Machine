
#ifndef INTRODUCTION_TO_STATE_MACHINES_STATE_H
#define INTRODUCTION_TO_STATE_MACHINES_STATE_H

typedef void (*StateHandler)(void);

typedef struct {
    int state;
    StateHandler handler;
} State;

enum state {
    state_idle,
    state_short,
    state_long
};

void State_Process(int state);

#endif //INTRODUCTION_TO_STATE_MACHINES_STATE_H
