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

#include "role_tmp_def.h"

Role_TMP_DEF::Role_TMP_DEF() {
    _avoidTeammates = false;
    _avoidOpponents = true;
    _avoidBall = true;
    _avoidOurGoalArea = true;
    _avoidTheirGoalArea = false;
    _pushBall = false;
    _accelerate = false;
    _canAvoidBall = false;
    _attackerPos.setPosition(false , 0.0f , 0.0f);
}

QString Role_TMP_DEF::name() {
    return "Role_TMP";
}

void Role_TMP_DEF::configure() {
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
    _minVelocity = 1.0f;
}
void Role_TMP_DEF::run() {
    Position ballPosition = getWorldMap()->getBall().getPosition();
    float largestMid = getLargestFreeAngle();

    if(!player()->isBehindBallXCoord(player()->position())) {
        Velocity ballVelocity = getWorldMap()->getBall().getVelocity();
        if(getWorldMap()->getLocations()->ourSide().isLeft()) {
           if(ballVelocity.vy() > 0) {
               _bhv_goToBall->setReferencePosition(Position(true, getWorldMap()->getLocations()->fieldMaxX(),
                                                            getWorldMap()->getLocations()->fieldMaxY()));
           } else {
               _bhv_goToBall->setReferencePosition(Position(true, getWorldMap()->getLocations()->fieldMaxX(),
                                                            getWorldMap()->getLocations()->fieldMinY()));
           }
        } else {
            if(ballVelocity.vy() > 0) {
                _bhv_goToBall->setReferencePosition(Position(true, getWorldMap()->getLocations()->fieldMinX(),
                                                             getWorldMap()->getLocations()->fieldMaxY()));
            } else {
                _bhv_goToBall->setReferencePosition(Position(true, getWorldMap()->getLocations()->fieldMinX(),
                                                             getWorldMap()->getLocations()->fieldMinY()));
            }
        }
        _bhv_goToBall->setAngle(2.44f);
        _bhv_goToBall->setOffsetBehindBall(0.3f);
        _bhv_goToBall->setAvoidFlags(_avoidBall, _avoidTeammates, _avoidOpponents, _avoidOurGoalArea, _avoidTheirGoalArea);
        setBehavior(BHV_GOTOBALL);
    } else {
        // Barrier
        Position desiredPosition;
        float y_Barrier;
        float x_Barrier = calcBarrier_Xcomponent();
        float modulo_vet = abs((x_Barrier - ballPosition.x())/cos(largestMid));
        y_Barrier = ballPosition.y() + (modulo_vet * sin(largestMid));
        if(_nofreeAngles){
            // std::cout << "Nao tenho angulos livres\n";
            desiredPosition.setPosition(true, x_Barrier, ballPosition.y());
        }else{
            desiredPosition.setPosition(true, x_Barrier, y_Barrier);
        }
        if(y_Barrier<-0.40f){
            y_Barrier = -0.40f;
            desiredPosition.setPosition(true, x_Barrier, y_Barrier);
        }
        else if(y_Barrier>0.40f){
            y_Barrier = 0.40f;
            desiredPosition.setPosition(true, x_Barrier, y_Barrier);
        }
        float posx_control = getConstants()->teamSide().isRight() == 1 ? 1.0f : -1.0f;
        float xAux = (0.55f*posx_control);

        if(ballPosition.x()<=xAux && xAux<0.0f){
            x_Barrier = calc_x_barrier();
            if(ballPosition.y()<0.0f){
                desiredPosition.setPosition(1, x_Barrier, -0.40f);
            }
            else if(ballPosition.y()>0.0f){
                 desiredPosition.setPosition(1, x_Barrier, 0.40f);
            }
        }
        else if(ballPosition.x()>=xAux && xAux>0.0f){
            x_Barrier = calc_x_barrier();
            if(ballPosition.y()<0.0f){
                desiredPosition.setPosition(true, x_Barrier, -0.40f);
            }
            else if(ballPosition.y()>0.0f){
                 desiredPosition.setPosition(true, x_Barrier, 0.40f);
            }
        }

        player()->setPlayerDesiredPosition(desiredPosition);
        setBehavior(BHV_MOVETO);
    }
}

float Role_TMP_DEF::getLargestFreeAngle() {
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

float Role_TMP_DEF::calcBarrier_Xcomponent(){
    if(getConstants()->teamSide().isRight()){
        return getWorldMap()->getLocations()->ourGoal().x() - _posXbarrier;
    } else {
        return getWorldMap()->getLocations()->ourGoal().x() + _posXbarrier;
    }
}

float Role_TMP_DEF::calc_x_barrier(){
    float posx_control = getConstants()->teamSide().isRight() == 1 ? 1.0f : -1.0f;
    float x_barrier = 0.68f;
    return (x_barrier*posx_control);
}

QPair<Position, Angle>Role_TMP_DEF::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    // Standard position will be at our penalty mark
    Position standardPosition = Utils::threePoints(getWorldMap()->getLocations()->ourGoal(),
                                                   getWorldMap()->getLocations()->ourPenaltyMark(), 0.2f, 0.0);
    Angle standardAngle = Angle(true, static_cast<float>(M_PI) / 2.0f);

    bool isForOurTeam = (forTeam == static_cast<VSSRef::Color>(getConstants()->teamColor()));

    switch(foul) {
        // In case of PENALTY_KICK, pos close to our field side
        case VSSRef::Foul::PENALTY_KICK: {
            if(isForOurTeam) {
                standardPosition = Utils::threePoints(getWorldMap()->getLocations()->fieldCenter(),
                                                      getWorldMap()->getLocations()->theirGoal(),
                                                      0.1f, static_cast<float>(M_PI));
                standardPosition = Position(true, standardPosition.x(),standardPosition.y() + 0.15f);
                standardAngle = Angle(true, 0.0);
            }
            else {
                standardPosition = Utils::threePoints(getWorldMap()->getLocations()->fieldCenter(),
                                                      getWorldMap()->getLocations()->ourGoal(),
                                                      0.1f, static_cast<float>(M_PI));
                standardPosition = Position(true, standardPosition.x(),standardPosition.y() - 0.15f);
                standardAngle = Angle(true, 0.0);
            }
        }
        break;
        // In case of OUR KICKOFF, it will be positioned close to attacker
        case VSSRef::Foul::KICKOFF: {
            if(isForOurTeam) {
                standardPosition = Utils::threePoints(getWorldMap()->getLocations()->fieldCenter(),
                                                      getWorldMap()->getLocations()->theirGoal(),
                                                      0.3f, static_cast<float>(M_PI));
                standardAngle = Angle(true, 0.0);
            }
        }
        break;
        case VSSRef::Foul::GOAL_KICK: {
            if(!isForOurTeam) {
                standardPosition = Utils::threePoints(getWorldMap()->getLocations()->theirPenaltyMark(),
                                                      getWorldMap()->getLocations()->theirAreaLeftBackCorner(),
                                                      0.1f, static_cast<float>(M_PI));
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
