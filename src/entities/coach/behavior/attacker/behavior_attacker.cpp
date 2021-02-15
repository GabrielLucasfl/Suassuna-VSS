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

#include "behavior_attacker.h"

Behavior_Attacker::Behavior_Attacker() {

}

QString Behavior_Attacker::name() {
    return "Behavior_Attacker";
}

void Behavior_Attacker::configure() {
    // Starting skills
    _skill_goTo = new Skill_GoTo();
    _skill_move = new Skill_Move();
    _skill_pushball = new Skill_PushBall();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
    addSkill(SKILL_MOVE, _skill_move);
    addSkill(Skill_PUSHBALL, _skill_pushball);
}

void Behavior_Attacker::run() {
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

bool Behavior_Attacker::pushball() {
    Position ballPos = player()->getWorldMap()->getBall().getPosition();
    //list of enemy players
    int enemyPlayers[] = player()->getWorldMap()->getAvailablePlayers();

    float ballAngle = player()->getPlayerRotateAngleTo(ballPos);

    for(int i = 0; i < enemyPlayers.size; i++) {
        //for every enemy, gets the angle between it and the player. Then, compare it to the angle the player
        //is from the ball. If they're similar within a treshold, then the enemy is between the player and the ball
        float enemyAngle = player()->getPlayerRotateAngleTo(enemyPlayers[i].getPosition());

        if(ballAngle - enemyAngle < 0.1 && ballAngle - enemyAngle > -0.1){
            //if the x position from the enemy is less than the ball, it means the enemy player is behind the ball and not blocking it
            if(enemyPlayers[i].getPosition().x() < ballPos.x()){
                return false;
            }
        }
    }
    return true;
}
