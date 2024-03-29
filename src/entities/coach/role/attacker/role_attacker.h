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
    bool inRangeToPush(Position ballPos, float postIncrement = 0.0f);
    float normAngle(float angleRadians);
    void setPriority(bool prior) { _prior = prior; }
private:
    // Role inherited methods
    void configure();
    void run();

    // Methods
    bool hasAllyInTheirArea();
    Position defineReferencePosition();

    // Behaviors enum
    enum {
        BHV_MOVETO,
        BHV_GOTOBALL
    };

    // State machine
    int _state;
    enum {
        MOVETO,
        GOTOBALL,
        AVOIDBALL
    };

    // Behaviors pointers
    Behavior_MoveTo *_bhv_moveTo;
    Behavior_GoToBall *_bhv_goToBall;

    // Attributes
    bool _push;
    bool _avoidTeammates;
    bool _avoidOpponents;
    bool _avoidBall;
    bool _avoidOurGoalArea;
    bool _avoidTheirGoalArea;
    float _offsetRange;
    bool _charge;
    bool _gameInterrupted;
    float _lastSpeed;
    bool _positiveAngle;
    bool _prior;

    // Timer
    Timer _interuption;

    // Replacer
    QPair<Position, Angle> getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant);

    //Enum for ourTeam vs theirTeam
    enum{
        THEIRTEAM,
        OURTEAM
    };
    //Penalty function
    void penaltyKick(quint8 _teamPriority, QPair<Position, Angle> *_placement);

    //GoalKick function
    void goalKick(quint8 _teamPriority, QPair<Position, Angle> *_placement);

    //Control placement variable
    QPair<Position, Angle> _placement;

    //KickOff function
    void kickOff(quint8 _teamPriority, QPair<Position, Angle> *_placement);

    //FreeBall Function
    void freeBall(QPair<Position, Angle> *_placement, VSSRef::Quadrant quadrant);

    //set push speed
    float pushSpeed(float playerBallDist);

    //get distance based on angle to ball
    float getDist(float angle);

    //get the target angle to use with threePoints method
    float getAngle(float angle);

    //get the right position to push the ball to their goal
    Position getPushPosition(Position ballPos);

};

#endif // ROLE_ATTACKER_H
