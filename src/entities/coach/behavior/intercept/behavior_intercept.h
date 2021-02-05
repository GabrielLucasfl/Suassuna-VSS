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

    void setInterceptSegment(Position firstPoint, Position secondPoint) { _firstLimitationPoint = firstPoint; _secondLimitationPoint = secondPoint; }
    void setDesiredVelocity(float velocityNeeded) { _velocityNeeded = velocityNeeded; }
    void setVelocityFactor(float velocityFActor) { _velocityFactor = velocityFActor; }
    void selectVelocityNeeded(bool activateVelocityNeeded) { _activateVelocityNeeded = activateVelocityNeeded; }
    Position getIntercetPosition() { return _objectPos; }
    void setObjectPosition (Position objectPos) { _objectPos = objectPos; }
    void setObjectVelocity (Velocity objectVel) { _objectVel = objectVel; }
    void setInterceptPosition (Position interceptPos) { _objectPos = interceptPos; }
    void setInterceptVelocity (Velocity interceptVel) { _objectVel = interceptVel; }

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
    Position _objectPos; // Position where the player should be
    Position _interceptPos;
    Velocity _objectVel;
    Velocity _interceptVel;
    float _velocityNeeded;
    float _velocityFactor;
    bool _activateVelocityNeeded;

    // Auxiliary Functions
    Position getOrthogonalProjection();
    Position getInterceptionPosition();
};

#endif // BEHAVIOR_INTERCEPT_H
