/***
 * Maracatronics Robotics
 * Federal University of Pernambuco (UFPE) at Recife
 * http://www.maracatronics.com/
 *
 * This file is part of Armorial project.
 *
 * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
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

#include "role_supporter.h"

#define SUPFACTOR 0.2f

Role_Supporter::Role_Supporter(){
    _avoidTeammates = false;
    _avoidOpponents = true;
    _avoidBall = true;
    _avoidOurGoalArea = true;
    _avoidTheirGoalArea = false;
    _canAvoidBall = false;
}

QString Role_Supporter::name() {
    return "Role_Supporter";
}

void Role_Supporter::configure() {
    // Starting behaviors
    _bhv_moveTo = new Behavior_MoveTo();
    _bhv_intercept = new Behavior_Intercept();
    _bhv_barrier = new Behavior_Barrier();
    _bhv_goToBall = new Behavior_GoToBall();

    // Adding behaviors to behaviors list
    addBehavior(BHV_MOVETO, _bhv_moveTo);
    addBehavior(BHV_INTERCEPT, _bhv_intercept);
    addBehavior(BHV_BARRIER, _bhv_barrier);    
    addBehavior(BHV_GOTOBALL, _bhv_goToBall);

    //configure vars
    _posXbarrier = 0.2f;
    _limitYup = 0.6f;
    _limitYdown = -0.6f;
}

void Role_Supporter::run() {
    if(player()->isBehindBallXCoord(player()->position())) {
        _avoidBall = true;
    } else {
        _avoidBall = false;
    }
    if(hasAllyInTheirArea()) {
        _avoidTheirGoalArea = true;
    } else {
        _avoidTheirGoalArea = false;
    }

    Position ballPosition = getWorldMap()->getBall().getPosition();
    float largestMid = getLargestFreeAngle();

    // Advanced Support
    float posx_advanced = calc_x_advanced();
    float moduloVet = abs((posx_advanced - ballPosition.x())/cos(largestMid));
    float posy_advanced;
    if(_nofreeAngles){
        posy_advanced = ballPosition.y();
    } else {
        posy_advanced = ballPosition.y() + (moduloVet * sin(largestMid));
    }
    Position desiredPosition(true, posx_advanced, posy_advanced);
    player()->setPlayerDesiredPosition(player()->limitPosInsideField(desiredPosition));

    _bhv_moveTo->enableRotation(false);
    _bhv_moveTo->setBaseSpeed(33);
    _bhv_moveTo->setAvoidFlags(_avoidBall, _avoidTeammates, _avoidOpponents, _avoidOurGoalArea, _avoidTheirGoalArea);
    player()->setPlayerDesiredPosition(desiredPosition);
    setBehavior(BHV_MOVETO);
}

float Role_Supporter::getLargestFreeAngle() {
    Position ballPosition = getWorldMap()->getBall().getPosition();
    QList<quint8> playersId1 = getWorldMap()->getAvailablePlayers(Colors::Color::BLUE);
    QList<quint8> playersId2 = getWorldMap()->getAvailablePlayers(Colors::Color::YELLOW);

    QList<quint8>::const_iterator it;
    QList<Object> players;

    // Associating players as obstacles
    for(int i=0; i<playersId1.size(); i++){
        Object p = getWorldMap()->getPlayer(Colors::Color::BLUE,playersId1[i]);
        if (!(player()->position().x() == p.getPosition().x() && player()->position().y() == p.getPosition().y())){
            players.push_back(p);
        }
    }

    for(int i=0; i<playersId2.size(); i++){
        Object p = getWorldMap()->getPlayer(Colors::Color::YELLOW,playersId2[i]);
        if (!(player()->position().x() == p.getPosition().x() && player()->position().y() == p.getPosition().y())){
            players.push_back(p);
        }
    }

    QList<Obstacle> obstacles = FreeAngles::getObstacles(ballPosition, 30.0f, players);
    QList<FreeAngles::Interval> intervalos;

    if(getConstants()->teamColor() == Colors::Color::YELLOW){
        Position leftPost = getWorldMap()->getLocations()->ourGoalLeftPost();
        Position rightPost = getWorldMap()->getLocations()->ourGoalRightPost();
        intervalos = FreeAngles::getFreeAngles(ballPosition,leftPost,rightPost,obstacles,false);
    } else {
        Position leftPost = getWorldMap()->getLocations()->ourGoalLeftPost();
        Position rightPost = getWorldMap()->getLocations()->ourGoalRightPost();
        intervalos = FreeAngles::getFreeAngles(ballPosition,leftPost,rightPost,obstacles,false);
    }

    float largestAngle=0;
    float largestMid=0;

    // Ordering the free angles
    if(intervalos.size()==0) {
        _nofreeAngles = true;
        largestMid = 0.0f;
    } else {
        _nofreeAngles = false;
        for(int i=0; i<intervalos.size(); i++) {
            float angI = intervalos.at(i).angInitial();
            float angF = intervalos.at(i).angFinal();
            Utils::angleLimitZeroTwoPi(&angI);
            Utils::angleLimitZeroTwoPi(&angF);
            float dif = angF - angI;
            Utils::angleLimitZeroTwoPi(&dif);

            if(dif>largestAngle) {
                largestAngle = dif;
                largestMid = angF - dif/2;
                Utils::angleLimitZeroTwoPi(&largestMid);
            }
        }
    }
    return largestMid;
}

bool Role_Supporter::hasAllyInTheirArea() {
    Colors::Color ourColor = getConstants()->teamColor();
    QList<quint8> ourPlayers = getWorldMap()->getAvailablePlayers(ourColor);
    for(int i=0; i<ourPlayers.size(); i++) {
        Position posPlayer = getWorldMap()->getPlayer(ourColor, ourPlayers[i]).getPosition();
        if((ourPlayers[i] != player()->playerId()) && getWorldMap()->getLocations()->isInsideTheirArea(posPlayer)) {
            return true;
        }
    }
    return false;
}

// Not used
float Role_Supporter::calcBarrier_Xcomponent(){
    if(getConstants()->teamSide().isRight()){
        return getWorldMap()->getLocations()->ourGoal().x() - _posXbarrier;
    } else {
        return getWorldMap()->getLocations()->ourGoal().x() + _posXbarrier;
    }
}

float Role_Supporter::calc_x_advanced(){
    float posx_control = getConstants()->teamSide().isRight() == 1 ? 1.0f : -1.0f;
    float distance_advanced = 0.4f;
    Position position_ball = getWorldMap()->getBall().getPosition();
    return position_ball.x() + (distance_advanced*posx_control);
}

// Not used
float Role_Supporter::calc_x_barrier(){
    float posx_control = getConstants()->teamSide().isRight() == 1 ? 1.0f : -1.0f;
    float x_barrier = 0.68f;
    return (x_barrier*posx_control);
}

QPair<Position, Angle> Role_Supporter::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam,
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
            else if((atQuadrant == VSSRef::Quadrant::QUADRANT_2 || atQuadrant == VSSRef::Quadrant::QUADRANT_3)
                    && !getConstants()->teamSide().isLeft()) {

                standardPosition = getWorldMap()->getLocations()->theirPenaltyMark();
                standardAngle = Angle(true, 0.0);
            }
        }
        break;
    }

    return QPair<Position, Angle>(standardPosition, standardAngle);
}
