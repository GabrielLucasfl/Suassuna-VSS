#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <src/utils/types/object/object.h>

class BallObject : public Object
{
public:
    BallObject();
    Position getMatch(Object obj);
    Position getPredPosition(int cycles=1);
    Position getPredPosition(float interval);
protected:
    // Attributes
    float _mass = 0.043f;
    float _friction = 0.06f;
    float _gravity = 9.8f;
    float _slip = 0.09f;
    // Methods
    float getForce() { return _friction * _mass * _gravity * _slip; }
    Position predictNextPosition(int cycles=1);

};

#endif // BALLOBJECT_H
