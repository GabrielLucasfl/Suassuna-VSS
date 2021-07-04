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

#ifndef OBJECT_H
#define OBJECT_H

#include <src/utils/types/position/position.h>
#include <src/utils/types/velocity/velocity.h>
#include <src/utils/types/angle/angle.h>
#include <src/entities/vision/filters/visionfilters.h>
#include <QReadWriteLock>

class Object
{
public:
    Object();
    ~Object();

    // Getters
    Position getPosition();
    Velocity getVelocity();
    Angle getOrientation();
    Velocity getAcceleration();
    Position getPredPosition(int cycles=1);
    Position getPredPosition(float interval);
    Velocity getMavgVelocity() { return _mavgVelocity; }

    // Filtering
    bool isObjectSafe();
    bool isObjectLoss();

    // Update
    void updateObject(float confidence, Position pos, Angle orientation = Angle(false, 0.0));
    void setInvalid();

protected:
    // Object params
    Position _position;
    Velocity _velocity;
    Velocity _mavgVelocity;
    Angle _orientation;
    Velocity _acceleration;
    float _dt;
    float _confidence;

    float _beta;
    // Object filters
    LossFilter _lossFilter;
    NoiseFilter _noiseFilter;
    KalmanFilter _kalmanFilter;

    // Other methods
    Position predictNextPosition(int cycles=1);
    void updateMavgVelocity();

    // Timer
    Timer _stepTimer;
};

#endif // OBJECT_H
