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
    _timer.start();
    _minVelocity = 1.0f;
    //_bhv_moveTo->setAvoidFlags(0,0,0,1,0);
    _bhv_moveTo->setAvoidFlags(0,1,1,1,0);
}

void Role_Supporter::run() {
    //_bhv_moveTo->setTargetPosition(player()->position());
    Position positionVar;
    Position positionBall = getWorldMap()->getBall().getPosition();
    positionVar = Position(true, positionBall.x()-0.3f, positionBall.y());

    QList<quint8> playersId1 = getWorldMap()->getAvailablePlayers(Colors::Color::BLUE);
    QList<quint8> playersId2 = getWorldMap()->getAvailablePlayers(Colors::Color::YELLOW);

    QList<quint8>::const_iterator it;
    QList<Object> players;

    for(int i=0; i<playersId1.size(); i++){
        Object p = getWorldMap()->getPlayer(Colors::Color::BLUE,playersId1[i]);
        if( !(player()->position().x() == p.getPosition().x() && player()->position().y() == p.getPosition().y()) ){
            players.push_back(p);
        }

        //std::cout << static_cast<int>(playersId1[i]) << '\n';
    }

    for(int i=0; i<playersId2.size(); i++){
        Object p = getWorldMap()->getPlayer(Colors::Color::YELLOW,playersId2[i]);
        if( !(player()->position().x() == p.getPosition().x() && player()->position().y() == p.getPosition().y()) ){
            players.push_back(p);
        }
        //std::cout << static_cast<int>(playersId2[i]) << '\n';
    }

    QList<Obstacle> obstacles = FreeAngles::getObstacles(positionBall, 30.0f, players);
    QList<FreeAngles::Interval> intervalos;

    if(getConstants()->teamColor() == Colors::Color::YELLOW){
        Position leftPost = getWorldMap()->getLocations()->ourGoalLeftPost();
        Position rightPost = getWorldMap()->getLocations()->ourGoalRightPost();
        intervalos = FreeAngles::getFreeAngles(positionBall,leftPost,rightPost,obstacles,false);
    }else{
        Position leftPost = getWorldMap()->getLocations()->ourGoalLeftPost();
        Position rightPost = getWorldMap()->getLocations()->ourGoalRightPost();
        intervalos = FreeAngles::getFreeAngles(positionBall,leftPost,rightPost,obstacles,false);
        //intervalos = FreeAngles::getFreeAngles(positionBall,player()->getWorldMap()->getLocations()->ourGoalRightPost(),player()->getWorldMap()->getLocations()->ourGoalLeftPost(),obstacles,false);
    }

    float largestAngle=0;
    float largestMid=0;
    float dbAngF, dbAngI;

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
                Utils::angleLimitZeroTwoPi(&largestMid);
                dbAngF = angF; dbAngI = angI;
            }
        }
    }

    _timer.stop();
    double tempo = _timer.getSeconds();
    if(tempo >= 1.0){
        //printf("O intervalo escolhido foi: %f \t %f\n e a maior abertura eh %f\n",dbAngI, dbAngF, largestMid);
        //printf("Tam lista %d \n", obstacles.size());
        _timer.start();
    }
    Position desiredPosition = player()->position();
    if(isBall_ourfield()){
        // Barrier
        float y_Barrier;
        float x_Barrier = calcBarrier_Xcomponent();
        float modulo_vet = abs((x_Barrier - positionBall.x())/cos(largestMid));
        y_Barrier = positionBall.y() + (modulo_vet * sin(largestMid));
        if(_nofreeAngles){
            // std::cout << "Nao tenho angulos livres\n";
            desiredPosition.setPosition(1,x_Barrier,positionBall.y());
        }else{
            desiredPosition.setPosition(1,x_Barrier,y_Barrier);
        }
        if(y_Barrier<-0.40f){
            y_Barrier = -0.40f;
            desiredPosition.setPosition(1,x_Barrier,y_Barrier);
        }
        else if(y_Barrier>0.40f){
            y_Barrier = 0.40f;
            desiredPosition.setPosition(1,x_Barrier,y_Barrier);
        }
        float posx_control = getConstants()->teamSide().isRight() == 1 ? 1.0f : -1.0f;
        float xAux = (0.55f*posx_control);

        if(positionBall.x()<=xAux && xAux<0.0f){
            x_Barrier = calc_x_barrier();
            if(positionBall.y()<0.0f){
                desiredPosition.setPosition(1, x_Barrier, -0.40f);
            }
            else if(positionBall.y()>0.0f){
                 desiredPosition.setPosition(1, x_Barrier, 0.40f);
            }
        }
        else if(positionBall.x()>=xAux && xAux>0.0f){
            x_Barrier = calc_x_barrier();
            if(positionBall.y()<0.0f){
                desiredPosition.setPosition(1, x_Barrier, -0.40f);
            }
            else if(positionBall.y()>0.0f){
                 desiredPosition.setPosition(1, x_Barrier, 0.40f);
            }
        }
        //_bhv_moveTo->setMinimalVelocity(_minVelocity);
        _bhv_moveTo->setTargetPosition(desiredPosition);
        setBehavior(BHV_MOVETO);
    }else{
        // Advanced Support
        float posx_advanced = calc_x_advanced();
        float moduloVet = abs((posx_advanced - positionBall.x())/cos(largestMid));
        float posy_advanced;
        if(_nofreeAngles){
            posy_advanced = positionBall.y();
            desiredPosition.setPosition(1,posx_advanced,posy_advanced);
        }else{
            posy_advanced = positionBall.y() + (moduloVet * sin(largestMid));
            desiredPosition.setPosition(1,posx_advanced,posy_advanced);
        }
        _bhv_moveTo->setTargetPosition(desiredPosition);
        //_bhv_moveTo->setMinimalVelocity(_minVelocity);
        setBehavior(BHV_MOVETO);
    }
    setBehavior(BHV_MOVETO);
    player()->rotateTo(positionBall);
}

bool Role_Supporter::isBall_ourfield(){
    Position position_ball = getWorldMap()->getBall().getPosition();
    if(getConstants()->teamSide().isRight()){
        return (position_ball.x() > 0.0f);
    }else{
        return (position_ball.x() < 0.0f);
    }
}

float Role_Supporter::calcBarrier_Xcomponent(){
    if(getConstants()->teamSide().isRight()){
        return getWorldMap()->getLocations()->ourGoal().x() - _posXbarrier;
    }else{
        return getWorldMap()->getLocations()->ourGoal().x() + _posXbarrier;
    }
}

void Role_Supporter::limit_Ypos(float * posy){
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
    Position position_ball = getWorldMap()->getBall().getPosition();
    return position_ball.x() + (distance_advanced*posx_control);
}

float Role_Supporter::calc_x_barrier(){
    float posx_control = getConstants()->teamSide().isRight() == 1 ? 1.0f : -1.0f;
    float x_barrier = 0.68f;
    return (x_barrier*posx_control);
}

QPair<Position, Angle> Role_Supporter::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    Position standardPosition;
    Angle standardAngle = Angle(true, 0);
    if (getWorldMap()->getLocations()->ourSide().isRight()) {
        standardPosition = Position(true, 0.375f, 0.0f);
    } else {
        standardPosition = Position(true, -0.375f, 0.0f);
    }
    return QPair<Position,Angle>(standardPosition, standardAngle);
}