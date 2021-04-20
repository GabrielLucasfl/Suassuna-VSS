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

#ifndef ROLE_SUPPORTER_H
#define ROLE_SUPPORTER_H

#include <src/entities/coach/role/role.h>
#include <src/entities/coach/behavior/behaviors.h>
#include <src/utils/freeangles/freeangles.h>
#include <src/utils/timer/timer.h>

class Role_Supporter : public Role
{
public:
    Role_Supporter();
    QString name();

private:
    // Role inherited methods
    void configure();
    void run();

    // Behaviors enum
    enum {
        BHV_MOVETO,
        BHV_INTERCEPT,
        BHV_BARRIER,
        BHV_GOTOBALL
    };

    // Behaviors pointers
    Behavior_MoveTo *_bhv_moveTo;
    Behavior_Intercept *_bhv_intercept;
    Behavior_Barrier *_bhv_barrier;
    Behavior_GoToBall *_bhv_goToBall;

    // Behaviors util functions
    float getLargestFreeAngle();
    bool inRangeToPush(Position ballPos);
    bool isBall_ourfield();
    float calcBarrier_Xcomponent();
    void limit_Ypos(float * posy);
    float calc_x_advanced();
    float calc_x_barrier();
    bool hasAllyInTheirArea();

    // Avoid Flags
    bool _avoidTeammates;
    bool _avoidOpponents;
    bool _avoidBall;
    bool _avoidOurGoalArea;
    bool _avoidTheirGoalArea;

    //vars
    float _posXbarrier;
    float _limitYup;
    float _limitYdown;
    bool _nofreeAngles;
    float _minVelocity;
    Timer _timer;
    bool _pushBall;
    bool _accelerate;
    bool _canAvoidBall;

    QPair<Position, Angle> getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant);

};

#endif // Role_Supporter_H
