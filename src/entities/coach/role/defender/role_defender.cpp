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

#include "role_defender.h"

Role_Defender::Role_Defender() {
}

QString Role_Defender::name() {
    return "Role_Defender";
}

void Role_Defender::configure() {
    // Starting behaviors
    _bhv_moveTo = new Behavior_MoveTo();
    _bhv_intercept = new Behavior_Intercept();
    _bhv_barrier = new Behavior_Barrier();

    // Adding behaviors to behaviors list
    addBehavior(BHV_MOVETO, _bhv_moveTo);
    addBehavior(BHV_INTERCEPT, _bhv_intercept);
    addBehavior(BHV_BARRIER, _bhv_barrier);
}

void Role_Defender::run() {
    // Fixed variables
    Position ballPosition = getWorldMap()->getBall().getPosition();
    Velocity ballVelocity = getWorldMap()->getBall().getVelocity();
    Position invalidPosition(false, 0.0f, 0.0f);

    setBehavior(BHV_BARRIER);

//    // Taking the position where the GK wait for
//    Position standardPosition;
//    if (getWorldMap()->getLocations()->ourSide().isRight()) {
//        standardPosition = Position(true, 0.5f, 0.0f);
//    } else {
//        standardPosition = Position(true, -0.5f, 0.0f);
//    }

//    // Reference position to look at
//    Position lookingPosition(true, standardPosition.x(), 2.0f);

//    if (abs(player()->position().x() - standardPosition.x()) > 0.3f
//            || getWorldMap()->getLocations()->isInsideTheirField(ballPosition)) {
//        // Get a break at the standard position if the ball is far away or if the player is outside our goal area
//        _bhv_moveTo->setTargetPosition(standardPosition);
//        setBehavior(BHV_MOVETO);
//    }
//    else if (player()->getPlayerDistanceTo(ballPosition) < 0.20f && ballVelocity.abs() < player()->isLookingTo(ballPosition, 1.0f)) {
//        // Clear the ball if it is close enough and if it is in a good angle to deslocate
//        _bhv_moveTo->setTargetPosition(ballPosition);
//        //_bhv_moveTo->setMinimalVelocity(1.0);
//        setBehavior(BHV_MOVETO);
//    }
//    else if (!player()->isLookingTo(lookingPosition, 0.3f)) {
//        // Rotates to a better angle of movement
//        _bhv_moveTo->setTargetPosition(lookingPosition);
//        _bhv_moveTo->enableRotation(true);
//        setBehavior(BHV_MOVETO);
//    } else {
//        // Intercept the ball movement in order to prevent a danger play
//        Position firstLimitationPoint(true, standardPosition.x(), 0.65f);
//        Position secondLimitationPoint(true, standardPosition.x(), -0.65f);
//        _bhv_intercept->setInterceptSegment(firstLimitationPoint, secondLimitationPoint);
//        _bhv_intercept->setObjectPosition(ballPosition);
//        _bhv_intercept->setObjectVelocity(ballVelocity);
//        setBehavior(BHV_INTERCEPT);
//    }
}




QPair<Position, Angle> Role_Defender::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    // Standard position will be at our penalty mark
    Position standardPosition = Utils::threePoints(getWorldMap()->getLocations()->ourGoal(), getWorldMap()->getLocations()->ourPenaltyMark(), 0.2f, 0.0);
    Angle standardAngle = Angle(true, M_PI / 2.0);

    bool isForOurTeam = (forTeam == static_cast<VSSRef::Color>(getConstants()->teamColor()));

    switch(foul) {
        // In case of PENALTY_KICK, pos close to our field side
        case VSSRef::Foul::PENALTY_KICK: {
            if(isForOurTeam) {
                standardPosition = Utils::threePoints(getWorldMap()->getLocations()->fieldCenter(),
                                                      getWorldMap()->getLocations()->theirGoal(),
                                                      0.1f, M_PI);
                standardPosition = Position(true, standardPosition.x(),standardPosition.y() + 0.15);
                standardAngle = Angle(true, 0.0);
            }
            else {
                standardPosition = Utils::threePoints(getWorldMap()->getLocations()->fieldCenter(),
                                                      getWorldMap()->getLocations()->ourGoal(),
                                                      0.1f, M_PI);
                standardPosition = Position(true, standardPosition.x(),standardPosition.y() - 0.15);
                standardAngle = Angle(true, 0.0);
            }
        }
        break;
        // In case of OUR KICKOFF, it will be positioned close to attacker
        case VSSRef::Foul::KICKOFF: {
            if(isForOurTeam) {
                standardPosition = Utils::threePoints(getWorldMap()->getLocations()->fieldCenter(),
                                                      getWorldMap()->getLocations()->theirGoal(),
                                                      0.3f, M_PI);
                standardAngle = Angle(true, 0.0);
            }
        }
        break;
        case VSSRef::Foul::GOAL_KICK: {
            if(!isForOurTeam) {
                standardPosition = Utils::threePoints(getWorldMap()->getLocations()->theirPenaltyMark(),
                                                      getWorldMap()->getLocations()->theirGoalLeftMidPost(),
                                                      0.1f, M_PI);
                standardAngle = Angle(true, 0.0);
            }
        }
        break;
        // In free ball, pos at their penalty mark (if free ball occurs at their field side)
        // Otherwise, pos at standard position (barrier)
        case VSSRef::Foul::FREE_BALL: {
            if((atQuadrant == VSSRef::Quadrant::QUADRANT_1 || atQuadrant == VSSRef::Quadrant::QUADRANT_4) && getConstants()->teamSide().isLeft()) {
                standardPosition = getWorldMap()->getLocations()->theirPenaltyMark();
                standardAngle = Angle(true, 0.0);
            }
            else if((atQuadrant == VSSRef::Quadrant::QUADRANT_2 || atQuadrant == VSSRef::Quadrant::QUADRANT_3) && !getConstants()->teamSide().isLeft()) {
                standardPosition = getWorldMap()->getLocations()->theirPenaltyMark();
                standardAngle = Angle(true, 0.0);
            }
        }
        break;
    }

    return QPair<Position, Angle>(standardPosition, standardAngle);
}
