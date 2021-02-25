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

#ifndef BEHAVIOR_INTERCEPT_H
#define BEHAVIOR_INTERCEPT_H

#include <src/entities/coach/behavior/behavior.h>
#include <src/entities/coach/skill/skills.h>

class Behavior_Intercept : public Behavior
{
public:
    Behavior_Intercept();
    QString name();

    // Setters
    void setInterceptSegment(Position firstPoint, Position secondPoint);
    void setBaseSpeed(float baseSpeed) { _baseSpeed = baseSpeed; }
    void setObjectPosition(Position objectPos) { _objectPos = objectPos; }
    void setObjectVelocity(Velocity objectVel) { _objectVel = objectVel; }
    void enableSpin(bool spinEnabled) { _spinEnabled = spinEnabled; }
    void setLinearError(float desiredLinearError) { _desiredLinearError = desiredLinearError; }

    // Getters
    Position getInterceptPosition() { return _interceptPos; }
    Velocity getInterceptVelocity() { return _interceptVel; }

private:
    // Behavior inherited methods
    void configure();
    void run();

    // Skills enum
    enum {
        SKILL_GOTO,
        SKILL_SPIN
    };

    // Skills pointers
    Skill_GoTo *_skill_goTo;
    Skill_Spin *_skill_spin;

    // Parameters
    Position _firstLimitationPoint;
    Position _secondLimitationPoint;
    Position _objectPos;
    Velocity _objectVel;
    Position _interceptPos;
    Velocity _interceptVel;
    float _baseSpeed;
    bool _spinEnabled;
    float _desiredLinearError;

    // Auxiliary Functions
    Position getOrthogonalProjection();
    Position getInterceptionPosition();
};

#endif // BEHAVIOR_INTERCEPT_H
