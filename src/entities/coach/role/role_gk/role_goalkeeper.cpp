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
    _gkOverlap = false;
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
    Position ballPosition;
    Velocity ballVelocity = getWorldMap()->getBall().getVelocity();

    // Ball projection
    Position ballPos = getWorldMap()->getBall().getPosition();
    float velMod = ballVelocity.abs();

    Position ballDirection, ballProj;
    if(ballVelocity.abs() > 0) {
        ballDirection = Position(true, ballVelocity.vx()/velMod, ballVelocity.vy()/velMod);
    } else {
        ballDirection = Position(true, 0, 0);
    }
    float factor = 0.2f * velMod;
    factor = std::min(factor, 0.5f);
    ballProj = Position(true, ballPos.x() + factor*ballDirection.x(), ballPos.y() + factor*ballDirection.y());
    ballPosition = ballProj;

    // Spin minimal distance from ball
    float distSpin = 0.09f;
    float ballVelo = ballVelocity.abs();
    // max vx = 2.8  min vx = 0
    float maxPlus = 0.16f;
    distSpin += maxPlus*(ballVelo/2.8f);

    // Taking the position where the GK wait for
    Position standardPosition;
    if (getWorldMap()->getLocations()->ourSide().isRight()) {
        standardPosition = Position(true, 0.72f, 0.0f);
    } else {
        standardPosition = Position(true, -0.72f, 0.0f);
    }

    // Reference position to look at
    Position lookingPosition(true, standardPosition.x(), 2.0f);
    _bhv_moveTo->enableRotation(false);
    _bhv_moveTo->setBaseSpeed(getConstants()->playerBaseSpeed());
    _bhv_moveTo->setSpin(false);

    if (_gkOverlap) {
        _bhv_moveTo->setTargetPosition(getWorldMap()->getBall().getPosition());
        _bhv_moveTo->setBaseSpeed(getConstants()->playerBaseSpeed());

        _overlapTimer.stop();
        if (_overlapTimer.getSeconds() > 8.0) {
            _gkOverlap = false;
        }
    } else {
        if (!getWorldMap()->getLocations()->isInsideOurArea(player()->position())
                || getWorldMap()->getLocations()->isInsideTheirField(ballPosition)) {
            // Get a break at the standard position if the ball is far away or if the player is outside our goal area
            _bhv_moveTo->setTargetPosition(standardPosition);
            setBehavior(BHV_MOVETO);
        }
        else if (getWorldMap()->getLocations()->isInsideOurArea(ballPosition)) {
            // Clear the ball if it is stationed at our goal area (or almost stationed)
            if(Utils::distance(player()->position(), ballPosition) > distSpin) {
                _bhv_moveTo->setTargetPosition(ballPosition);
                setBehavior(BHV_MOVETO);
            }else {
                _bhv_moveTo->setSpinOrientation(spinOrientarion());
                _bhv_moveTo->setSpin(true);
                setBehavior(BHV_MOVETO);
            }
        }else if(getWorldMap()->getLocations()->isInsideTheirField(ballPos) && (getWorldMap()->getLocations()->ourSide().isLeft()? ballVelocity.vx() > 0 : ballVelocity.vx() < 0)) {
            _bhv_moveTo->setTargetPosition(standardPosition);
            setBehavior(BHV_MOVETO);
        }
        else if (!player()->isLookingTo(lookingPosition, 0.17f)) {
            // Rotates to a better angle of movement
            _bhv_moveTo->setTargetPosition(lookingPosition);
            _bhv_moveTo->enableRotation(true);
            setBehavior(BHV_MOVETO);
        }
        else {
            if (ballPosition.x() > 0.6f && getWorldMap()->getLocations()->ourSide().isRight()) {
                if (ballPosition.y() > 0.35f) {
                    _bhv_moveTo->setTargetPosition(Position(true, standardPosition.x(), 0.3f));
                    setBehavior(BHV_MOVETO);
                }
                else if (ballPosition.y() < -0.35f) {
                    _bhv_moveTo->setTargetPosition(Position(true, standardPosition.x(), -0.3f));
                    setBehavior(BHV_MOVETO);
                }
            }
            else if (ballPosition.x() < -0.6f && getWorldMap()->getLocations()->ourSide().isLeft()) {
                if (ballPosition.y() > 0.35f) {
                    _bhv_moveTo->setTargetPosition(Position(true, standardPosition.x(), 0.3f));
                    setBehavior(BHV_MOVETO);
                }
                else if (ballPosition.y() < -0.35f) {
                    _bhv_moveTo->setTargetPosition(Position(true, standardPosition.x(), -0.3f));
                    setBehavior(BHV_MOVETO);
                }
            } else {
                // Intercept the ball movement in order to prevent a goal
                Position firstLimitationPoint(true, standardPosition.x(), 0.2f);
                Position secondLimitationPoint(true, standardPosition.x(), -0.2f);
                _bhv_intercept->setInterceptSegment(firstLimitationPoint, secondLimitationPoint);
                _bhv_intercept->setObjectPosition(ballPosition);
                _bhv_intercept->setObjectVelocity(ballVelocity);
                _bhv_intercept->setBaseSpeed(getConstants()->playerBaseSpeed());
                setBehavior(BHV_INTERCEPT);
            }
        }
    }
}

bool Role_Goalkeeper::spinOrientarion() {
    Position ballPos = getWorldMap()->getBall().getPosition();
    Position playerPos = player()->position();
    if(getWorldMap()->getLocations()->ourSide().isRight()) {
        if(ballPos.y() > playerPos.y()) {
            return false;
        }else {
            return true;
        }
    }else {
        if(ballPos.y() > playerPos.y()) {
            return true;
        }else {
            return false;
        }
    }
}

QPair<Position, Angle> Role_Goalkeeper::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    Position standardPosition;
    if (getWorldMap()->getLocations()->ourSide().isRight()) {
        standardPosition = Position(true, 0.67f, 0.0f);
    } else {
        standardPosition = Position(true, -0.67f, 0.0f);
    }

    Position foulPosition;
    Angle foulAngle;

    _gkOverlap = false;
    _overlapTimer.stop();

    switch (foul) {
    case VSSRef::Foul::PENALTY_KICK: {
        if (VSSRef::Color(getConstants()->teamColor()) == forTeam) {
            foulPosition = standardPosition;
            foulAngle = Angle(true, static_cast<float>(M_PI) / 2);
        } else {
            _gkOverlap = true;
            _overlapTimer.start();
            foulPosition = standardPosition;
            if (getWorldMap()->getLocations()->ourSide().isLeft()) {
                foulAngle = Angle(true, 0);
            } else {
                foulAngle = Angle(true, static_cast<float>(M_PI));
            }
        }
    } break;
    case VSSRef::Foul::KICKOFF: {
        foulPosition = standardPosition;
        foulAngle = Angle(true, static_cast<float>(M_PI) / 2);
    } break;
    case VSSRef::Foul::FREE_BALL: {
        foulPosition = standardPosition;
        foulAngle = Angle(true, static_cast<float>(M_PI) / 2);
    } break;
    case VSSRef::Foul::GOAL_KICK: {
        if (static_cast<VSSRef::Color>(getConstants()->teamColor()) == forTeam) {
            _gkOverlap = true;
            _overlapTimer.start();
            Position ballPosition = getWorldMap()->getBall().getPosition();
            if (getWorldMap()->getLocations()->ourSide().isLeft()) {
                if (ballPosition.y() > 0.0f) {
                    //Quadrant 2
                    foulPosition = Position(true, standardPosition.x(), 0.28f);
                    foulAngle = Angle(true, static_cast<float>(M_PI) / 4);
                } else {
                    // Quadrant 3
                    foulPosition = Position(true, standardPosition.x(), -0.28f);
                    foulAngle = Angle(true, static_cast<float>(M_PI) * 7 / 4);
                }
            } else {
                if (ballPosition.y() > 0.0f) {
                    // Quadrant 1
                    foulPosition = Position(true, standardPosition.x(), 0.28f);
                    foulAngle = Angle(true, static_cast<float>(M_PI) * 3 / 4);
                } else {
                    //Quadrant 4
                    foulPosition = Position(true, standardPosition.x(), -0.28f);
                    foulAngle = Angle(true, static_cast<float>(M_PI) * 5 / 4);
                }
            }
        } else {
            foulPosition = standardPosition;
            foulAngle = Angle(true, static_cast<float>(M_PI) / 2);
        }
    } break;
    default: {
        foulPosition = standardPosition;
        foulAngle = Angle(true, static_cast<float>(M_PI) / 2);
    }
    }

    return QPair<Position, Angle>(foulPosition, foulAngle);
}
