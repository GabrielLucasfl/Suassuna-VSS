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

#include "role_goalkeeper.h"

Role_Goalkeeper::Role_Goalkeeper() {
}

QString Role_Goalkeeper::name() {
    return "Role_Goalkeeper";
}

void Role_Goalkeeper::configure() {
    // Starting behaviors
    _bhv_moveTo = new Behavior_MoveTo();
    _bhv_intercept = new Behavior_Intercept();

    // Adding behaviors to behaviors list
    addBehavior(BHV_MOVETO, _bhv_moveTo);
    addBehavior(BHV_INTERCEPT, _bhv_intercept);
}

void Role_Goalkeeper::run() {
    // Fixed variables
    Position ballPosition = getWorldMap()->getBall().getPosition();
    Velocity ballVelocity = getWorldMap()->getBall().getVelocity();
    Position invalidPosition(false, 0.0f, 0.0f);

    // Taking the position where the GK wait for
    Position standardPosition;
    if (getWorldMap()->getLocations()->ourSide().isRight()) {
        standardPosition = Position(true, 0.69f, 0.0f);
    } else {
        standardPosition = Position(true, -0.69f, 0.0f);
    }

    // Reference position to look at
    Position lookingPosition(true, standardPosition.x(), 2.0f);

    if (!getWorldMap()->getLocations()->isInsideOurArea(player()->position())
            || getWorldMap()->getLocations()->isInsideTheirField(ballPosition)) {
        // Get a break at the standard position if the ball is far away or if the player is outside our goal area
        _bhv_moveTo->setTargetPosition(standardPosition);
        setBehavior(BHV_MOVETO);
    }
    else if (getWorldMap()->getLocations()->isInsideOurArea(ballPosition) && ballVelocity.abs() < 0.01f) {
        // Clear the ball if it is stationed at our goal area (or almost stationed)
        _bhv_moveTo->setTargetPosition(ballPosition);
        //_bhv_moveTo->setMinimalVelocity(1.0);
        setBehavior(BHV_MOVETO);
    }
    else if (!player()->isLookingTo(lookingPosition, 0.3f)) {
        // Rotates to a better angle of movement
        _bhv_moveTo->setTargetPosition(invalidPosition);
//        _bhv_moveTo->setAngularSpeed(4.0);
        setBehavior(BHV_MOVETO);
    } else {
        // Intercept the ball movement in order to prevent a goal
        Position firstLimitationPoint(true, standardPosition.x(), 0.3f);
        Position secondLimitationPoint(true, standardPosition.x(), -0.3f);
        _bhv_intercept->setInterceptSegment(firstLimitationPoint, secondLimitationPoint);
        _bhv_intercept->setObjectPosition(ballPosition);
        _bhv_intercept->setObjectVelocity(ballVelocity);
        setBehavior(BHV_INTERCEPT);
    }
}

QPair<Position, Angle> Role_Goalkeeper::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    Position standardPosition;
    if (getWorldMap()->getLocations()->ourSide().isRight()) {
        standardPosition = Position(true, 0.69f, 0.0f);
    } else {
        standardPosition = Position(true, -0.69f, 0.0f);
    }

    Position foulPosition;
    Angle foulAngle;

    switch (foul) {
    case VSSRef::Foul::PENALTY_KICK: {
        if (VSSRef::Color(getConstants()->teamColor()) == forTeam) {
            foulPosition = standardPosition;
            foulAngle = Angle(true, 90);
        } else {
            foulPosition = standardPosition;
            foulAngle = Angle(true, 0);
        }
    } break;
    case VSSRef::Foul::KICKOFF: {
        foulPosition = standardPosition;
        foulAngle = Angle(true, 90);
    } break;
    case VSSRef::Foul::FREE_BALL: {
        foulPosition = standardPosition;
        foulAngle = Angle(true, 90);
    } break;
    case VSSRef::Foul::GOAL_KICK: {
        if (static_cast<VSSRef::Color>(getConstants()->teamColor()) == forTeam) {
            if (atQuadrant == VSSRef::Quadrant::QUADRANT_2 || atQuadrant == VSSRef::Quadrant::QUADRANT_3) {
                foulPosition = Position(true, standardPosition.x(), 0.2f);
                foulAngle = Angle(true, 135);
            } else {
                foulPosition = Position(true, standardPosition.x(), -0.2f);
                foulAngle = Angle(true, 45);
            }
        } else {
            foulPosition = standardPosition;
            foulAngle = Angle(true, 90);
        }
    } break;
    default: {
        foulPosition = standardPosition;
        foulAngle = Angle(true, 90);
    }
    }

    return QPair<Position, Angle>(foulPosition, foulAngle);
}
