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

#define DISPLACEMENT 6

Player::Player(quint8 playerId, Constants *constants, Referee *referee ,WorldMap *worldMap,
               NavigationAlgorithm *navAlg) : Entity(ENT_PLAYER) {
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

Position Player::getPlayerDesiredPosition() {
    return _desiredPosition;
}

float Player::getPlayerDistanceTo(Position &targetPosition) {
    return sqrt(powf(position().x() - targetPosition.x(), 2) + powf(position().y() - targetPosition.y(), 2));
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
    float error = getSmallestAngleDiff(angleRobot, angleToObject);

    // Check if robot front can be reversed to reach target
    bool reversed = false;
    if(fabs(error) > static_cast<float>(M_PI)/2 + static_cast<float>(M_PI)/20) {
        // Mark as reversed and update robot ori
        reversed = true;
        angleRobot = Utils::to180Range(angleRobot + static_cast<float>(M_PI));

        // Calculates the error and reverses the front of the robot
        error = getSmallestAngleDiff(angleRobot, angleToObject);
    }

    // Motor reference
    float motorSpeed = ((Kp*error) + (Kd * (error - _lastError)));// + 0.2 * sumErr;
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

    // PID
    PID linearPID(6.0f, 0.0f, 1.0f);
    linearPID.setOutputLimits(30.f);
    float linearPIDOutput = linearPID.getOutput(0.0f, getPlayerDistanceTo(_desiredPosition));
    PID angularPID(0.0f, 0.0f, 0.0f);
    angularPID.setOutputLimits(30.f);
    float angularPIDOutput = linearPID.getOutput(0.0f, getPlayerDistanceTo(_desiredPosition));

    leftMotorSpeed *= (linearPIDOutput - angularPIDOutput);
    rightMotorSpeed *= (linearPIDOutput + angularPIDOutput);

    return std::make_pair(leftMotorSpeed, rightMotorSpeed);
}

float Player::getSmallestAngleDiff(float target, float source) {
    // Gets the smallest angle between the target and one of the "fronts" of the robot
    float diff;
    diff = fmod(target + 2 * static_cast<float>(M_PI), 2 * static_cast<float>(M_PI))
            - fmod(source + 2 * static_cast<float>(M_PI), 2 * static_cast<float>(M_PI));

    if (diff > static_cast<float>(M_PI)) {
        diff = diff - 2 * static_cast<float>(M_PI);
    } else if (diff < static_cast<float>(-M_PI)) {
        diff = diff + 2 * static_cast<float>(M_PI);
    }
    return diff;
}

float Player::getLinearError() {
    return 0.05f; // 5cm
}

float Player::getAngularError() {
    return 0.02f; // ~= 1.15 deg
}

bool Player::isLookingTo(Position &pos, float error) {
    // Taking the reference angle
    float referenceAngle = atan2(pos.y() - position().y(), pos.x() - position().x());

    // Analysing condition
    float angleDifference = abs(orientation().value() - referenceAngle);
    if (angleDifference > abs(static_cast<float>(M_PI) - angleDifference)) {
        angleDifference = abs(static_cast<float>(M_PI) - angleDifference);
    }
    if (angleDifference < error) {
        return true;
    } else {
        return false;
    }
}

bool Player::isBehindBallXCoord(Position pos, float inc) {
    // Inspects if the player object of interest in between the ball and the player's goal, comparing the X coordinate
    Position posBall = getWorldMap()->getBall().getPosition();
    float robotRadius = 0.035f;
    float ballRadius = 0.0215f;
    bool isBehindObjX;
    if(getWorldMap()->getLocations()->ourSide().isLeft()) {
        isBehindObjX = pos.x() < (posBall.x() - robotRadius - ballRadius - inc);
    } else {
        isBehindObjX = pos.x() > (posBall.x() + robotRadius + ballRadius + inc);
    }
    return isBehindObjX;
}

QString Player::roleName() {
    _mutexRole.lock();
    QString roleName = (_playerRole == nullptr) ? "No Role" : _playerRole->name();
    _mutexRole.unlock();

    return roleName;
}

QString Player::behaviorName() {
    _mutexRole.lock();
    QString behaviorName = (_playerRole == nullptr) ? "No Behavior" : _playerRole->actualBehaviorName();
    _mutexRole.unlock();

    return behaviorName;
}

void Player::setRole(Role *role) {
    _mutexRole.lock();

    // Set old role player to nullptr
    if(_playerRole != nullptr) _playerRole->setPlayer(nullptr);

    // Set new role
    _playerRole = role;

    // Assign player to this role
    if(_playerRole != nullptr) _playerRole->setPlayer(this);

    _mutexRole.unlock();
}

QLinkedList<Position> Player::getPath() const {
    return _nav->getPath();
}

void Player::goTo(float desiredBaseSpeed, float desiredLinearError, bool avoidTeammates,
                  bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea , bool avoidTheirGoalArea) {

    Position destination = limitPosInsideField(_desiredPosition);
    destination = projectPosOutsideGoalArea(destination, avoidOurGoalArea, avoidTheirGoalArea);

    // Take angle to target
    float angleToTarget;
    // If there isn't a valid target position: move forward
    if(destination.isInvalid()) {
        angleToTarget = orientation().value();
    }
    // If there is a valid target position: move towards it
    else {
        std::pair<Angle,float> movement = getNavDirectionDistance(destination, orientation(), avoidTeammates, avoidOpponents,
                                                                  avoidBall, avoidOurGoalArea, avoidTheirGoalArea);
        angleToTarget = movement.first.value();
        if(getPlayerDistanceTo(destination) <= desiredLinearError) {
            idle();
            return;
        }
    }

    // Take wheels speed
    std::pair<float, float> wheelsSpeed = getWheelsSpeed(angleToTarget, desiredBaseSpeed);

    // Send wheels speed to actuator
    emit setWheelsSpeed(playerId(), wheelsSpeed.first, wheelsSpeed.second);
}

void Player::rotateTo() {
    std::pair<float,float> speed = getWheelsSpeed(Utils::getAngle(position(), _desiredPosition), 0);

    emit setWheelsSpeed(playerId(), speed.first, speed.second);
}

void Player::spin(bool isClockWise) {
    if (isClockWise) {
        emit setWheelsSpeed(playerId(), 80, -80);
    } else {
        emit setWheelsSpeed(playerId(), -80, 80);
    }
}

void Player::idle() {
    emit setWheelsSpeed(playerId(), 0.0, 0.0);
}

Position Player::projectPosOutsideGoalArea(Position targetPosition, bool avoidOurArea, bool avoidTheirArea) {
    // Check avoids
    if (!avoidOurArea && !avoidTheirArea) {
        return targetPosition;
    }

    //Target position is already in a valid position
    if (!getWorldMap()->getLocations()->isInsideTheirArea(targetPosition)
            && !getWorldMap()->getLocations()->isInsideOurArea(targetPosition)) {
        return targetPosition;
    }

    // Check our area
    if (avoidOurArea && getWorldMap()->getLocations()->isInsideOurArea(targetPosition)) {
        // Check quadrant
        if (targetPosition.y() >= 0.0f) {
            Position topCorner;
            if (getWorldMap()->getLocations()->ourSide().isLeft()) {
                topCorner = getWorldMap()->getLocations()->ourAreaLeftFrontCorner();
            }
            else {
                topCorner = getWorldMap()->getLocations()->ourAreaRightFrontCorner();
            }
            // Compare Distances
            float horizontalDist = abs(targetPosition.x() - topCorner.x());
            float verticalDist = abs(targetPosition.y() - topCorner.y());

            if (horizontalDist <= verticalDist) {
                float directionX = 0.01f * ((topCorner.x() - targetPosition.x())/abs(topCorner.x() - targetPosition.x()));
                return Position(true, topCorner.x() + DISPLACEMENT * directionX, targetPosition.y());
            } else {
                return Position(true, targetPosition.x(), topCorner.y() + DISPLACEMENT * 0.01f);
            }

        } else {
            Position bottomCorner;
            if (getWorldMap()->getLocations()->ourSide().isLeft()) {
                bottomCorner = getWorldMap()->getLocations()->ourAreaRightFrontCorner();
            }
            else {
                bottomCorner = getWorldMap()->getLocations()->ourAreaLeftFrontCorner();
            }

            // Compare Distances
            float horizontalDist = abs(targetPosition.x() - bottomCorner.x());
            float verticalDist = abs(targetPosition.y() - bottomCorner.y());

            if (horizontalDist <= verticalDist) {
                float directionX = 0.01f * ((bottomCorner.x() - targetPosition.x())/abs(bottomCorner.x()
                                                                                       - targetPosition.x()));
                return Position(true, bottomCorner.x() + DISPLACEMENT * directionX, targetPosition.y());
            } else {
                return Position(true, targetPosition.x(), bottomCorner.y() - DISPLACEMENT * 0.01f);
            }
        }
    }
    // Check their area
    if (avoidTheirArea && getWorldMap()->getLocations()->isInsideTheirArea(targetPosition)) {
        // Check quadrant
        if (targetPosition.y() >= 0.0f) {
            Position topCorner;
            if (getWorldMap()->getLocations()->theirSide().isLeft()) {
                topCorner = getWorldMap()->getLocations()->theirAreaLeftFrontCorner();
            }
            else {
                topCorner = getWorldMap()->getLocations()->theirAreaRightFrontCorner();
            }

            // Compare Distances
            float horizontalDist = abs(targetPosition.x() - topCorner.x());
            float verticalDist = abs(targetPosition.y() - topCorner.y());

            if (horizontalDist <= verticalDist) {
                float directionX = 0.01f * ((topCorner.x() - targetPosition.x())/abs(topCorner.x() - targetPosition.x()));
                return Position(true, topCorner.x() + DISPLACEMENT * directionX, targetPosition.y());
            } else {
                return Position(true, targetPosition.x(), topCorner.y() + DISPLACEMENT * 0.01f);
            }

        } else {
            Position bottomCorner;
            if (getWorldMap()->getLocations()->theirSide().isLeft()) {
                bottomCorner = getWorldMap()->getLocations()->theirAreaRightFrontCorner();
            }
            else {
                bottomCorner = getWorldMap()->getLocations()->theirAreaLeftFrontCorner();
            }

            // Compare Distances
            float horizontalDist = abs(targetPosition.x() - bottomCorner.x());
            float verticalDist = abs(targetPosition.y() - bottomCorner.y());

            if (horizontalDist <= verticalDist) {
                float directionX = 0.01f * (bottomCorner.x() - targetPosition.x())/abs(bottomCorner.x() - targetPosition.x());
                return Position(true, bottomCorner.x() + DISPLACEMENT * directionX, targetPosition.y());
            } else {
                return Position(true, targetPosition.x(), bottomCorner.y() - DISPLACEMENT * 0.01f);
            }
        }
    }
    return targetPosition;
}

Position Player::limitPosInsideField(Position dest) {
    float minX = getWorldMap()->getLocations()->fieldMinX();
    float maxX = getWorldMap()->getLocations()->fieldMaxX();
    float minY = getWorldMap()->getLocations()->fieldMinY();
    float maxY = getWorldMap()->getLocations()->fieldMaxY();
    Position s1;
    Position s2;
    float offset = 0.07f;

    // Check if intersect areas


    // If dest is above upper wall
    if(dest.y() > maxY) {
        // Project on upper wall line
        s1 = Position(true, minX, maxY);
        s2 = Position(true, maxX, maxY);
        Position proj = Utils::projectPointAtLine(s1, s2, dest);
        dest.setPosition(true, proj.x(), proj.y() - offset);
    }
    // If dest is bellow lower wall
    if(dest.y() < minY) {
        // Project on lower wall line
        s1 = Position(true, minX, minY);
        s2 = Position(true, maxX, minY);
        Position proj = Utils::projectPointAtLine(s1, s2, dest);
        dest.setPosition(true, proj.x(), proj.y() + offset);
    }
    // If dest is behind left wall
    if(dest.x() < minX) {
        // Project on left wall line
        s1 = Position(true, minX, minY);
        s2 = Position(true, minX, maxY);
        Position proj = Utils::projectPointAtLine(s1, s2, dest);
        dest.setPosition(true, proj.x() + offset, proj.y());
    }
    // If dest if behind right wall
    if(dest.x() > maxX) {
        // Project on right wall line
        s1 = Position(true, maxX, minY);
        s2 = Position(true, maxX, maxY);
        Position proj = Utils::projectPointAtLine(s1, s2, dest);
        dest.setPosition(true, proj.x() - offset, proj.y());
    }
    return dest;
}

void Player::initialization() {
    QString teamColorName = getConstants()->teamColorName().toUpper();
    std::cout << Text::cyan("[PLAYER " + teamColorName.toStdString() + ":" + std::to_string(playerId()) + "] ", true)
                 + Text::bold("Thread started.") + '\n';
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
    std::cout << Text::cyan("[PLAYER " + teamColorName.toStdString() + ":" + std::to_string(playerId()) + "] ", true)
                 + Text::bold("Thread ended.") + '\n';
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

WorldMap* Player::getWorldMap() {
    if(_worldMap == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("WorldMap with nullptr value at Player") + '\n';
    }
    else {
        return _worldMap;
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

std::pair<Angle,float> Player::getNavDirectionDistance(const Position &destination, const Angle &positionToLook,
                                                       bool avoidTeammates, bool avoidOpponents, bool avoidBall,
                                                       bool avoidOurGoalArea, bool avoidTheirGoalArea) {

    _nav->setGoal(destination, positionToLook, avoidTeammates, avoidOpponents, avoidBall, avoidOurGoalArea,
                  avoidTheirGoalArea);

    std::pair<Angle,float> movement = std::make_pair(_nav->getDirection(), _nav->getDistance());
    //movement.first.setValue(movement.first.value() - orientation().value());
    return movement;
}

void Player::receiveFoul(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    _mutexRole.lock();

    if(_playerRole != nullptr) {
        QPair<Position, Angle> placementPosition = _playerRole->getPlacementPosition(foul, forTeam, atQuadrant);
        emit sendPlacement(this->playerId(), placementPosition.first, placementPosition.second);
    }

    _mutexRole.unlock();
}
