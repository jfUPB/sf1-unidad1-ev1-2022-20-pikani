#ifndef EVENTS_H
#define EVENTS_H

enum class BUTTONS{NONE, ONE_BTN, TWO_BTN};

typedef struct{
    bool trigger;
    BUTTONS whichButton;
} buttonEvt_t;

extern buttonEvt_t buttonEvt;

#endif