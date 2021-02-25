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

#ifndef ROLE_ATTACKER_H
#define ROLE_ATTACKER_H

#include <src/entities/coach/role/role.h>
#include <src/entities/coach/behavior/behaviors.h>

class Role_Attacker : public Role
{
public:
    Role_Attacker();
    QString name();

    // Auxiliar
    bool isBehindBall(Position posObjective);
    bool isBehindBallXcoord(Position pos);
    bool inRangeToPush(Position ballPos);
    float normAngle(float angleRadians);
private:
    // Role inherited methods
    void configure();
    void run();

    // Behaviors enum
    enum {
        BHV_MOVETO,
        BHV_GOTOBALL
    };

    // State machine
    int _state;
    enum {
        MOVETO,
        GOTOBALL
    };

    // Behaviors pointers
    Behavior_MoveTo *_bhv_moveTo;
    Behavior_GoToBall *_bhv_goToBall;

    // Attributes
    bool _push;

    // Replacer
    QPair<Position, Angle> getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant);

    //Enum for ourTeam vs theirTeam
    enum{
        THEIRTEAM,
        OURTEAM
    };
    //Penalty stuff
    void penaltyKick(quint8 _teamPriority, QPair<Position, Angle> *_penaltyPlacement);
    void goalKick(quint8 _teamPriority, QPair<Position, Angle> *_penaltyPlacement);
    QPair<Position, Angle> _penaltyPlacement;
    void kickOff(QPair<Position, Angle> *_penaltyPlacement);
    void freeBall(QPair<Position, Angle> *_penaltyPlacement, VSSRef::Quadrant quadrant);

};

#endif // ROLE_ATTACKER_H
