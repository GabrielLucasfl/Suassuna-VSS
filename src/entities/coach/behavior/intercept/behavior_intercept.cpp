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
    _velocityNeeded = 0.0f;
    _velocityFactor = 1.0f;
    _activateVelocityNeeded = false;
}

QString Behavior_Intercept::name() {
    return "Behavior_Intercept";
}

void Behavior_Intercept::configure() {
    // Starting skills
    _skill_goTo = new Skill_GoTo();
    _skill_spin = new Skill_Spin();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
    addSkill(SKILL_SPIN, _skill_spin);
}

void Behavior_Intercept::run() {
    if(_firstLimitationPoint.y() > _secondLimitationPoint.y()){
        std::swap(_firstLimitationPoint, _secondLimitationPoint);
    }

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
    _skill_goTo->setTargetPosition(_interceptPos);
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
    // Taking the angular coefficients
    float objectAngular = _objectVel.vy() / _objectVel.vx();
    float trajectoryAngular = (_firstLimitationPoint.y() - _secondLimitationPoint.y())
            / (_firstLimitationPoint.x() - _secondLimitationPoint.x());

    // Analysing vector parallelism
    if (trajectoryAngular == objectAngular) {
        return player()->position();
    } else {
        // Taking the linear coefficients
        float objectLinear = _objectPos.y() - objectAngular * _objectPos.x();
        float interceptLinear = _firstLimitationPoint.y() - trajectoryAngular * _firstLimitationPoint.x();

        // Taking the inteception Position
        float intercept_x = (objectLinear - interceptLinear) / (trajectoryAngular - objectAngular);
        float intercept_y = _interceptPos.x() * objectAngular + objectLinear;
        Position interceptPosition(true, intercept_x, intercept_y);

        float segmentDistance = sqrt(pow(_firstLimitationPoint.x() - _secondLimitationPoint.x(),2)
                                     + pow(_firstLimitationPoint.y() - _secondLimitationPoint.y(),2));
        float firstDistance = sqrt(pow(_firstLimitationPoint.x() - interceptPosition.x(),2)
                                   + pow(_firstLimitationPoint.y() - interceptPosition.y(),2));
        float secondDistance = sqrt(pow(_secondLimitationPoint.x() - interceptPosition.x(),2)
                                    + pow(_secondLimitationPoint.y() - interceptPosition.y(),2));

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
}
