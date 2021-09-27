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
    _ellipseA = 0.0f;
    _ellipseB = 0.0f;
    _ellipseCenter = Position(false, 0.0f, 0.0f);
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
    Position desiredPosition;
    Position ballPosition = getWorldMap()->getBall().getPosition();
    Position ballPred = getWorldMap()->getBall().getPredPosition(20);
    Position ourGoal = getWorldMap()->getLocations()->ourGoal();
    Position fieldCenter = getWorldMap()->getLocations()->fieldCenter();

    std::cout << _switch << std::endl;

    if(_switch){
        if(fabs(player()->position().x()) <= 0.15f){
            _switch = false;
        }

        if(ballPred.y() >= 0.325f){
            desiredPosition = Position(true, 0.0f, 0.0f);
        }
        else if(ballPred.y() >= 0.0f){
            desiredPosition = Position(true, 0.0f, -0.325f);
        }
        else if(ballPred.y() >= -0.325f){
            desiredPosition = Position(true, 0.0f, 0.325f);
        }
        else{
            desiredPosition = Position(true, 0.0f, 0.0f);
        }
    }
    else if (ballPosition.x() != ourGoal.x()) {
        float alpha;
        if (getWorldMap()->getLocations()->ourSide().isRight()) {
            alpha = normAngle(static_cast<float>(M_PI) - Utils::getAngle(ourGoal, ballPred));
        } else {
            alpha = normAngle(-Utils::getAngle(ourGoal, ballPred));
        }
        float dist = getDist(alpha);

        if (_ellipseCenter.isInvalid()) {
            desiredPosition = Utils::threePoints(ourGoal, fieldCenter, dist, -alpha);
        } else {
            desiredPosition = Utils::threePoints(_ellipseCenter, fieldCenter, dist, -alpha);
        }
    } else {
        desiredPosition = player()->position();
    }

    player()->setPlayerDesiredPosition(desiredPosition);
    setBehavior(BHV_MOVETO);
}

float Role_Defender::getDist(float alpha){
    float dist = sqrt((_ellipseA * _ellipseB) / (_ellipseB * powf(cosf(alpha), 2) + _ellipseA * powf(sinf(alpha), 2)));
    return dist;
}

float Role_Defender::normAngle(float angleRadians) {
    if (angleRadians > static_cast<float>(M_PI)) {
        return angleRadians = angleRadians - 2 * static_cast<float>(M_PI);
    } else if (angleRadians < -static_cast<float>(M_PI)) {
        return angleRadians = angleRadians + 2 * static_cast<float>(M_PI);
    } else {
        return angleRadians;
    }
}

QPair<Position, Angle> Role_Defender::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam,
                                                           VSSRef::Quadrant atQuadrant) {
    // Standard position will be at our penalty mark
    Position standardPosition = Utils::threePoints(getWorldMap()->getLocations()->ourGoal(),
                                                   getWorldMap()->getLocations()->ourPenaltyMark(), 0.2f, 0.0);
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
                                                      getWorldMap()->getLocations()->theirAreaLeftBackCorner(),
                                                      0.1f, M_PI);
                standardAngle = Angle(true, 0.0);
            }
        }
        break;
        // In free ball, pos at their penalty mark (if free ball occurs at their field side)
        // Otherwise, pos at standard position (barrier)
        case VSSRef::Foul::FREE_BALL: {
            if((atQuadrant == VSSRef::Quadrant::QUADRANT_1 || atQuadrant == VSSRef::Quadrant::QUADRANT_4)
                    && getConstants()->teamSide().isLeft()) {
                standardPosition = getWorldMap()->getLocations()->theirPenaltyMark();
                standardAngle = Angle(true, 0.0);
            }
            else if(((atQuadrant == VSSRef::Quadrant::QUADRANT_2) || (atQuadrant == VSSRef::Quadrant::QUADRANT_3))
                    && !getConstants()->teamSide().isLeft()) {
                standardPosition = getWorldMap()->getLocations()->theirPenaltyMark();
                standardAngle = Angle(true, 0.0);
            }
        }
        break;
    }

    return QPair<Position, Angle>(standardPosition, standardAngle);
}
