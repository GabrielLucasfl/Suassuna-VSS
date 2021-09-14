#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include <src/utils/utils.h>
#include <src/utils/timer/timer.h>

class PlayerState {
public:
    PlayerState(quint8 id);
    quint8 id;
    Timer timer;
    bool stuckState;
    bool isStuck();
};

#endif // PLAYERSTATE_H
