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

#include "behavior_moveto.h"

Behavior_MoveTo::Behavior_MoveTo() {
    _targetPosition = Position(false, 0.0, 0.0);
    _isRotationEnabled = false;
    _avoidTeammates = false;
    _avoidOpponents = false;
    _avoidBall = false;
    _avoidOurGoalArea = false;
    _avoidTheirGoalArea = false;
}

QString Behavior_MoveTo::name() {
    return "Behavior_MoveTo";
}

void Behavior_MoveTo::configure() {
    _desiredBaseSpeed = getConstants()->playerBaseSpeed();

    // Starting skills
    _skill_goTo = new Skill_GoTo();
    _skill_rotateTo = new Skill_RotateTo();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
    addSkill(SKILL_ROTATE, _skill_rotateTo);
}

void Behavior_MoveTo::run() {
    if (_isRotationEnabled) {
        _skill_rotateTo->setTargetPosition(_targetPosition);
        setSkill(SKILL_ROTATE);
    } else {
        // Situation where we use the GoTo skill
        _skill_goTo->setTargetPosition(_targetPosition);
        _skill_goTo->setMovementBaseSpeed(_desiredBaseSpeed);
        _skill_goTo->setAvoidBall(_avoidBall);
        _skill_goTo->setAvoidTeammates(_avoidTeammates);
        _skill_goTo->setAvoidOpponents(_avoidOpponents);
        _skill_goTo->setAvoidOurGoalArea(_avoidOurGoalArea);
        _skill_goTo->setAvoidTheirGoalArea(_avoidTheirGoalArea);
        setSkill(SKILL_GOTO);
    }
}

void Behavior_MoveTo::setAvoidFlags(bool avoidBall, bool avoidTeammates, bool avoidOpponents, bool avoidOurGoalArea, bool avoidTheirGoalArea) {
    _avoidBall = avoidBall;
    _avoidTeammates = avoidTeammates;
    _avoidOpponents = avoidOpponents;
    _avoidOurGoalArea = avoidOurGoalArea;
    _avoidTheirGoalArea = avoidTheirGoalArea;
}
