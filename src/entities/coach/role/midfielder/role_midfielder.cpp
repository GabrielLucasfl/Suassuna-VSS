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

#include "role_midfielder.h"

#define MIDFACTOR 0.2f
Role_Midfielder::Role_Midfielder() {
    _attackerPos.setPosition(false , 0.0f , 0.0f);
    _avoidBall = false;
    _avoidTeammates = false;
    _avoidOpponents = false;
    _avoidOurGoalArea = false;
    _avoidTheirGoalArea = false;
}

QString Role_Midfielder::name() {
    return "Role_Midfielder";
}

void Role_Midfielder::configure() {
    // Starting behaviors
    _bhv_moveTo = new Behavior_MoveTo();
    _bhv_intercept = new Behavior_Intercept();
    _bhv_barrier = new Behavior_Barrier();

    // Adding behaviors to behaviors list
    addBehavior(BHV_MOVETO, _bhv_moveTo);
    addBehavior(BHV_INTERCEPT, _bhv_intercept);
    addBehavior(BHV_BARRIER, _bhv_barrier);
}

void Role_Midfielder::run() {
    // Ball projection
    Position ballPos = getWorldMap()->getBall().getPosition();
    Velocity ballVel = getWorldMap()->getBall().getVelocity();
    Position ballDirection, ballProj;
    Position desiredPosition;
    if(ballVel.abs() > 0.03f) {
        ballDirection = Position(true, ballVel.vx()/ballVel.abs(), ballVel.vy()/ballVel.abs());
    } else {
        ballDirection = Position(true, 0, 0);
    }
    float factor = std::min(MIDFACTOR * ballVel.abs(), 0.5f);
    ballProj = Position(true, ballPos.x() + factor*ballDirection.x(), ballPos.y() + factor*ballDirection.y());
    //limit desired area
    if(getWorldMap()->getLocations()->ourSide().isLeft()){
        if(ballProj.x() > getWorldMap()->getLocations()->theirPenaltyMark().x()){
            desiredPosition.setPosition(true, getWorldMap()->getLocations()->theirPenaltyMark().x() , ballProj.y());
        }
        else if(ballProj.x() < getWorldMap()->getLocations()->ourPenaltyMark().x()){
            desiredPosition.setPosition(true, getWorldMap()->getLocations()->ourPenaltyMark().x() , ballProj.y());
        }
        else{
            desiredPosition.setPosition(true, ballProj.x() , ballProj.y());
        }
    }
    else{
        if(ballProj.x() < getWorldMap()->getLocations()->theirPenaltyMark().x()){
            desiredPosition.setPosition(true, getWorldMap()->getLocations()->theirPenaltyMark().x() , ballProj.y());
        }
        else if(ballProj.x() > getWorldMap()->getLocations()->ourPenaltyMark().x()){
            desiredPosition.setPosition(true, getWorldMap()->getLocations()->ourPenaltyMark().x() , ballProj.y());
        }
        else{
            desiredPosition.setPosition(true, ballProj.x() , ballProj.y());
        }
    }


    // ideias:
    /* -se posicionar no meio de campo meio pra tras: feito
     * avançar para a bola em caso de rebote: feito
     * troca de role: ta quase
     */
    if(player()->isBehindBallXCoord(desiredPosition)){
        _avoidBall = false;
    }
    else{
        _avoidBall = true;
    }
    _avoidTeammates = false;
    _avoidOpponents = false;
    _avoidOurGoalArea = true;
    _avoidTheirGoalArea = false;
    _bhv_moveTo->setAvoidFlags(_avoidBall, _avoidTeammates, _avoidOpponents, _avoidOurGoalArea, _avoidTheirGoalArea);
    _bhv_moveTo->setBaseSpeed(40);
    player()->setPlayerDesiredPosition(desiredPosition);
    setBehavior(BHV_MOVETO);


}




//meanwhile same as defender
QPair<Position, Angle> Role_Midfielder::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam,
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
