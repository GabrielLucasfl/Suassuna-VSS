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

Behavior_goToBall::Behavior_goToBall()
{

}
QString Behavior_goToBall::name() {
    return "Behavior_Second_Attacker";
}

void Behavior_goToBall::configure() {
    // Starting skills
    _skill_goTo = new Skill_GoTo();
    _skill_move = new Skill_Move();
    _skill_pushball = new Skill_PushBall();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
    addSkill(SKILL_MOVE, _skill_move);
    addSkill(Skill_PUSHBALL, _skill_pushball);
}
void Behavior_goToBall::run() {
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
        _targetPosition = threePoints(ballPos , Locations.theirGoal() , 0.13f , M_PI );
        _skill_goTo->setTargetPosition(_targetPosition);
        _skill_goTo->setMinimalVelocity(_minimalVelocity);
        setSkill(SKILL_GOTO);
    }

}

float Behavior_goToBall::getAngle(const Position &a, const Position &b) {
    return std::atan2(b.y()-a.y(), b.x()-a.x());
}

float Behavior_goToBall::angleDiff(const float A, const float B) {
    float diff = fabs(B - A);
    if(diff > M_PI)
        diff = 2*M_PI - diff;
    return diff;
}
bool Behavior_goToBall::isBehindBall(Position posObjective) {
    Position posBall = player()->getWorldMap()->getBall().getPosition();
    Position posPlayer = player()->getWorldMap()->getLocations();
    float anglePlayer = Behavior_Second_Attacker::getAngle(posBall, posPlayer);
    float angleDest = Behavior_Second_Attacker::getAngle(posBall, posObjective);
    float diff = Behavior_Second_Attacker::angleDiff(anglePlayer, angleDest);
    return (diff>M_PI/18.0f);
}
Position Behavior_goToBall::threePoints(const Position &near, const Position &far, float distance, float beta) {
    Angle alpha(true, atan2(far.y()-near.y(), far.x()-near.x()));
    Angle gama(true, alpha.value()+beta);
    Position p(true, near.x()+distance*cos(gama.value()), near.y()+distance*sin(gama.value()), 0.0);
    return p;
}
