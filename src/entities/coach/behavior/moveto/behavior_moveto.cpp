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
    _minimalVelocity = 0.0;
    _linearSpeed = 0.0;
    _angularSpeed = 0.0;
}

QString Behavior_MoveTo::name() {
    return "Behavior_MoveTo";
}

void Behavior_MoveTo::configure() {
    // Starting skills
    _skill_goTo = new Skill_GoTo();
    _skill_move = new Skill_Move();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
    addSkill(SKILL_MOVE, _skill_move);
}

void Behavior_MoveTo::run() {
    if (_targetPosition.isInvalid()) {
        // Situation where we use the Move skill
        _skill_move->setMovementSpeed(_linearSpeed, _angularSpeed);
        setSkill(SKILL_MOVE);
    } else {
        // Situation where we use the GoTo skill
        _skill_goTo->setTargetPosition(_targetPosition);
        _skill_goTo->setMinimalVelocity(_minimalVelocity);
        setSkill(SKILL_GOTO);
    }
}

void Behavior_MoveTo::setAvoidFlags(bool avoidBall, bool avoidTeammates, bool avoidOpponents, bool avoidOurGoalArea, bool avoidTheirGoalArea) {
    _skill_goTo->setAvoidBall(avoidBall);
    _skill_goTo->setAvoidTeammates(avoidTeammates);
    _skill_goTo->setAvoidOpponents(avoidOpponents);
    _skill_goTo->setAvoidOurGoalArea(avoidOurGoalArea);
    _skill_goTo->setAvoidTheirGoalArea(avoidTheirGoalArea);
}
