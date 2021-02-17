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

Player::Player(quint8 playerId, Constants* constants, Referee* referee ,WorldMap* worldMap, NavigationAlgorithm* navAlg) : Entity(ENT_PLAYER) {
    _playerId = playerId;
    _constants = constants;
    _worldMap = worldMap;
    _referee = referee;
    _playerRole = nullptr;
    _nav = new Navigation(this, navAlg, constants, worldMap);
}

QString Player::name() {
    return QString("PLAYER %1").arg(getConstants()->teamColorName());
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

void Player::setRole(Role* role) {
    _mutexRole.lock();

    // Set old role player to nullptr
    if(_playerRole != nullptr) _playerRole->setPlayer(nullptr);

    // Set new role
    _playerRole = role;

    _mutexRole.unlock();
}

float Player::getPlayerRotateAngleTo(Position &targetPosition) {
    float rotateAngle = Utils::getAngle(position(), targetPosition) - orientation().value();

    if(rotateAngle > float(M_PI)) rotateAngle -= 2.0f * float(M_PI);
    if(rotateAngle < float(-M_PI)) rotateAngle += 2.0f * float(M_PI);

    if(rotateAngle > float(M_PI_2)) rotateAngle -= float(M_PI);
    if(rotateAngle < float(-M_PI_2)) rotateAngle += float(M_PI);

    return rotateAngle;
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

void Player::goTo(Position &targetPosition, float minVel, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea , bool avoidTheirGoalArea) {
    // Get direction and distance to next point in path from navigation algorithm
    std::pair<Angle,float> movement = getNavDirectionDistance(targetPosition, orientation(), avoidTeammates, avoidOpponents, avoidBall, avoidOurGoalArea, avoidTheirGoalArea);

    // Update distance and orientation variables
    float distToPoint = movement.second;
    float angPlayerToPoint = movement.first.value();

    if (distToPoint < minVel) {
        distToPoint = minVel;
    }

    // Check if angle is in range [-pi, pi]
    if(angPlayerToPoint > float(M_PI)) angPlayerToPoint -= 2.0f * float(M_PI);
    if(angPlayerToPoint < float(-M_PI)) angPlayerToPoint += 2.0f * float(M_PI);

    // Check best robot front to use
    bool swapSpeed = false;
    if(angPlayerToPoint > float(M_PI) / 2.0f) {
        angPlayerToPoint -= float(M_PI);
        swapSpeed = true;
    } else if (angPlayerToPoint < float(-M_PI) / 2.0f) {
        angPlayerToPoint += float(M_PI);
        swapSpeed = true;
    }
    if(swapSpeed) {
        distToPoint *= -1;
    }

    emit setLinearSpeed(playerId(), distToPoint);
    // angularSpeed*(constant>1) to ensure our angular speed is enough to reach the desired orientation while our player is moving
    emit setAngularSpeed(playerId(), angPlayerToPoint*7);
}

void Player::rotateTo(Position &targetPosition) {
    float angleRobotToTarget = getPlayerRotateAngleTo(targetPosition);

    // Adjustments
    if(angleRobotToTarget > float(M_PI) / 2.0f){
        angleRobotToTarget -= float(M_PI);
    }
    else if(angleRobotToTarget < float(-M_PI) / 2.0f){
        angleRobotToTarget += float(M_PI);
    }

    emit setAngularSpeed(playerId(), angleRobotToTarget);
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
