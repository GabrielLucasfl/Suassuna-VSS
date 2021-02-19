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

Player::Player(quint8 playerId, Constants *constants, Referee *referee ,WorldMap *worldMap, NavigationAlgorithm *navAlg) : Entity(ENT_PLAYER) {
    _playerId = playerId;
    _constants = constants;
    _worldMap = worldMap;
    _referee = referee;
    _playerRole = nullptr;
    _lastError = 0;
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

void Player::setRole(Role *role) {
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

std::pair<float, float> Player::getWheelsSpeed(float angleToObject, float baseSpeed) {
    // Constants
    float Kp = std::get<0>(getConstants()->playerPID());
    float Kd = std::get<2>(getConstants()->playerPID());

    // Take base data
    float angleRobot = orientation().value();
    float error = smallestAngleDiff(angleRobot, angleToObject);

    // Check if robot front can be reversed to reach target
    bool reversed = false;
    if(fabs(error) > M_PI/2.0 + M_PI/20.0) {
        // Mark as reversed and update robot ori
        reversed = true;
        angleRobot = to180Range(angleRobot + M_PI);

        // Calculates the error and reverses the front of the robot
        error = smallestAngleDiff(angleRobot, angleToObject);
    }

    // PID
    float motorSpeed = (Kp*error) + (Kd * (error - _lastError));// + 0.2 * sumErr;
    _lastError = error;

    // Normalize
    if(baseSpeed != 0) {
        motorSpeed = motorSpeed > baseSpeed ? baseSpeed : motorSpeed;
        motorSpeed = motorSpeed < -baseSpeed ? -baseSpeed : motorSpeed;
    }

    // Update individual wheels speed
    float rightMotorSpeed;
    float leftMotorSpeed;

    if (motorSpeed > 0) {
        leftMotorSpeed = baseSpeed ;
        rightMotorSpeed = baseSpeed - motorSpeed;
    } else {
        leftMotorSpeed = baseSpeed + motorSpeed;
        rightMotorSpeed = baseSpeed;
    }


    // Check if reversed and update wheels speed
    if (reversed) {
        if (motorSpeed > 0) {
            leftMotorSpeed = -baseSpeed + motorSpeed;
            rightMotorSpeed = -baseSpeed ;
        } else {
            leftMotorSpeed = -baseSpeed ;
            rightMotorSpeed = -baseSpeed - motorSpeed;
        }
    }

    return std::make_pair(leftMotorSpeed, rightMotorSpeed);
}

float Player::to180Range(float angle) {
    angle = fmod(angle, 2 * M_PI);
    if (angle < -M_PI) {
        angle = angle + 2 * M_PI;
    } else if (angle > M_PI) {
        angle = angle - 2 * M_PI;
    }
    return angle;
}

float Player::smallestAngleDiff(float target, float source) {
    double diff;
    diff = fmod(target + 2*M_PI, 2 * M_PI) - fmod(source + 2*M_PI, 2 * M_PI);

    if (diff > M_PI) {
        diff = diff - 2 * M_PI;
    } else if (diff < -M_PI) {
        diff = diff + 2 * M_PI;
    }

    return diff;
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
    //movement.first.setValue(movement.first.value() - orientation().value());
    return movement;
}

void Player::goTo(Position &targetPosition, float minVel, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea , bool avoidTheirGoalArea) {
    // Take angle to target
    std::pair<Angle,float> movement = getNavDirectionDistance(targetPosition, orientation(), avoidTeammates, true, avoidBall, true, true);
    float angleToTarget = movement.first.value();

    if(getPlayerDistanceTo(targetPosition) <= getLinearError()) {
        idle();
        return;
    }

    // Take wheels speed
    std::pair<float, float> wheelsSpeed = getWheelsSpeed(angleToTarget, getConstants()->playerBaseSpeed());

    // Send wheels speed to actuator
    emit setWheelsSpeed(playerId(), wheelsSpeed.first, wheelsSpeed.second);
}

void Player::rotateTo(Position &targetPosition) {
    std::pair<float,float> speed = getWheelsSpeed(Utils::getAngle(position(), targetPosition), 0);
    std::cout << "LEFT: " << speed.first << "\nRIGHT: " << speed.second << std::endl;

    emit setWheelsSpeed(playerId(), speed.first, speed.second);
}

void Player::spin(bool isClockWise) {
    if (isClockWise) {
        emit setWheelsSpeed(playerId(), 50, 0);
    } else {
        emit setWheelsSpeed(playerId(), 0, 50);
    }
}

void Player::move(float desiredBaseSpeed) {
    std::pair<float,float> speed = getWheelsSpeed(orientation().value(), desiredBaseSpeed);
    setWheelsSpeed(playerId(), speed.first, speed.second);
}

void Player::idle() {
    emit setWheelsSpeed(playerId(), 0.0, 0.0);
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
