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

#define BALLFACTOR 0.2f

Behavior_GoToBall::Behavior_GoToBall() {
    // Default value
    _offsetBehindBall = 0.0;
    _targetPosition.setPosition(false, 0.0f , 0.0f);
    _referencePosition.setPosition(false, 0.0f, 0.0f);
    _avoidTeammates = false;
    _avoidOpponents = false;
    _avoidBall = false;
    _avoidOurGoalArea = false;
    _avoidTheirGoalArea = false;
    _angle = static_cast<float>(M_PI);
}
QString Behavior_GoToBall::name() {
    return "Behavior_GoToBall";
}

void Behavior_GoToBall::configure() {
    // Default base speed
    _desiredBaseSpeed = getConstants()->playerBaseSpeed();
    _desiredLinearError = player()->getLinearError();

    // Starting skills
    _skill_goTo = new Skill_GoTo();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);

}
void Behavior_GoToBall::run() {
    Position ballPos = getBallProjection();

    // Situation where we use the GoTo skill
    if(_referencePosition.isInvalid()) {
        _targetPosition = ballPos;
    } else {
        _targetPosition = Utils::threePoints(ballPos, _referencePosition, _offsetBehindBall, _angle);
        if(getWorldMap()->getLocations()->isOutsideField(_targetPosition, 0.95f) && isBehindBallXcoord(player()->position())){
            //check if is inside field
            _targetPosition = ballPos;
        }

    }
    _skill_goTo->setTargetPosition(_targetPosition);
    _skill_goTo->setMovementBaseSpeed(_desiredBaseSpeed);
    _skill_goTo->setAvoidBall(_avoidBall);
    _skill_goTo->setAvoidTeammates(_avoidTeammates);
    _skill_goTo->setAvoidOpponents(_avoidOpponents);
    _skill_goTo->setAvoidOurGoalArea(_avoidOurGoalArea);
    _skill_goTo->setAvoidTheirGoalArea(_avoidTheirGoalArea);
    _skill_goTo->setLinearError(_desiredLinearError);
    setSkill(SKILL_GOTO);
}

bool Behavior_GoToBall::isBehindBallXcoord(Position pos) {
    Position posBall = getWorldMap()->getBall().getPosition();
    float robotRadius = 0.035f;
    float ballRadius = 0.0215;
    bool isBehindObjX;
    //ensure that the object is behind ball considering the radius from each other
    if(getWorldMap()->getLocations()->ourSide().isLeft()) {
        isBehindObjX = pos.x() < (posBall.x() - robotRadius - ballRadius);
    }else {
        isBehindObjX = pos.x() > (posBall.x() + robotRadius + ballRadius);
    }
    return isBehindObjX;
}

void Behavior_GoToBall::setAvoidFlags(bool avoidBall, bool avoidTeammates, bool avoidOpponents, bool avoidOurGoalArea,
                                      bool avoidTheirGoalArea) {
    _avoidBall = avoidBall;
    _avoidTeammates = avoidTeammates;
    _avoidOpponents = avoidOpponents;
    _avoidOurGoalArea = avoidOurGoalArea;
    _avoidTheirGoalArea = avoidTheirGoalArea;
}

Position Behavior_GoToBall::getBallProjection() {
    // Ball projection
    Position ballPos = getWorldMap()->getBall().getPosition();
    Velocity ballVel = getWorldMap()->getBall().getVelocity();

    Position ballDirection, ballProj;
    if(ballVel.abs() > 0) {
        ballDirection = Position(true, ballVel.vx()/ballVel.abs(), ballVel.vy()/ballVel.abs());
    } else {
        ballDirection = Position(true, 0, 0);
    }

    float factor = std::min(BALLFACTOR * ballVel.abs(), 0.5f);
    ballProj = Position(true, ballPos.x() + factor*ballDirection.x(), ballPos.y() + factor*ballDirection.y());
    return ballProj;
}

