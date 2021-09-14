#include "playerstate.h"

PlayerState::PlayerState(quint8 id) {
    this->id = id;
    stuckState = false;
}

bool PlayerState::isStuck() {
    // If it was already stuck
    if(stuckState) {
        timer.stop();
        // If it is stuck for more than 2 seconds: return true
        if(timer.getSeconds() > 2) {
            return true;
        }
    }
    return false;
}
