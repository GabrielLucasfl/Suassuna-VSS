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
}

QString Behavior_MoveTo::name() {
    return "Behavior_MoveTo";
}

void Behavior_MoveTo::configure() {
    _desiredBaseSpeed = getConstants()->playerBaseSpeed();

    // Starting skills
    _skill_goTo = new Skill_GoTo();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
}

void Behavior_MoveTo::run() {
    // When targetPosition is invalid: robot moves forward with the desiredBaseSpeed
    _skill_goTo->setMovementBaseSpeed(_desiredBaseSpeed);
    _skill_goTo->setTargetPosition(_targetPosition);
    setSkill(SKILL_GOTO);
}

void Behavior_MoveTo::setAvoidFlags(bool avoidBall, bool avoidTeammates, bool avoidOpponents, bool avoidOurGoalArea, bool avoidTheirGoalArea) {
    _skill_goTo->setAvoidBall(avoidBall);
    _skill_goTo->setAvoidTeammates(avoidTeammates);
    _skill_goTo->setAvoidOpponents(avoidOpponents);
    _skill_goTo->setAvoidOurGoalArea(avoidOurGoalArea);
    _skill_goTo->setAvoidTheirGoalArea(avoidTheirGoalArea);
}
