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
#include <src/utils/freeangles/freeangles.h>

Role_Supporter::Role_Supporter(){

}

QString Role_Supporter::name() {
    return "Role_Supporter";
}

void Role_Supporter::configure() {
    // Starting behaviors
    _bhv_moveTo = new Behavior_MoveTo();
    _bhv_intercept = new Behavior_Intercept();

    // Adding behaviors to behaviors list
    addBehavior(BHV_MOVETO, _bhv_moveTo);
    addBehavior(BHV_INTERCEPT, _bhv_intercept);

    //configure vars
    _posXbarrier = 0.2f;
    _limitYup = 0.6f;
    _limitYdown = -0.6f;
}

void Role_Supporter::run() {

    _bhv_moveTo->setTargetPosition(player()->position());

    Position positionVar;
    Position positionBall = player()->getWorldMap()->getBall().getPosition();
    positionVar = Position(true, positionBall.x()-0.3f, positionBall.y());

    QList<quint8> playersId1 = player()->getWorldMap()->getAvailablePlayers(Colors::Color::BLUE);
    QList<quint8> playersId2 = player()->getWorldMap()->getAvailablePlayers(Colors::Color::YELLOW);

    QList<quint8>::const_iterator it;
    QList<Object> players;

    for(int i=0; i<playersId1.size(); i++){
       Object p = player()->getWorldMap()->getPlayer(Colors::Color::BLUE,playersId1[i]);
       players.push_back(p);
       std::cout << (int) playersId1[i]<< '\n';
    }

    for(int i=0; i<playersId2.size(); i++){
       Object p = player()->getWorldMap()->getPlayer(Colors::Color::YELLOW,playersId2[i]);
       players.push_back(p);
       std::cout << (int) playersId2[i]<< '\n';
    }

    QList<Obstacle> obstacles = FreeAngles::getObstacles(positionBall, 1.5f, players);
    QList<FreeAngles::Interval> intervalos;

    if(getConstants()->teamColor() == Colors::Color::YELLOW){
        intervalos = FreeAngles::getFreeAngles(positionBall,player()->getWorldMap()->getLocations()->ourGoalLeftPost(),player()->getWorldMap()->getLocations()->ourGoalRightPost(),obstacles,false);
    }else{
        intervalos = FreeAngles::getFreeAngles(positionBall,player()->getWorldMap()->getLocations()->ourGoalLeftPost(),player()->getWorldMap()->getLocations()->ourGoalRightPost(),obstacles,false);
        //intervalos = FreeAngles::getFreeAngles(positionBall,player()->getWorldMap()->getLocations()->ourGoalRightPost(),player()->getWorldMap()->getLocations()->ourGoalLeftPost(),obstacles,false);
    }

    float largestAngle=0;
    float largestMid=0;

    // Ordering the free angles
    if(intervalos.size()==0) {
        _nofreeAngles = true;
        largestMid = 0.0f;
    }else {
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
            }
        }
    }

    if(isBall_ourfield()){
        // Barrier
        Position desiredPosition;
        float y_Barrier;
        float x_Barrier = calcBarrier_Xcomponent();
        float modulo_vet = abs((x_Barrier - positionBall.x())/cos(largestMid));
        y_Barrier = positionBall.y() + (modulo_vet * sin(largestMid));
        if(_nofreeAngles){
            std::cout << "estou aqui\n";
            desiredPosition.setPosition(1,x_Barrier,positionBall.y());
        }else{
            std::cout << "nao estou aqui\n";
            desiredPosition.setPosition(1,x_Barrier,y_Barrier);
        }
        _bhv_moveTo->setTargetPosition(desiredPosition);
        setBehavior(BHV_MOVETO);
    }else{
        // Advanced Suport
        Position desiredPosition;
        float posx_advanced = calc_x_advanced();
        float moduloVet = abs((posx_advanced - positionBall.x())/cos(largestMid));
        //float
        float posy_advanced;
        if(_nofreeAngles){
            posy_advanced = positionBall.y();
            desiredPosition.setPosition(1,posx_advanced,posy_advanced);
        }else{
            posy_advanced = positionBall.y() + (moduloVet * sin(largestMid));
            desiredPosition.setPosition(1,posx_advanced,posy_advanced);
        }
        _bhv_moveTo->setTargetPosition(desiredPosition);
        setBehavior(BHV_MOVETO);
    }
    setBehavior(BHV_MOVETO);
}

bool Role_Supporter::isBall_ourfield(){
    Position position_ball = player()->getWorldMap()->getBall().getPosition();
    if(getConstants()->teamSide().isRight()){
        return (position_ball.x() > 0.0f);
    }else{
        return (position_ball.x() < 0.0f);
    }
}

float Role_Supporter::calcBarrier_Xcomponent(){
    if(getConstants()->teamSide().isRight()){
        return player()->getWorldMap()->getLocations()->ourGoal().x() - _posXbarrier;
    }else{
        return player()->getWorldMap()->getLocations()->ourGoal().x() + _posXbarrier;
    }
}

float Role_Supporter::limit_Ypos(float * posy){
    if(*posy > _limitYup){
        *posy = _limitYup;
    }
    if(*posy < _limitYdown){
        *posy = _limitYdown;
    }
}

float Role_Supporter::calc_x_advanced(){
    float posx_control = getConstants()->teamSide().isRight() == 1 ? 1.0f : -1.0f;
    float distance_advanced = 0.35f;
    Position position_ball = player()->getWorldMap()->getBall().getPosition();
    return position_ball.x() + (distance_advanced*posx_control);
}
