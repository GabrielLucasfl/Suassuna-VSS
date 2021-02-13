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

#include "player.h"

#include <src/entities/coach/role/role.h>
#include <src/entities/coach/player/navigation/navigation.h>

Player::Player(quint8 playerId, Constants *constants, Referee *referee ,WorldMap *worldMap, NavigationAlgorithm* navAlg) : Entity(ENT_PLAYER) {
    _playerId = playerId;
    _constants = constants;
    _worldMap = worldMap;
    _referee = referee;
    _playerRole = nullptr;
    _nav = new Navigation(this, navAlg, constants);
}

WorldMap* Player::getWorldMap() {
    if(_worldMap == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("WorldMap with nullptr value at Player") + '\n';
    }
    else {
        return _worldMap;
    }

    return nullptr;
}

quint8 Player::playerId() {
    return _playerId;
}

Position Player::position() {
    Position playerPos = getWorldMap()->getPlayer(getConstants()->teamColor(), playerId()).getPosition();
    return playerPos;
}

Angle Player::orientation() {
    Angle playerAngle = getWorldMap()->getPlayer(getConstants()->teamColor(), playerId()).getOrientation();
    return playerAngle;
}

bool Player::isLookingTo(Position &pos, float error){
    // Taking the reference angle
    float referenceAngle = atan2(pos.y() - position().y(), pos.x() - position().x());

    // Analysing condition
    float angleDifference = abs(orientation().value() - referenceAngle);
    if (angleDifference > abs(M_PI - angleDifference)) {
        angleDifference = abs(M_PI - angleDifference);
    }
    if (angleDifference < error) {
        return true;
    } else {
        return false;
    }
}

void Player::setRole(Role *role) {
    _mutexRole.lock();

    // Set old role player to nullptr
    if(_playerRole != nullptr) _playerRole->setPlayer(nullptr);

    // Set new role
    _playerRole = role;

    _mutexRole.unlock();
}

float Player::getPlayerDistanceTo(Position &targetPosition) {
    return sqrt(pow(position().x() - targetPosition.x(), 2) + pow(position().y() - targetPosition.y(), 2));
}

float Player::getLinearError() {
    return 0.05f; // 5cm
}

float Player::getAngularError() {
    return 0.02f; // ~= 1.15 deg
}

std::pair<Angle,float> Player::getNavDirectionDistance(const Position &destination, const Angle &positionToLook, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea, bool avoidTheirGoalArea) {
    _nav->setGoal(destination, positionToLook, avoidTeammates, avoidOpponents, avoidBall, avoidOurGoalArea, avoidTheirGoalArea);
    Angle direction = _nav->getDirection();
    float distance = _nav->getDistance();

    std::pair<Angle,float> movement = std::make_pair(direction, distance);
    movement.first.setValue(movement.first.value() - orientation().value());
    return movement;
}

float Player::getRotateAngle(Position targetPosition) {
    float rotateAngle = Utils::getAngle(position(), targetPosition) - orientation().value();

    if(rotateAngle > float(M_PI)) rotateAngle -= 2.0f * float(M_PI);
    if(rotateAngle < float(-M_PI)) rotateAngle += 2.0f * float(M_PI);

    if(rotateAngle > float(M_PI_2)) rotateAngle -= float(M_PI);
    if(rotateAngle < float(-M_PI_2)) rotateAngle += float(M_PI);

    return rotateAngle;
}

float Player::getVxToTarget(Position targetPosition){
    // Salvando posicao do robo pra os calculos
    float robot_x = position().x(), robot_y = position().y();

    // Define a velocidade do robô para chegar na bola
    float dx = (targetPosition.x() - robot_x);
    float dy = (targetPosition.y() - robot_y);

    // Pegando modulo do vetor distancia
    float distanceMod = sqrtf(powf(dx, 2.0) + powf(dy, 2.0));

    return distanceMod;
}

float Player::getRotateSpeed(float angleRobotToTarget){
    float ori = orientation().value();
    if(ori > float(M_PI)) ori -= 2.0f * float(M_PI);
    if(ori < float(-M_PI)) ori += 2.0f * float(M_PI);

    bool swapSpeed = false;
    if(angleRobotToTarget > float(M_PI) / 2.0f){
        angleRobotToTarget -= float(M_PI);
        swapSpeed = true;
    }
    else if(angleRobotToTarget < float(-M_PI) / 2.0f){
        angleRobotToTarget += float(M_PI);
        swapSpeed = true;
    }

    if(swapSpeed){
        if(ori > float(M_PI) / 2.0f){
            ori -= float(M_PI);
        }
        else if(ori < float(-M_PI) / 2.0f){
            ori += float(M_PI);
        }
    }
    float angleToTarget = angleRobotToTarget + ori;
    //float speed = _vwPID->calculate(angleToTarget, ori);
    // Without PID: rotateSpeed is the angular difference between targetAngle and actual orientarion
    float speed = angleToTarget - ori;

    return speed;
}

void Player::goTo(Position &targetPosition, float minVel, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea , bool avoidTheirGoalArea){
    // Taking orientation from path planning
    Angle anglePP;
    float help = getRotateAngle(targetPosition);

    if(targetPosition.isInvalid()) {
        anglePP = Angle(false, 0.0);
    }else {
        anglePP = Angle(true, help);
    }
    std::pair<Angle, float> a = getNavDirectionDistance(targetPosition, anglePP, avoidTeammates, avoidOpponents, avoidBall, avoidOurGoalArea, avoidTheirGoalArea);

    // Verificando se o lado de trás pra movimentação é a melhor escolha
    help = a.first.value();
    if(help > float(M_PI)) help -= 2.0f * float(M_PI);
    if(help < float(-M_PI)) help += 2.0f * float(M_PI);

    bool swapSpeed = false;
    if(help > float(M_PI) / 2.0f){
        help -= float(M_PI);
        swapSpeed = true;
    }
    else if(help < float(-M_PI) / 2.0f){
        help += float(M_PI);
        swapSpeed = true;
    }

    float rotateSpeed = getRotateSpeed(help);
    float vel;

    //if (pathActivated) vel = a.second;
    //else vel = getVxToTarget(targetPosition);
    vel = getVxToTarget(targetPosition);

    // Adjusting to minVel if lower
    if(vel <= minVel){
        // Adjusting absolute value of velocity
        vel = minVel;
    }
    // Se escolheu o lado de trás, inverte o vx
    if(swapSpeed) vel *= -1;

    float dist = Utils::distance(position(), targetPosition);
    float Fr_factor = (2.75f/2.0f);
    //std::cout << vel << std::endl;
    //Fr_factor = ((2.5f/2.0f)*Vel_teste);
    float k = 1;
    if(swapSpeed)k=-1;
    // To avoid circular trajectories
    rotateSpeed *= 10;
    //Vel_teste = 1.0f;
    if ( abs(help) > Angle::toRadians(80)) {
        emit setLinearSpeed(getConstants()->teamColor(), playerId(), 0.0f);
        emit setAngularSpeed(getConstants()->teamColor(), playerId(), rotateSpeed);
    } else {
        if(dist <= 0.1f){ // se estiver a 10cm ou menos do alvo
            if(abs(help) >= Angle::toRadians(15)){ // se a diferença for maior que 15 deg
                // zera a linear e espera girar
                emit setLinearSpeed(getConstants()->teamColor(), playerId(), 0.0f*k);
                emit setAngularSpeed(getConstants()->teamColor(), playerId(), rotateSpeed);
            }else{
                // caso esteja de boa, gogo
                emit setLinearSpeed(getConstants()->teamColor(), playerId(), vel);
                emit setAngularSpeed(getConstants()->teamColor(), playerId(), rotateSpeed);
            }
        }
        else if(dist > 0.1f && dist <= 0.5f){ // se estiver entre 10cm a 50cm do alvo
            if(abs(help) >= Angle::toRadians(25)){ // se a diferença for maior que 25 deg
                // linear * 0.3 e gira
                emit setLinearSpeed(getConstants()->teamColor(), playerId(), vel);
                emit setAngularSpeed(getConstants()->teamColor(), playerId(), Fr_factor*rotateSpeed);
            }else{
                // caso esteja de boa, gogo
                emit setLinearSpeed(getConstants()->teamColor(), playerId(), vel);
                emit setAngularSpeed(getConstants()->teamColor(), playerId(), rotateSpeed);
            }
        }
        else if(dist > 0.5f){ // se estiver a mais de 50cm do alvo
            if(abs(help) >= Angle::toRadians(35)){ // se a diferença for maior que 35 deg
                // linear * 0.5 e gira
                emit setLinearSpeed(getConstants()->teamColor(), playerId(), vel);
                emit setAngularSpeed(getConstants()->teamColor(), playerId(), Fr_factor*rotateSpeed);
            }else{
                // caso esteja de boa, gogo
                emit setLinearSpeed(getConstants()->teamColor(), playerId(), vel);
                emit setAngularSpeed(getConstants()->teamColor(), playerId(), rotateSpeed);
            }
        }
    }
}

void Player::rotateTo(Position &targetPosition) {
    float angleRobotToTarget = getRotateAngle(targetPosition);
    float speed = getRotateSpeed(angleRobotToTarget);

    emit setAngularSpeed(getConstants()->teamColor(), playerId(), speed);
}

void Player::spin(bool isClockWise) {
    if (isClockWise) {
        emit setAngularSpeed(playerId(), -50);
    } else {
        emit setAngularSpeed(playerId(), 50);
    }
}

void Player::move(float linearSpeed, float angularSpeed) {
    emit setLinearSpeed(playerId(), linearSpeed);
    emit setAngularSpeed(playerId(), angularSpeed);
}

void Player::idle() {
    emit setLinearSpeed(playerId(), 0.0);
    emit setAngularSpeed(playerId(), 0.0);
}

void Player::initialization() {
    QString teamColorName = getConstants()->teamColorName().toUpper();
    std::cout << Text::cyan("[PLAYER " + teamColorName.toStdString() + ":" + std::to_string(playerId()) + "] ", true) + Text::bold("Thread started.") + '\n';
}

void Player::loop() {
    if(position().isInvalid()) {
        setRole(nullptr);
        idle();
    }
    else {
        _mutexRole.lock();
        if(_playerRole != nullptr) {
            if(!_playerRole->isInitialized()) {
                _playerRole->initialize(getConstants(), getReferee(), getWorldMap());
            }
            _playerRole->setPlayer(this);
            _playerRole->runRole();
        }
        _mutexRole.unlock();
    }
}

void Player::finalization() {
    QString teamColorName = getConstants()->teamColorName().toUpper();
    std::cout << Text::cyan("[PLAYER " + teamColorName.toStdString() + ":" + std::to_string(playerId()) + "] ", true) + Text::bold("Thread ended.") + '\n';
}

Constants* Player::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Constants with nullptr value at Player") + '\n';
    }
    else {
        return _constants;
    }

    return nullptr;
}

Referee* Player::getReferee() {
    if(_referee == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Referee with nullptr value at Player") + '\n';
    }
    else {
        return _referee;
    }

    return nullptr;
}

void Player::receiveFoul(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    _mutexRole.lock();

    if(_playerRole != nullptr) {
        QPair<Position, Angle> placementPosition = _playerRole->getPlacementPosition(foul, forTeam, atQuadrant);
        emit sendPlacement(this->playerId(), placementPosition.first, placementPosition.second);
    }

    _mutexRole.unlock();
}

void Player::setGoal(Position pos) {
    _nav->setGoal(pos, orientation(), true, true, true, true, true);
}

QLinkedList<Position> Player::getPath() const {
    return _nav->getPath();
}
