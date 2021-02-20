#ifndef OBSTACLE_H
#define OBSTACLE_H
#include <src/entities/coach/coach.h>
#include <src/entities/coach/role/role.h>
#include <src/entities/coach/behavior/behaviors.h>


class Obstacle{
private:


public:
    Obstacle();
    Position _position;
    quint8 _team;
    float _initialAngle;
    float _finalAngle;
    float _radius;
    Position& position() { return _position; }
    quint8& team() { return _team; }
    float& initialAngle() { return _initialAngle; }
    float& finalAngle() { return _finalAngle; }
    float& radius() { return _radius; }

    void calcAnglesFrom(const Position watcher, float factor = 1.0f);

    Obstacle &operator=(const Obstacle &obstacle) {
        _position = obstacle._position;
        _team = obstacle._team;
        _initialAngle = obstacle._initialAngle;
        _finalAngle = obstacle._finalAngle;
        _radius = obstacle._radius;
        return *this;
    }
};

#endif // OBSTACLE_H
