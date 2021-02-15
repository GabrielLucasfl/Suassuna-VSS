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

#include "behavior_goForward.h"

Behavior_GoForward::Behavior_GoForward() {

}

QString Behavior_GoForward::name() {
    return "Behavior_GoForward";
}

void Behavior_GoForward::configure() {
    // Starting skills
    _skill_goTo = new Skill_GoTo();
    _skill_move = new Skill_Move();
    _skill_pushball = new Skill_PushBall();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
    addSkill(SKILL_MOVE, _skill_move);
    addSkill(Skill_PUSHBALL, _skill_pushball);
}

void Behavior_GoForward::run() {
    Position ballPos = player()->getWorldMap()->getBall().getPosition();

    if (_targetPosition.isInvalid()) {
        // Situation where we use the Move skill
        _skill_move->setMovementSpeed(_linearSpeed, _angularSpeed);
        setSkill(SKILL_MOVE);
    } else {
        if (player()->getPlayerDistanceTo(ballPos) < 0.3){
            _skill_pushball->setSpeedAndOmega(_linearSpeed, _angularSpeed);
        }
        // Situation where we use the GoTo skill
        _skill_goTo->setTargetPosition(_targetPosition);
        _skill_goTo->setMinimalVelocity(_minimalVelocity);
        setSkill(SKILL_GOTO);
    }
}

bool Behavior_GoForward::pushball() {
    //list of enemy players
    QList<quint8> enemyPlayers = player()->getWorldMap()->getAvailablePlayers(Colors::YELLOW);

    for(int i = 0; i < enemyPlayers.length(); i++) {
        //for every enemy, gets the angle between it and the player. If the angle is small, it means the enemy
        //player is blocking the player
        Position enemyPos = player()->getWorldMap()->getPlayer(Colors::YELLOW, i).getPosition();
        float enemyAngle = player()->getPlayerRotateAngleTo(enemyPos);

        if(enemyAngle < 0.1 && enemyAngle > -0.1){
            //if the enemy player x position is smaller tan the player's x position, it means it's behind the player
            if(enemyPos.x() < player()->position().x()) {
                return false;
            }
        }
    }
    return true;
}
