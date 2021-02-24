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
    _push = false;
    //setBehavior(BHV_GOTOBALL);
}

void Role_Attacker::run() {

    // Ball projection
    Position ballPos = getWorldMap()->getBall().getPosition();
    Velocity ballVel = getWorldMap()->getBall().getVelocity();
    Position ballDirection, ballProj;
    if(ballVel.abs() > 0 && !ballVel.isInvalid()) {
        ballDirection = Position(true, ballVel.vx(), ballVel.vy());
    } else {
        ballDirection = Position(true, 0, 0);
    }
    float factor = 5.0f * getWorldMap()->getBall().getVelocity().abs();
    factor = std::min(factor, 5.0f);
    ballProj = Position(true, ballPos.x() + factor*ballDirection.x(), ballPos.y() + factor*ballDirection.y());

    // Bhv goToBall parameters
    float bhvGoToBallOffset = 0.2f;    // Distance behind ball
    Position bhvGoToBallRef = getWorldMap()->getLocations()->theirGoal();   // Reference position

    //check if player is behind ball based on its reference position
    bool isInRange = inRangeToPush(ballProj) && (Utils::distance(ballProj, player()->position()) < 0.5f);
    bool isBehindBall = Role_Attacker::isBehindBall(Utils::threePoints(ballProj, bhvGoToBallRef, bhvGoToBallOffset, static_cast<float>(M_PI)));

    switch (_state) {
        case GOTOBALL: {
            _bhv_goToBall->setReferencePosition(getWorldMap()->getLocations()->theirGoal());
            _bhv_goToBall->setOffsetBehindBall(bhvGoToBallOffset);
            _bhv_goToBall->setAvoidFlags(true, true, true, true, false);
            setBehavior(BHV_GOTOBALL);
            if(isBehindBall || isInRange) {
                _state = MOVETO;
            }
            break;
        }
        case MOVETO: {
            Position betweenBallAndRef = Utils::threePoints(bhvGoToBallRef, ballProj, Utils::distance(bhvGoToBallRef, ballProj) - 0.01f, 0);
            if(!_push) {
                _bhv_moveTo->setBaseSpeed(getConstants()->playerBaseSpeed());
                _bhv_moveTo->setTargetPosition(betweenBallAndRef);
            }else {
                _bhv_moveTo->setBaseSpeed(50);
                _bhv_moveTo->setTargetPosition(betweenBallAndRef);
            }

            if(Utils::distance(player()->position(), ballProj) < 0.058f && !_push) {
                _push = true;
            }
            setBehavior(BHV_MOVETO);

            //transitions
            if(Utils::distance(player()->position(), ballProj) >= 0.3f || !isBehindBallXcoord(player()->position())) {
                _push = false;
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

    return ((diff < static_cast<float>(M_PI)/18.0f) && isBehindObjX);
}

bool Role_Attacker::isBehindBallXcoord(Position pos) {
    Position posBall = getWorldMap()->getBall().getPosition();
    float robotRadius = 0.035f;
    bool isBehindObjX;
    if(getWorldMap()->getLocations()->ourSide().isLeft()) {
        isBehindObjX = pos.x() < (posBall.x() - robotRadius);
    }else {
        isBehindObjX = pos.x() > (posBall.x() + robotRadius);
    }
    return isBehindObjX;
}

bool Role_Attacker::inRangeToPush(Position ballPos) {
    Position firstPost, secondPost;
    firstPost = getWorldMap()->getLocations()->theirGoalRightPost();
    secondPost = getWorldMap()->getLocations()->theirGoalLeftPost();

    // Angle from ball to posts of their goal
    float ballFirstPost = Utils::getAngle(ballPos, firstPost) - static_cast<float>(M_PI);
    ballFirstPost = normAngle(ballFirstPost);
    float ballSecondPost = Utils::getAngle(ballPos, secondPost) - static_cast<float>(M_PI);
    ballSecondPost = normAngle(ballSecondPost);

    // Angle from ball to player
    float ballPlayer = Utils::getAngle(ballPos, player()->position());

    // Compare angles to know if player is in the angle range to push the ball to their goal
    if(getWorldMap()->getLocations()->theirSide().isRight()) {
        if((ballSecondPost < 0 && ballFirstPost > 0) || (ballSecondPost > 0 && ballFirstPost < 0)) {
            if(ballPlayer > std::max(ballSecondPost, ballFirstPost)
                || ballPlayer < std::min(ballSecondPost, ballFirstPost)) {
                return true;
            }else {
                return false;
            }
        }
    }
    if(ballPlayer < std::max(ballSecondPost, ballFirstPost)
        && ballPlayer > std::min(ballSecondPost, ballFirstPost)) {
        return true;
    } else {
        return false;
    }

}

float Role_Attacker::normAngle(float angleRadians) {
    if (angleRadians > static_cast<float>(M_PI)) {
        return angleRadians = angleRadians - 2 * static_cast<float>(M_PI);
    } else if (angleRadians < -static_cast<float>(M_PI)) {
        return angleRadians = angleRadians + 2 * static_cast<float>(M_PI);
    } else {
        return angleRadians;
    }
}

QPair<Position, Angle> Role_Attacker::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    //// TODO: Configure this in new roles!!!!!!!!!!!!!!!!!!!!!!!!!
    return QPair<Position, Angle>(Position(true, 0.0, 0.0), Angle(true, 0.0));
}
