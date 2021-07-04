#include "ballobject.h"
#include <src/utils/utils.h>

BallObject::BallObject()
{

}

Position BallObject::predictNextPosition(int cycles) {
    float work, energy, force, ds, totalDs=0;
    Velocity velocity = _velocity;
    float vel = velocity.abs();
    float nextVel = vel;
    Position pos = _position;

    //  If object isn't moving
    if(vel < 0.01f) {
        return pos;
    }
    // Based on FIRASim
    force = getForce();
    for(int c = 0; c < cycles; c++) {
        ds = vel * _dt; // displacement
        totalDs += ds;  // total displacement
        work = force * ds;  // work = force * displacement
        energy = (_mass * powf(vel, 2))/2;  // E = m*v^2/2
        energy -= work;
        if(energy <= 0) {
            break;
        }
        nextVel = sqrt(2*energy/_mass);
        if(nextVel <= 0) {
            break;
        }
        vel = nextVel;
    }
    // Update position adding the total displacement estimated
    float posX = (velocity.vx()/velocity.abs()) * totalDs + pos.x();
    float posY = (velocity.vy()/velocity.abs()) * totalDs + pos.y();
    return Position(true, posX, posY);
}

Position BallObject::getMatch(Object obj) {
    Position objPos = obj.getPosition();
    float objVel = obj.getMavgVelocity().abs(); // Moving average velocity
    bool foundMatch = false;
    int cycles = 0;
    Position thisPos = _position;
    Position lastPos = thisPos;
    float distObjThis = Utils::distance(objPos, thisPos) - 0.06f;   // Distance - (robotRadius+ballRadius)
    if(distObjThis <= 0) {
        thisPos = getPredPosition();
        foundMatch = true;
    }
    while(!foundMatch) {
        distObjThis = Utils::distance(objPos, thisPos) - 0.06f; // Distance - (robotRadius+ballRadius)
        float spentTime = distObjThis/objVel;   // Time player would take to reach ball
        int cycleDiff = (static_cast<int>(std::ceil(spentTime/_dt)) - cycles);
        if(cycleDiff < 0) {
            return thisPos;
        }
        // If match position is outside field or many cycles have been considered
        if((abs(thisPos.x()) > 0.75f || abs(thisPos.y()) > 0.65f) || cycles > 270) {
            return lastPos;
        }
        cycles++;
        lastPos = thisPos;
        thisPos = getPredPosition(cycles);
    }
    return thisPos;
}
