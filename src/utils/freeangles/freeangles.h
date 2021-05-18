#ifndef FREEANGLES_H
#define FREEANGLES_H
#include <src/entities/coach/coach.h>
#include <src/entities/coach/role/role.h>
#include <src/entities/coach/behavior/behaviors.h>
#include <src/utils/freeangles/angleinterval/angleinterval.h>
#include "obstacle.h"

class FreeAngles
{
public:
    // WorldMap setter
    static void setWorldMap(WorldMap* worldMap);

    // Getters
    static QList<Obstacle> getObstacles();
    static QList<Obstacle> getObstacles(const Position &watcher);
    static QList<Obstacle> getObstacles(const Position &watcher, float distanceRadius);

    static QList<AngleInterval> getFreeAngles(const Position &watcher, const Position &initialPos, const Position &finalPos, bool returnObstructed = false);
    static QList<AngleInterval> getFreeAngles(const Position &watcher, const Position &initialPos, const Position &finalPos, const QList<Obstacle> &obstacles, bool returnObstructed = false);

    static QList<AngleInterval> getFreeAngles(const Position &watcher, float initialPosAngle, float finalPosAngle, bool returnObstructed = false);
    static QList<AngleInterval> getFreeAngles(const Position &watcher, float initialPosAngle, float finalPosAngle, const QList<Obstacle> &obstacles, bool returnObstructed = false);

private:
    // WorldMap pointer
    static WorldMap* _worldMap;
    static WorldMap* getWorldMap();

    // Internal
    static QList<AngleInterval> _getFreeAngles(const Position &watcher, float initialPosAngle, float finalPosAngle, const QList<Obstacle> &obstacles, bool returnObstructed = false);
    static QList<Obstacle> calcObstaclesObstruction(const Position &watcher, const QList<Obstacle> &obstacles);
};

#endif // FREEANGLES_H
