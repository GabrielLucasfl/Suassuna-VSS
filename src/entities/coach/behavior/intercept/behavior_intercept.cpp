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

#include "behavior_intercept.h"

Behavior_Intercept::Behavior_Intercept() {
    _firstLimitationPoint = Position(false, 0.0, 0.0);
    _secondLimitationPoint = Position(false, 0.0, 0.0);
    _objectPos = Position(false, 0.0, 0.0);
    _objectVel = Velocity(false, 0.0, 0.0);
    _spinEnabled = false;
}

QString Behavior_Intercept::name() {
    return "Behavior_Intercept";
}

void Behavior_Intercept::setInterceptSegment(Position firstPoint, Position secondPoint) {
    _firstLimitationPoint = firstPoint;
    _secondLimitationPoint = secondPoint;
}

void Behavior_Intercept::configure() {
    _baseSpeed = getConstants()->playerBaseSpeed();

    // Starting skills
    _skill_goTo = new Skill_GoTo();
    _skill_spin = new Skill_Spin();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
    addSkill(SKILL_SPIN, _skill_spin);
}

void Behavior_Intercept::run() {
    // Check ball speed (maybe a error)
    if(_objectVel.abs() <= 0.01f) {
        _interceptPos = player()->position();
    } else {
        if (_firstLimitationPoint.isInvalid() || _secondLimitationPoint.isInvalid()) {
            _interceptPos = getOrthogonalProjection();
        } else {
            _interceptPos = getInterceptionPosition();
        }
    }

    // Distances to the intercept position
    float objectDistance = sqrt(powf(_objectPos.x() - _interceptPos.x(),2) + powf(_objectPos.y() - _interceptPos.y(),2));
    float playerDistance = player()->getPlayerDistanceTo(_interceptPos);

    Position uninatyVelocityVector(true, (_interceptPos.x() - player()->position().x()) / playerDistance,
                                   (_interceptPos.y() - player()->position().y()) / playerDistance);
    float playerVelocity = playerDistance * _objectVel.abs() / objectDistance;
    _interceptVel = Velocity(true, playerVelocity * uninatyVelocityVector.x(), playerVelocity * uninatyVelocityVector.y());

    if (_spinEnabled && playerDistance < player()->getLinearError()) {
        setSkill(SKILL_SPIN);
    } else {
        _skill_goTo->setTargetPosition(_interceptPos);
        _skill_goTo->setMovementBaseSpeed(_baseSpeed);
        setSkill(SKILL_GOTO);
    }
}

Position Behavior_Intercept::getOrthogonalProjection() {
    // Taking the vector distance from object to player
    Position playerVector(true, player()->position().x() - _objectPos.x(), player()->position().y() - _objectPos.y());

    // Taking the numeric proportion from orthogonal projection formula
    float numericProportion = (playerVector.x() * _objectVel.vx() + playerVector.y() * _objectVel.vy()) / powf(_objectVel.abs(), 2);

    // Taking the orthogonal projection position
    float projection_x = _objectPos.x() + numericProportion * _objectVel.vx();
    float projection_y = _objectPos.y() + numericProportion * _objectVel.vy();

    return Position(true, projection_x, projection_y);
}

Position Behavior_Intercept::getInterceptionPosition() {
    // We will create lines/vectors of movement, which means we must take the anlgular coefficiens of them.
    // This may be a problem if the denominator is null. We have to analyse the 4 possible cases in order
    // to predict what to do to interpt the target.
    float trajectory_x = _firstLimitationPoint.x() - _secondLimitationPoint.x();
    float trajectory_y = _firstLimitationPoint.y() - _secondLimitationPoint.y();
    Position interceptPosition;

    if (_objectVel.vx() == 0.0f && trajectory_x == 0.0f) {
        // Case where both anular coefficents goes to infinite which also means the lines are parallels so
        // the interception is impossible.
        return player()->position();
    }
    else if (_objectVel.vx() == 0.0f && trajectory_x != 0.0f) {
        // Case where the object of interception moves on the y direction so the line of movement of the
        // object is 'x = _objectPos.x()'.

        // Taking the angular coefficient
        float trajectoryAngular = trajectory_y / trajectory_x;

        // Taking the linear coefficient
        float trajectoryLinear = _firstLimitationPoint.y() - trajectoryAngular * _firstLimitationPoint.x();

        // Taking the inteception Position
        float intercept_y = trajectoryAngular * _objectPos.x() + trajectoryLinear;
        interceptPosition = Position(true, _objectPos.x(), intercept_y);
    }
    else if (_objectVel.vx() != 0.0f && trajectory_x == 0.0f) {
        // Case where our player  moves on the y direction so the line of movement of the object is
        // 'x = _firstLimitationPoint.x()', once our player x position will be the same of this point.

        // Taking the angular coefficient
        float objectAngular = _objectVel.vy() / _objectVel.vx();

        // Taking the linear coefficient
        float objectLinear = _objectPos.y() - objectAngular * _objectPos.x();

        // Taking the inteception Position
        float intercept_y = objectAngular * _firstLimitationPoint.x() + objectLinear;
        interceptPosition = Position(true, _firstLimitationPoint.x(), intercept_y);
    } else {
        // Taking the angular coefficients
        float objectAngular = _objectVel.vy() / _objectVel.vx();
        float trajectoryAngular = trajectory_y / trajectory_x;

        // Analysing vector parallelism
        if (trajectoryAngular == objectAngular) {
            return player()->position();
        } else {
            // Taking the linear coefficients
            float objectLinear = _objectPos.y() - objectAngular * _objectPos.x();
            float trajectoryLinear = _firstLimitationPoint.y() - trajectoryAngular * _firstLimitationPoint.x();

            // Taking the inteception Position
            float intercept_x = (objectLinear - trajectoryLinear) / (trajectoryAngular - objectAngular);
            float intercept_y = intercept_x * objectAngular + objectLinear;
            interceptPosition = Position(true, intercept_x, intercept_y);
        }
    }

    float segmentDistance = sqrt(powf(_firstLimitationPoint.x() - _secondLimitationPoint.x(),2)
                                 + powf(_firstLimitationPoint.y() - _secondLimitationPoint.y(),2));
    float firstDistance = sqrt(powf(_firstLimitationPoint.x() - interceptPosition.x(),2)
                               + powf(_firstLimitationPoint.y() - interceptPosition.y(),2));
    float secondDistance = sqrt(powf(_secondLimitationPoint.x() - interceptPosition.x(),2)
                                + powf(_secondLimitationPoint.y() - interceptPosition.y(),2));

    // Checking the limitation
    if (firstDistance + secondDistance > segmentDistance) {
        if (firstDistance < secondDistance) {
            return _firstLimitationPoint;
        } else {
            return _secondLimitationPoint;
        }
    } else {
        return interceptPosition;
    }
}
