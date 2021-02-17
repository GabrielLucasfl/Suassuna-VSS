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

Behavior_GoToBall::Behavior_GoToBall()
{

}
QString Behavior_GoToBall::name() {
    return "Behavior_GoToBall";
}

void Behavior_GoToBall::configure() {
    // Starting skills
    _skill_goTo = new Skill_GoTo();
    _skill_move = new Skill_Move();
    _skill_pushball = new Skill_PushBall();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
    addSkill(SKILL_MOVE, _skill_move);
    addSkill(Skill_PUSHBALL, _skill_pushball);

    // Setting offset
    setOffsetBehindBall(0.0f);
    _targetPosition.setPosition(false, 0.0f , 0.0f);
    _referencePosition.setPosition(false, 0.0f, 0.0f);
}
void Behavior_GoToBall::run() {
    Position ballPos = player()->getWorldMap()->getBall().getPosition();

    // Situation where we use the GoTo skill
    if(_referencePosition.isInvalid()) {
        _targetPosition = ballPos;
    } else {
        _targetPosition = threePoints(ballPos , _referencePosition , _offsetBehindBall , M_PI );
        if(_targetPosition.isInvalid()) { // check if is inside field
            _targetPosition = ballPos;
        }
    }
    _skill_goTo->setTargetPosition(_targetPosition);
    _skill_goTo->setMinimalVelocity(_minimalVelocity);
    setSkill(SKILL_GOTO);


}

float Behavior_GoToBall::getAngle(const Position &a, const Position &b) {
    return std::atan2(b.y()-a.y(), b.x()-a.x());
}

float Behavior_GoToBall::angleDiff(const float A, const float B) {
    float diff = fabs(B - A);
    if(diff > M_PI)
        diff = 2*M_PI - diff;
    return diff;
}
bool Behavior_GoToBall::isBehindBall(Position posObjective) {
    Position posBall = player()->getWorldMap()->getBall().getPosition();
    Position posPlayer = player()->position();
    float anglePlayer = Behavior_GoToBall::getAngle(posBall, posPlayer);
    float angleDest = Behavior_GoToBall::getAngle(posBall, posObjective);
    float diff = Behavior_GoToBall::angleDiff(anglePlayer, angleDest);
    return (diff>M_PI/18.0f);
}
Position Behavior_GoToBall::threePoints(const Position &near, const Position &far, float distance, float beta) {
    Angle alpha(true, atan2(far.y()-near.y(), far.x()-near.x()));
    Angle gama(true, alpha.value()+beta);
    Position p(true, near.x()+distance*cos(gama.value()), near.y()+distance*sin(gama.value()));
    return p;
}
