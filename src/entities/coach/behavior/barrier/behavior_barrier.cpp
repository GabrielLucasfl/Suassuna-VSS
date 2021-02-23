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

#include "behavior_barrier.h"

Behavior_Barrier::Behavior_Barrier() {
    _ellipseAxis_X = 0.0;
    _ellipseAxis_Y = 0.0;
    _isRotationEnabled = false;

}

QString Behavior_Barrier::name() {
    return "Behavior_Barrier";
}

void Behavior_Barrier::configure() {
    _desiredBaseSpeed = getConstants()->playerBaseSpeed();

    // Starting skills
    _skill_goTo = new Skill_GoTo();
    _skill_rotateTo = new Skill_RotateTo();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
    addSkill(SKILL_ROTATE, _skill_rotateTo);
}

void Behavior_Barrier::run() {
    Position ellipseCenter = getWorldMap()->getLocations()->ourGoal();
    Position ballPosition = getWorldMap()->getBall().getPosition();
    Velocity ballVelocity = getWorldMap()->getBall().getVelocity();

    float angularCoefficient;

    // Ellipse equation: x²/a² + y²/b² = 1
    // The center of the ellipse is the goal center, so locomotion will occur in half ellipse (left or right)
    // So the important equation is: x = (+/-)a * sqrt(1 - y²/b²)

    // The barrier has to intercept the ball in the point where the line of interest intercepts the ellipse,
    // or at least, in the nearest point the ellipse is from the line. So it is a optimization problem.
    angularCoefficient = ballVelocity.vy() / ballVelocity.vx();

    // It is also possible to use the line which connects the ball to our goal. This case is much easier to deal with.
    angularCoefficient = (ellipseCenter.y() - ballPosition.y()) / (ellipseCenter.x() - ballPosition.x());
    float linearCoefficient = ballPosition.y() - angularCoefficient * ballPosition.x();

    float interceptX = ellipseCenter.x() - (ellipseCenter.x() / abs(ellipseCenter.x())) * (powf(_ellipseAxis_X, 2.0) * angularCoefficient)
            / sqrt(powf(_ellipseAxis_Y, 2.0) + powf(_ellipseAxis_X, 2.0) * powf(angularCoefficient, 2.0));
    float interceptY = (ellipseCenter.x() / abs(ellipseCenter.x())) * powf(_ellipseAxis_Y, 2.0)
            / sqrt(powf(_ellipseAxis_Y, 2.0) + powf(_ellipseAxis_X, 2.0) * powf(angularCoefficient, 2.0));

}

void Behavior_Barrier::setAvoidFlags(bool avoidBall, bool avoidTeammates, bool avoidOpponents, bool avoidOurGoalArea, bool avoidTheirGoalArea) {
    _skill_goTo->setAvoidBall(avoidBall);
    _skill_goTo->setAvoidTeammates(avoidTeammates);
    _skill_goTo->setAvoidOpponents(avoidOpponents);
    _skill_goTo->setAvoidOurGoalArea(avoidOurGoalArea);
    _skill_goTo->setAvoidTheirGoalArea(avoidTheirGoalArea);
}

Position Behavior_Barrier::getInterceptPosition() {
    Position ellipseCenter = getWorldMap()->getLocations()->ourGoal();
    Position ballPosition = getWorldMap()->getBall().getPosition();
    Velocity ballVelocity = getWorldMap()->getBall().getVelocity();
    float angularCoefficient;

    if (ellipseCenter.y() == ballPosition.y()) {
        return player()->position();
    } else {
        angularCoefficient = (ellipseCenter.x() - ballPosition.x()) / (ellipseCenter.y() - ballPosition.y());
    }
    float linearCoefficient = ballPosition.x() - angularCoefficient * ballPosition.y();


}
