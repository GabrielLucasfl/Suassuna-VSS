#include "freeangles.h"
#include <iostream>
#include <src/utils/utils.h>
#include <src/entities/coach/role/role.h>
#include <src/entities/coach/behavior/behaviors.h>
#include <src/entities/world/worldmap.h>
#include <src/entities/world/world.h>
#include <src/entities/coach/coordinator/coordinator.h>

#include "freeangles.h"

WorldMap* FreeAngles::_worldMap = nullptr;

void FreeAngles::setWorldMap(WorldMap *worldMap) {
    FreeAngles::_worldMap = worldMap;
}

WorldMap* FreeAngles::getWorldMap() {
    if(_worldMap == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("WorldMap with nullptr value at FreeAngles") + '\n';
    }
    else {
        return _worldMap;
    }

    return nullptr;
}

QList<Obstacle> FreeAngles::getObstacles(const Position &watcher, float distanceRadius) {
    // Check nullptr
    if(getWorldMap() == nullptr) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold("FreeAngles::getObstacles(args) WorldMap with nullptr.") + '\n';
        return QList<Obstacle>();
    }

    // Create list of obstacles
    QList<Obstacle> obstacles;

    for(int i = Colors::Color::YELLOW; i <= Colors::Color::BLUE; i++) {
        // Take list of available objets in the field
        QList<Object> objects = getWorldMap()->getAvailableObjects(Colors::Color(i));
        QList<Object>::iterator it;

        for(it = objects.begin(); it != objects.end(); it++) {
            // Take object data
            Object object = (*it);
            const Position objectPosition = (*it).getPosition();

            // Check if watcher == objectPosition
            if(!watcher.isInvalid() && watcher == objectPosition) {
                continue;
            }

            // Check radius
            if(!watcher.isInvalid()) {
                float distToPos = Utils::distance(watcher, objectPosition);
                if(distToPos > distanceRadius) {
                    continue;
                }
            }

            // Update info
            Obstacle obst;
            obst = object; // Overloaded operator (take position)

            // Push to list
            obstacles.push_back(obst);
        }
    }

    return obstacles;
}

QList<Obstacle> FreeAngles::getObstacles(const Position &watcher) {
    return FreeAngles::getObstacles(watcher, 1000.0f);
}

QList<Obstacle> FreeAngles::getObstacles() {
    return FreeAngles::getObstacles(Position(false, 0.0, 0.0), 1000.0f);
}

QList<AngleInterval> FreeAngles::getFreeAngles(const Position &watcher, const Position &initialPos, const Position &finalPos, bool returnObstructed) {
    float minPosAngle = Utils::getAngle(watcher, initialPos);
    float maxPosAngle = Utils::getAngle(watcher, finalPos);
    QList<Obstacle> obstacles = getObstacles(watcher);
    return _getFreeAngles(watcher, minPosAngle, maxPosAngle, obstacles, returnObstructed);
}

QList<AngleInterval> FreeAngles::getFreeAngles(const Position &watcher, const Position &initialPos, const Position &finalPos, const QList<Obstacle> &obstacles, bool returnObstructed) {
    float minPosAngle = Utils::getAngle(watcher, initialPos);
    float maxPosAngle = Utils::getAngle(watcher, finalPos);
    return _getFreeAngles(watcher, minPosAngle, maxPosAngle, obstacles, returnObstructed);
}

QList<AngleInterval> FreeAngles::getFreeAngles(const Position &watcher, float initialPosAngle, float finalPosAngle, bool returnObstructed) {
    QList<Obstacle> obstacles = getObstacles(watcher);
    return _getFreeAngles(watcher, initialPosAngle, finalPosAngle, obstacles, returnObstructed);
}

QList<AngleInterval> FreeAngles::getFreeAngles(const Position &watcher, float initialPosAngle, float finalPosAngle, const QList<Obstacle> &obstacles, bool returnObstructed) {
    return _getFreeAngles(watcher, initialPosAngle, finalPosAngle, obstacles, returnObstructed);
}

QList<AngleInterval> FreeAngles::_getFreeAngles(const Position &watcher, float initialPosAngle, float finalPosAngle, const QList<Obstacle> &obstacles, bool returnObstructed) {
    QMap<float, quint8> freeAngles;

    // Limit angle range (0-360)
    Utils::angleLimitZeroTwoPi(&initialPosAngle);
    Utils::angleLimitZeroTwoPi(&finalPosAngle);

    // Check conditions (init == final, init < final and init > final)
    if(initialPosAngle == finalPosAngle) {
        return QList<AngleInterval>();
    }
    else if(initialPosAngle < finalPosAngle) {
        freeAngles.insert(initialPosAngle, 0);
        freeAngles.insert(finalPosAngle, 0);
    }
    else {
        if(finalPosAngle == 0) {
            finalPosAngle = 2.0*M_PI;
            freeAngles.insert(initialPosAngle, 0);
            freeAngles.insert(finalPosAngle, 0);
        }
        else {
            freeAngles.insert(0, 0);
            freeAngles.insert(initialPosAngle, 0);
            freeAngles.insert(finalPosAngle, 0);
            freeAngles.insert(2.0*M_PI, 0);
        }
    }

    // Calc obstacles obstruction angles
    QList<Obstacle> tmpObstacles = calcObstaclesObstruction(watcher, obstacles);
    QList<Obstacle>::iterator it;

    // For each obstacle
    for(it = tmpObstacles.begin(); it != tmpObstacles.end(); it++) {
        // Take obstacle and its angles
        Obstacle obst = (*it);
        Angle obstInitialAngle = obst.initialAngle();
        Angle obstFinalAngle = obst.finalAngle();

        // Check obstacle angles (I > F)
        if(obstInitialAngle > obstFinalAngle) {
            Obstacle newObst1 = obst;
            newObst1.setInitialAngle(Angle(true, 0.0));
            newObst1.setFinalAngle(obst.finalAngle());

            Obstacle newObst2 = obst;
            newObst2.setInitialAngle(obst.initialAngle());
            newObst2.setFinalAngle(Angle(true, 2.0*M_PI));

            obst = newObst1;
            tmpObstacles.push_back(newObst2);
        }

        // Run in freeAngles to add obstructions
        QMap<float, quint8> tmpFreeAngles(freeAngles);
        int size = tmpFreeAngles.size();

        // Check size
        if(size % 2 == 0) {
            for(int fa = 0; fa < size; fa +=2) {
                const float initAngle = tmpFreeAngles.keys().at(fa);
                const float finalAngle = tmpFreeAngles.keys().at(fa+1);

                // Check intervals
                if(obst.initialAngle() < initAngle && obst.finalAngle() <= initAngle) {
                    // ok
                }
                else if(obst.initialAngle() <= initAngle && obst.finalAngle() > initAngle && obst.finalAngle() < finalAngle) {
                    freeAngles.remove(initAngle);
                    freeAngles.insert(obst.finalAngle().value(), 0);
                }
                else if(obst.initialAngle() > initAngle && obst.initialAngle() < finalAngle && obst.finalAngle() > initAngle && obst.finalAngle() < finalAngle) {
                    freeAngles.insert(obst.initialAngle().value(), 0);
                    freeAngles.insert(obst.finalAngle().value(), 0);
                }
                else if(obst.initialAngle() > initAngle && obst.initialAngle() < finalAngle && obst.finalAngle() >= finalAngle) {
                    freeAngles.remove(finalAngle);
                    freeAngles.insert(obst.initialAngle().value(), 0);
                }
                else if(obst.initialAngle() >= finalAngle && obst.finalAngle() > finalAngle) {
                    // ok
                }
                else if(obst.initialAngle() <= initAngle && obst.finalAngle() >= finalAngle) {
                    freeAngles.remove(initAngle);
                    freeAngles.remove(finalAngle);
                }
                else {
                    // never reach here
                }
            }
        }
    }

    // Construct return
    QList<AngleInterval> retn;
    for(int i = 0; i < freeAngles.size(); i++) {
        int nextI = i+1;
        if(nextI>=freeAngles.size()) {
            nextI = 0;
        }

        // Get info
        float initAngle = freeAngles.keys().at(i);
        float finalAngle = freeAngles.keys().at(nextI);
        bool obstructed = (i % 2) != 0;

        if(!obstructed || (obstructed && returnObstructed)) {
            // Remove initial==360 && final==0
            if(initAngle == 2.0*M_PI && finalAngle == 0.0f) {
                continue;
            }

            retn.push_back(AngleInterval(initAngle, finalAngle, obstructed));
        }
    }

    // Merge intervals
    if(!retn.empty()) {
        int size = retn.size();
        int i = 0;
        for(int c = 0; c < size+1; c++) {
            int nextI = i+1;
            if(nextI >= retn.size()) {
                nextI = 0;
            }

            // Check merge
            AngleInterval curr = retn.at(i);
            AngleInterval next = retn.at(nextI);
            if(((curr.finalAngle() == next.initialAngle()) || (next.initialAngle() == (curr.finalAngle().value() - 2.0*M_PI))) && curr.isObstructed() == next.isObstructed()) {
                retn.removeAt(i);
                retn.removeAt(nextI);
                retn.insert(i, AngleInterval(curr.initialAngle(), next.finalAngle(), curr.isObstructed()));
            }

            i++;
            if(i >= retn.size()) {
                i = 0;
            }
        }
    }

    return retn;
}

QList<Obstacle> FreeAngles::calcObstaclesObstruction(const Position &watcher, const QList<Obstacle> &obstacles) {
    // Iterate through obstacles and calc obstruction
    QList<Obstacle> retnObstacles;
    for(int i=0; i<obstacles.size(); i++) {
        Obstacle obst = obstacles.at(i);

        // Continue if obstacle is the watcher
        if(watcher == obst.position()) {
            continue;
        }

        obst.calcAnglesFrom(watcher);
        retnObstacles.push_back(obst);
    }

    return retnObstacles;
}




























