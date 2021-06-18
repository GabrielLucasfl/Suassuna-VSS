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
    Position desiredPosition;
    Position ballPosition = getWorldMap()->getBall().getPosition();
    Velocity ballVelocity = getWorldMap()->getBall().getVelocity();
    Position ballRelocated(true, ballPosition.x() + ballVelocity.vx(), ballPosition.y() + ballVelocity.vy());
    
    // Gnerating ball movement line
    float angularCoeff = (ballRelocated.y() - ballPosition.y()) / (ballRelocated.x() - ballPosition.x());
    float linearCoeff = ballPosition.y() - ballPosition.x() * angularCoeff;
    // --> y = angularCoefficient * x + linearCoefficient <--
    // Sacou?
    
    int signal;

    if (getConstants()->teamSide().isRight()) {
        signal = -1;
    } else {
        signal = 1;
    }

    //float x = 0.75f + signal * sqrt(0.09f * (1 - powf(angularCoefficient * x + linearCoefficient, 2) / 0.45f));
    float x = (sqrt(180 - 189 * powf(angularCoeff, 2) - 600 * angularCoeff * linearCoeff - 400 * powf(linearCoeff, 2)) * sqrt(5)
            - 20 * angularCoeff * linearCoeff + 75) / (20 * (powf(angularCoeff, 2) + 5));
    float y = angularCoeff * x + linearCoeff;
    desiredPosition.setPosition(true, x, y);
    player()->setPlayerDesiredPosition(desiredPosition);
    setBehavior(BHV_MOVETO);
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
