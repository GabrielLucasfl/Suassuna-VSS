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
    void setBaseSpeed(float desiredBaseSpeed) { _desiredBaseSpeed = desiredBaseSpeed; }
    void setOffsetBehindBall(float offsetBehindBall) { _offsetBehindBall = offsetBehindBall; }
    void setReferencePosition(Position referencePosition) {_referencePosition = referencePosition; }    
    void setAvoidFlags(bool avoidBall, bool avoidTeammates, bool avoidOpponents, bool avoidOurGoalArea, bool avoidTheirGoalArea);

    // Auxiliar
    Position getBallProjection();

private:
    // Behavior inherited methods
    void configure();
    void run();

    // Skills enum
    enum {
        SKILL_GOTO
    };

    // Skills pointers
    Skill_GoTo *_skill_goTo;

    // Parameters
    Position _targetPosition;
    float _desiredBaseSpeed;
    float _offsetBehindBall;
    Position _referencePosition;

    // Avoid Parameters
    bool _avoidTeammates;
    bool _avoidOpponents;
    bool _avoidBall;
    bool _avoidOurGoalArea;
    bool _avoidTheirGoalArea;
};


#endif // BEHAVIOR_GOTOBALL_H
