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

#ifndef BEHAVIOR_BARRIER_H
#define BEHAVIOR_BARRIER_H

#include <src/entities/coach/behavior/behavior.h>
#include <src/entities/coach/skill/skills.h>

class Behavior_Barrier : public Behavior
{
public:
    Behavior_Barrier();
    QString name();

    // Setters
    void setTargetPosition(Position targetPosition) { _targetPosition = targetPosition; }
    void setBaseSpeed(float baseSpeed) { _desiredBaseSpeed = baseSpeed; }
    void setAvoidFlags(bool avoidBall, bool avoidTeammates, bool avoidOpponents, bool avoidOurGoalArea, bool avoidTheirGoalArea);
    void setEllipseAxis(float ellipseAxis_X, float ellipseAxis_Y) { _ellipseAxis_X = ellipseAxis_X; _ellipseAxis_Y = ellipseAxis_Y; }
    void enableRotation(bool isRotationEnabled) { _isRotationEnabled = isRotationEnabled; }

    void setRadius(float radius) {_radius = radius;}

private:
    // Behavior inherited methods
    void configure();
    void run();

    // Skills enum
    enum {
        SKILL_GOTO,
        SKILL_ROTATE,
        SKILL_SPIN
    };

    // Skills pointers
    Skill_GoTo *_skill_goTo;
    Skill_RotateTo *_skill_rotateTo;
    Skill_Spin *_skill_spin;

    // Parameters
    Position _targetPosition;
    float _desiredBaseSpeed;
    bool _isRotationEnabled;
    float _ellipseAxis_X;
    float _ellipseAxis_Y;


    //default _radius value
    float _radius;

    // Internal functions
    Position projectPosOutsideGoalArea(Position pos, bool avoidOurArea, bool avoidTheirArea);
    bool spinOrientarion();

    // Avoid Parameters
    bool _avoidTeammates;
    bool _avoidOpponents;
    bool _avoidBall;
    bool _avoidOurGoalArea;
    bool _avoidTheirGoalArea;
};

#endif // BEHAVIOR_BARRIER_H
