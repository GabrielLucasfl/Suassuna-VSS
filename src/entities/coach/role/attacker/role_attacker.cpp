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

#include "role_attacker.h"
#include <src/utils/utils.h>

Role_Attacker::Role_Attacker() {
}

QString Role_Attacker::name() {
    return "Role_Attacker";
}

void Role_Attacker::configure() {
    // Starting behaviors
    _bhv_moveTo = new Behavior_MoveTo();
    _bhv_goToBall = new Behavior_GoToBall();

    // Adding behaviors to behaviors list
    addBehavior(BHV_MOVETO, _bhv_moveTo);
    addBehavior(BHV_GOTOBALL, _bhv_goToBall);

    _state = GOTOBALL;
}

void Role_Attacker::run() {

    Position ballPos = getWorldMap()->getBall().getPosition();
    //check if player is behind ball
    bool isBehindBall = Role_Attacker::isBehindBall(player()->position());
    //ball possession
    bool ballPossession;
    if(player()->getPlayerDistanceTo(ballPos) < 0.04f) {
        ballPossession = true;
    } else {
        ballPossession = false;
    }

    switch (_state) {
        case GOTOBALL : {
            std::cout << "GOTOBALL" << std::endl;
            _bhv_goToBall->setReferencePosition(getWorldMap()->getLocations()->theirGoal());
            _bhv_goToBall->setOffsetBehindBall(0.06f);
            setBehavior(BHV_GOTOBALL);

            //transitions
            if(ballPossession && isBehindBall) {
                _state = MOVETO;
            }

        } break;
        case MOVETO : {
            std::cout << "MOVETO" << std::endl;
            _bhv_moveTo->setTargetPosition(Position(false,0,0));
            setBehavior(BHV_MOVETO);

            //transitions
            if(!ballPossession || !isBehindBall) {
                _state = GOTOBALL;
            }

        } break;
        default : {

        } break;
    }

    //_bhv_attacker->setTargetPosition(player()->getWorldMap()->getBall().getPosition());

    //_bhv_moveTo->setTargetPosition(player()->getWorldMap()->getBall().getPosition());

    //_behavior_moveTo->setMinimalVelocity();
    //_bhv_intercept->setInterceptSegment(Position(true, 0.6f, 0.35f), Position(true, 0.6f, -0.35f));
    //_bhv_intercept->setObjectPosition(player()->getWorldMap()->getBall().getPosition());
    //_bhv_intercept->setObjectVelocity(player()->getWorldMap()->getBall().getVelocity());

    setBehavior(BHV_GOTOBALL);
}

bool Role_Attacker::isBehindBall(Position posObjective) {
    Position posBall = getWorldMap()->getBall().getPosition();
    Position posPlayer = player()->position();
    float anglePlayer = Utils::getAngle(posBall, posPlayer);
    float angleDest = Utils::getAngle(posBall, posObjective);
    float diff = Utils::angleDiff(anglePlayer, angleDest);
    return (diff > static_cast<float>(M_PI)/18.0f);
}

QPair<Position, Angle> Role_Attacker::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    //// TODO: Configure this in new roles!!!!!!!!!!!!!!!!!!!!!!!!!
    return QPair<Position, Angle>(Position(true, 0.0, 0.0), Angle(true, 0.0));
}
