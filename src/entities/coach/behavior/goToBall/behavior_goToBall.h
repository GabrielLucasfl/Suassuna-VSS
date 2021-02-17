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

#ifndef BEHAVIOR_GOTOBALL_H
#define BEHAVIOR_GOTOBALL_H

#include <src/entities/coach/behavior/behavior.h>
#include <src/entities/coach/skill/skills.h>
#include <src/entities/coach/player/player.h>

class Behavior_GoToBall : public Behavior
{
public:
    Behavior_GoToBall();
    QString name();

    // Setters
    void setMinimalVelocity(float minimalVelocity) { _minimalVelocity = minimalVelocity; }
    void setLinearSpeed(float linearSpeed) { _linearSpeed = linearSpeed; }
    void setAngularSpeed(float angularSpeed) { _angularSpeed = angularSpeed; }
    void setOffsetBehindBall(float offsetBehindBall) { _offsetBehindBall = offsetBehindBall; }
    void setReferencePosition(Position referencePosition) {_referencePosition = referencePosition; }
    // Auxiliar

    float getAngle(const Position &a, const Position &b);
    float angleDiff(const float A, const float B);
    bool isBehindBall(Position posObjective);
    Position threePoints(const Position &near, const Position &far, float distance, float beta);

private:
    // Behavior inherited methods
    void configure();
    void run();

    // Skills enum
    enum {
        SKILL_GOTO,
        SKILL_MOVE,
        Skill_PUSHBALL
    };

    // Skills pointers
    Skill_GoTo *_skill_goTo;
    Skill_Move *_skill_move;
    Skill_PushBall *_skill_pushball;

    // Parameters
    Position _targetPosition;
    float _minimalVelocity;
    float _linearSpeed;
    float _angularSpeed;
    float _offsetBehindBall;
    Position _referencePosition;
};


#endif // BEHAVIOR_GOTOBALL_H
