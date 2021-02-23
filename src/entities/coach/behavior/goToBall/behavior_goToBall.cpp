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

#include "behavior_goToBall.h"
#include <src/utils/utils.h>

Behavior_GoToBall::Behavior_GoToBall() {
    // Default value
    _offsetBehindBall = 0.0;
    _targetPosition.setPosition(false, 0.0f , 0.0f);
    _referencePosition.setPosition(false, 0.0f, 0.0f);

}
QString Behavior_GoToBall::name() {
    return "Behavior_GoToBall";
}

void Behavior_GoToBall::configure() {
    // Default base speed
    _desiredBaseSpeed = getConstants()->playerBaseSpeed();

    // Starting skills
    _skill_goTo = new Skill_GoTo();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);

}
void Behavior_GoToBall::run() {
    Position ballPos = getWorldMap()->getBall().getPosition();

    // Situation where we use the GoTo skill
    if(_referencePosition.isInvalid()) {
        _targetPosition = ballPos;
    } else {
        _targetPosition = Utils::threePoints(ballPos , _referencePosition , _offsetBehindBall , M_PI );
        if(_targetPosition.isInvalid()) { // check if is inside field
            _targetPosition = ballPos;
        }
    }
    _skill_goTo->setTargetPosition(_targetPosition);
    _skill_goTo->setMovementBaseSpeed(_desiredBaseSpeed);
    setSkill(SKILL_GOTO);

}

float Behavior_GoToBall::getAngle(const Position &a, const Position &b) {
    return std::atan2(b.y()-a.y(), b.x()-a.x());
}

bool Behavior_GoToBall::isBehindBall(Position posObjective) {
    Position posBall = getWorldMap()->getBall().getPosition();
    Position posPlayer = player()->position();
    float anglePlayer = Utils::getAngle(posBall, posPlayer);
    float angleDest = Utils::getAngle(posBall, posObjective);
    float diff = Utils::angleDiff(anglePlayer, angleDest);
    return (diff > static_cast<float>(M_PI)/18.0f);
}

Position Behavior_GoToBall::ballPrevision() {
    Position ballPosition = getWorldMap()->getBall().getPosition();
    Position enemyGoal = getWorldMap()->getLocations()->theirGoal();
    float angle = atan2((enemyGoal.y() - ballPosition.y()), (enemyGoal.x() - ballPosition.x()));

    Velocity ballVelocity = getWorldMap()->getBall().getVelocity();
    Velocity playerVelocity = getWorldMap()->getPlayer(getConstants()->teamColor(),player()->playerId()).getVelocity();
    float fracVelX = (playerVelocity.vx()/ballVelocity.vx());
    float fracVelY = (playerVelocity.vy()/ballVelocity.vy());

    float futurePositionX = (player()->position().x() + _offsetBehindBall * cos(angle) - ballPosition.x() * fracVelX)/(1-fracVelX);
    float futurePositionY = (player()->position().y() + _offsetBehindBall * sin(angle) - ballPosition.y() * fracVelY)/(1-fracVelY);

    return Position(true, futurePositionX, futurePositionY);
}

void Behavior_GoToBall::setAvoidFlags(bool avoidBall, bool avoidTeammates, bool avoidOpponents, bool avoidOurGoalArea, bool avoidTheirGoalArea) {
    _skill_goTo->setAvoidBall(avoidBall);
    _skill_goTo->setAvoidTeammates(avoidTeammates);
    _skill_goTo->setAvoidOpponents(avoidOpponents);
    _skill_goTo->setAvoidOurGoalArea(avoidOurGoalArea);
    _skill_goTo->setAvoidTheirGoalArea(avoidTheirGoalArea);
}

