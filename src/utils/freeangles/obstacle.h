/***
 * Maracatronics Robotics
 * Federal University of Pernambuco (UFPE) at Recife
 * http://www.maracatronics.com/
 *
 * This file is part of Armorial project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <src/utils/types/object/object.h>

class Obstacle
{
public:
    Obstacle(float obstacleRadius = 0.09f);

    // Getters
    Position position();
    Angle initialAngle();
    Angle finalAngle();
    float radius();

    // Setters
    void setPosition(Position position);
    void setInitialAngle(Angle initialAngle);
    void setFinalAngle(Angle finalAngle);
    void setRadius(float radius);

    //
    void calcAnglesFrom(const Position &watcher, float radiusFactor = 1.0f);

    // Operator overload
    Obstacle &operator=(Object object);

private:
    // Internal
    Position _obstaclePosition;
    Angle _initialAngle;
    Angle _finalAngle;
    float _obstacleRadius;

    // Angle control
    void setInitialAngle(float value);
    void setFinalAngle(float value);
};

#endif // OBSTACLE_H
