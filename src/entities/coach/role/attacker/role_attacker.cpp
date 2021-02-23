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
    //setBehavior(BHV_GOTOBALL);
}

void Role_Attacker::run() {

    // Ball projection
    Position ballPos = getWorldMap()->getBall().getPosition();
    Velocity ballVel = getWorldMap()->getBall().getVelocity();
    Position ballDirection, ballProj;
    if(ballVel.abs() > 0 && !ballVel.isInvalid()) {
        ballDirection = Position(true, ballVel.vx()/ballVel.abs(), ballVel.vy()/ballVel.abs());
    } else {
        ballDirection = Position(true, 0, 0);
    }
    ballProj = Position(true, ballPos.x() + 0.05f*ballDirection.x(), ballPos.y() + 0.05f*ballDirection.y());

    // Bhv goToBall parameters
    float bhvGoToBallOffset = 0.2f;    // Distance behind ball
//    if(isBehindBallXcoord(player()->position())){
//        float offsetBasedOnDist = Utils::distance(ballProj, player()->position())*0.1f;
//        bhvGoToBallOffset = std::min(offsetBasedOnDist, 0.25f);
//        bhvGoToBallOffset = std::max(bhvGoToBallOffset, 0.2f);  // Distance behind ball
//    }

    Position bhvGoToBallRef = getWorldMap()->getLocations()->theirGoal();   // Reference position

    //check if player is behind ball based on its reference position
    bool isBehindBall = Role_Attacker::isBehindBall(Utils::threePoints(ballProj, bhvGoToBallRef, bhvGoToBallOffset, static_cast<float>(M_PI)));

    switch (_state) {
        case GOTOBALL: {
            std::cout << "GOTOBALL" << std::endl;
            _bhv_goToBall->setReferencePosition(getWorldMap()->getLocations()->theirGoal());
            _bhv_goToBall->setOffsetBehindBall(bhvGoToBallOffset);
            _bhv_goToBall->setAvoidFlags(true, true, true, true, false);
            std::cout << isBehindBall << std::endl;
            setBehavior(BHV_GOTOBALL);
            if(isBehindBall) {
                _state = MOVETO;
            }
            break;
        }
        case MOVETO: {
            std::cout << "MOVETO  " << std::endl;

            Position betweenBallAndRef = Utils::threePoints(bhvGoToBallRef, ballProj, Utils::distance(bhvGoToBallRef,ballProj) - 0.01f, 0);
            _bhv_moveTo->setTargetPosition(betweenBallAndRef);

            if(Utils::distance(player()->position(), ballPos) < 0.06f) {
                _bhv_moveTo->setBaseSpeed(50);
            }else {
                //_bhv_moveTo->setBaseSpeed(30);
            }
            setBehavior(BHV_MOVETO);

            //transitions
            if(Utils::distance(player()->position(), ballPos) >= 0.3f || !isBehindBallXcoord(player()->position())) {
                _state = GOTOBALL;
            }
            break;
        }
        default: {
            break;
        }
    }
}

bool Role_Attacker::isBehindBall(Position posObjective) {
    Position posBall = getWorldMap()->getBall().getPosition();
    Position posPlayer = player()->position();
    float anglePlayer = Utils::getAngle(posBall, posPlayer);
    float angleDest = Utils::getAngle(posBall, posObjective);
    float diff = Utils::angleDiff(anglePlayer, angleDest);

    bool isBehindObjX = isBehindBallXcoord(player()->position());

    return ((diff < static_cast<float>(M_PI)/30.0f) && isBehindObjX);
}

bool Role_Attacker::isBehindBallXcoord(Position pos) {
    Position posBall = getWorldMap()->getBall().getPosition();
    bool isBehindObjX;
    if(getWorldMap()->getLocations()->ourSide().isLeft()) {
        isBehindObjX = pos.x() < posBall.x();
    }else {
        isBehindObjX = pos.x() > posBall.x();
    }
    return isBehindObjX;
}

QPair<Position, Angle> Role_Attacker::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    //// TODO: Configure this in new roles!!!!!!!!!!!!!!!!!!!!!!!!!
    return QPair<Position, Angle>(Position(true, 0.0, 0.0), Angle(true, 0.0));
}
