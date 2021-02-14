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

float Player::getPlayerRotateAngleTo(Position &targetPosition) {
    float componentX = (targetPosition.x() - position().x());
    float componentY = (targetPosition.y() - position().y());
    float distToTarget = sqrt(pow(componentX, 2) + pow(componentY, 2));

    componentX = componentX / distToTarget;

    // Check possible divisions for 0
    if(isnanf(componentX)) {
        return 0.0f;
    }

    float angleOriginToTarget; // Angle from field origin to targetPosition
    float angleRobotToTarget;  // Angle from robot to targetPosition

    if(componentY < 0.0f) {
        angleOriginToTarget = 2*M_PI - acos(componentX); // Angle that the target make with x-axis to robot
    } else {
        angleOriginToTarget = acos(componentX); // Angle that the target make with x-axis to robot
    }

    angleRobotToTarget = angleOriginToTarget - orientation().value();

    // Adjusting to rotate the minimum possible
    if(angleRobotToTarget > M_PI) angleRobotToTarget -= 2.0 * M_PI;
    if(angleRobotToTarget < -M_PI) angleRobotToTarget += 2.0 * M_PI;

    return angleRobotToTarget;
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
    Position playerPosition = position();

    float dx = (targetPosition.x() - playerPosition.x());
    float dy = (targetPosition.y() - playerPosition.y());
    float distanceMod = sqrtf(powf(dx, 2.0) + powf(dy, 2.0));
    float angleRobotToTarget = getPlayerRotateAngleTo(targetPosition);
    // Get direction and distance to next point in path from navigation algorithm
    std::pair<Angle,float> movement = getNavDirectionDistance(targetPosition, orientation(), avoidTeammates, avoidOpponents, avoidBall, avoidOurGoalArea, avoidTheirGoalArea);

    // Update distance and orientation variables
    distanceMod = movement.second;
    angleRobotToTarget = movement.first.value();

    if (distanceMod < minVel) {
        distanceMod = minVel;
    }

    bool swapSpeed = false;
    if(angleRobotToTarget > float(M_PI) / 2.0f) {
        angleRobotToTarget -= float(M_PI);
        swapSpeed = true;
    } else {
        if (angleRobotToTarget < float(-M_PI) / 2.0f) {
        angleRobotToTarget += float(M_PI);
        swapSpeed = true;
        }
    }

    if(swapSpeed) {
        distanceMod *= -1;
    }

    emit setLinearSpeed(getConstants()->teamColor(), playerId(), distanceMod);
    // angularSpeed*(constant>1) to ensure our angular speed is enough to reach the desired orientation while our player is moving
    emit setAngularSpeed(getConstants()->teamColor(), playerId(), angleRobotToTarget*7);
}

void Player::rotateTo(Position &targetPosition) {
    float angleRobotToTarget = getPlayerRotateAngleTo(targetPosition);

    emit setAngularSpeed(getConstants()->teamColor(), playerId(), angleRobotToTarget);
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

QLinkedList<Position> Player::getPath() const {
    return _nav->getPath();
}
