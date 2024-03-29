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

#define GKFACTOR 0.2f

Role_Goalkeeper::Role_Goalkeeper() {
    _gkOverlap = false;
    _isOverlapTimerInit = false;

    _ellipseParameters.first = 0.0f;
    _ellipseParameters.second = 0.0f;
    _ellipseCenter = Position(false, 0.0f, 0.0f);
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
    Velocity ballVelocity = getWorldMap()->getBall().getVelocity();
    Position ballPos = getWorldMap()->getBall().getPosition();
    Position ballPred = getWorldMap()->getBall().getPredPosition(3 * getConstants()->predictionBaseCycles() / 4);

    Position ballDirection;
    if(ballVelocity.abs() > 0) {
        ballDirection = Position(true, ballVelocity.vx()/ballVelocity.abs(), ballVelocity.vy()/ballVelocity.abs());
    } else {
        ballDirection = Position(true, 0, 0);
    }
    float factor = std::min(GKFACTOR * ballVelocity.abs(), 0.5f);
    Position ballProjection = Position(true, ballPos.x() + factor*ballDirection.x(), ballPos.y() + factor*ballDirection.y());
    ballPred = ballProjection;
    // Spin minimal distance from ball
    float distSpin = 0.09f;
    // max vx = 2.8  min vx = 0
    float maxPlus = 0.16f;
    distSpin += maxPlus*(ballVelocity.abs()/2.8f);
    // 0.09 + 0.16 * (ballVelocity / 2.8)

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
    _bhv_moveTo->enableSpin(false);

    if (_gkOverlap) {
        if(!_isOverlapTimerInit) {
            _overlapTimer.start();
            _isOverlapTimerInit = true;
        }
        player()->setPlayerDesiredPosition(getWorldMap()->getBall().getPosition());
        _bhv_moveTo->setBaseSpeed(getConstants()->playerBaseSpeed());
        setBehavior(BHV_MOVETO);

        _overlapTimer.stop();


        if(Utils::distance(player()->position(), getWorldMap()->getLocations()->ourGoal()) > 0.38f) {
            _gkOverlap = false;
        }

        if(Utils::distance(player()->position(), getWorldMap()->getBall().getPosition()) > 0.38f) {
            _gkOverlap = false;
        }

        if (_overlapTimer.getSeconds() > 1.0) {
            _isOverlapTimerInit = false;
            _gkOverlap = false;
        }
    } else {
        if(Utils::distance(player()->position(), ballPred) < 0.8f*distSpin) {
            _bhv_moveTo->setSpinOrientation(spinOrientarion());
            _bhv_moveTo->enableSpin(true);
            setBehavior(BHV_MOVETO);
        }
        else if (getWorldMap()->getLocations()->isInsideOurArea(ballPos) && ballVelocity.abs()<0.2f){
            //the ball is inside the goalkeeper's area and is moving very slow, so the gk needs to get the ball out to avoid danger of goal
            if(getWorldMap()->getLocations()->ourSide().isRight() && (player()->position().x() >= ballPos.x())){//check if the gk is behind the ball
                player()->setPlayerDesiredPosition(ballPos);
                setBehavior(BHV_MOVETO);
            }
            else if(getWorldMap()->getLocations()->ourSide().isLeft() && (player()->position().x() <= ballPos.x())){
                player()->setPlayerDesiredPosition(ballPos);
                setBehavior(BHV_MOVETO);
            }
        }
        else if(isBallInsideEllipse() && ballVelocity.abs() < 0.25f && ballVelocity.vy() < 0.2f) {
            player()->setPlayerDesiredPosition(ballPred);
            setBehavior(BHV_MOVETO);
        }
        else if (!getWorldMap()->getLocations()->isInsideOurArea(player()->position())
                || getWorldMap()->getLocations()->isInsideTheirField(ballPred)) {
            // Get a break at the standard position if the ball is far away or if the player is outside our goal area
            player()->setPlayerDesiredPosition(standardPosition);
            setBehavior(BHV_MOVETO);
        }
        else if((getWorldMap()->getLocations()->ourSide().isLeft()? ballVelocity.vx() > 0 : ballVelocity.vx() < 0)
                 && !isBallInsideEllipse()) {
            player()->setPlayerDesiredPosition(standardPosition);
            setBehavior(BHV_MOVETO);
        }
        else if (!player()->isLookingTo(lookingPosition, 0.17f)) {
            // Rotates to a better angle of movement
            player()->setPlayerDesiredPosition(lookingPosition);
            _bhv_moveTo->enableRotation(true);
            setBehavior(BHV_MOVETO);
        }
        else {
            if ((ballPred.x() > 0.6f && getWorldMap()->getLocations()->ourSide().isRight()) ||
                    (ballPred.x() < -0.6f && getWorldMap()->getLocations()->ourSide().isLeft())) {
                if (ballPred.y() > 0.35f) {
                    player()->setPlayerDesiredPosition(Position(true, standardPosition.x(), 0.3f));
                    setBehavior(BHV_MOVETO);
                }
                else if (ballPred.y() < -0.35f) {
                    player()->setPlayerDesiredPosition(Position(true, standardPosition.x(), -0.3f));
                    setBehavior(BHV_MOVETO);
                }
            }
            else if(Utils::distance(ballPred, getWorldMap()->getLocations()->ourGoal()) < 0.38f){
                float _limity = ballPred.y();
                Utils::limitValue(&_limity, -0.2f, 0.2f);
                player()->setPlayerDesiredPosition(Position(true, standardPosition.x(), _limity));
                setBehavior(BHV_MOVETO);
            } else {
                // Intercept the ball movement in order to prevent a goal

                Position firstLimitationPoint(true, standardPosition.x(), 0.2f);
                Position secondLimitationPoint(true, standardPosition.x(), -0.2f);
                float factorRangeGK = 0.7f*0.2f*(1 - (abs(ballPred.y())/getWorldMap()->getLocations()->fieldMaxY()));
                if (ballPred.y() > 0.0f) {
                    secondLimitationPoint.setPosition(true, standardPosition.x(), -factorRangeGK);
                } else {
                    firstLimitationPoint.setPosition(true, standardPosition.x(), factorRangeGK);
                }
                _bhv_intercept->setInterceptSegment(firstLimitationPoint, secondLimitationPoint);
                _bhv_intercept->setObjectPosition(ballPred);
                _bhv_intercept->setObjectVelocity(ballVelocity);
                _bhv_intercept->setBaseSpeed(getConstants()->playerBaseSpeed());
                setBehavior(BHV_INTERCEPT);
            }
        }
    }
}

bool Role_Goalkeeper::isBallInsideEllipse(){
    Position ballPred = getWorldMap()->getBall().getPredPosition(3 * getConstants()->predictionBaseCycles() / 4);
    float aux = pow((ballPred.x() - _ellipseCenter.x()), 2) / _ellipseParameters.first + pow(ballPred.y(), 2)
            / _ellipseParameters.second;
    if(aux >= 1){
        //std::cout << "Fora\n";
        return false;
    }
    else{
        //std::cout << "Dentro\n";
        return true;
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
            _isOverlapTimerInit = false;
            _overlapTimer.start();
            if (getWorldMap()->getLocations()->ourSide().isLeft()) {
                foulPosition = Position(true, -0.7125f, 0.0f);
                foulAngle = Angle(true, 0);
            } else {
                foulPosition = Position(true, 0.7125f, 0.0f);
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
            _isOverlapTimerInit = false;
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
