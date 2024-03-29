#ifndef FREEANGLES_H
#define FREEANGLES_H
#include <src/entities/coach/coach.h>
#include <src/entities/coach/role/role.h>
#include <src/entities/coach/behavior/behaviors.h>
#include "obstacle.h"

class FreeAngles{
public:
    class Interval{
    private:
        float _angInitial;
        float _angFinal;
        bool _obstructed;
    public:
        Interval(){
            _angInitial = _angFinal = 0.0;
            _obstructed = false;
        }
        Interval(float angInitial, float angFinal, bool obstructed){
            _angInitial = angInitial;
            _angFinal = angFinal;
            _obstructed = obstructed;
        }

        float angInitial() const { return _angInitial; }
        float angFinal() const { return _angFinal; }
        bool obstructed() const { return _obstructed; }
    };

private:

    static QList<Interval> _getFreeAngles(const Position watcher, float initialPosAngle, float finalPosAngle, const QList<Obstacle> obstacles, bool returnObstructed = false);

    static QList<Obstacle> calcObstaclesObstruction(const Position &watcher, const QList<Obstacle> &obstacles);

public:

    //static QList<Interval> _getFreeAngles(const Position &watcher, float initialPosAngle, float finalPosAngle, const QList<Obstacle> &obstacles, bool returnObstructed = false);

    static QList<Obstacle> getObstacles(const Position watcher, float distanceRadius, QList<Object> playrs);

    static QList<Interval> getFreeAngles(const Position watcher, const Position initialPos, const Position finalPos, bool returnObstructed = false);
    static QList<Interval> getFreeAngles(const Position watcher, const Position initialPos, const Position finalPos, const QList<Obstacle> obstacles, bool returnObstructed = false);

    static QList<Interval> getFreeAngles(const Position watcher, float initialPosAngle, float finalPosAngle, bool returnObstructed = false);
    static QList<Interval> getFreeAngles(const Position watcher, float initialPosAngle, float finalPosAngle, const QList<Obstacle> obstacles, bool returnObstructed = false);
};
#endif // FREEANGLES_H
