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

#include "obstacle.h"
#include <src/utils/utils.h>
#include <src/utils/text/text.h>

Obstacle::Obstacle(float obstacleRadius) {
    _obstaclePosition = Position(false, 0.0, 0.0);
    _initialAngle = Angle(false, 0.0);
    _finalAngle = Angle(false, 0.0);
    _obstacleRadius = obstacleRadius;
}

Position Obstacle::position() {
    return _obstaclePosition;
}

Angle Obstacle::initialAngle() {
    return _initialAngle;
}

Angle Obstacle::finalAngle() {
    return _finalAngle;
}

float Obstacle::radius() {
    return _obstacleRadius;
}

void Obstacle::setPosition(Position position) {
    _obstaclePosition = position;
}

void Obstacle::setInitialAngle(Angle initialAngle) {
    _initialAngle = initialAngle;
}

void Obstacle::setFinalAngle(Angle finalAngle){
    _finalAngle = finalAngle;
}

void Obstacle::setRadius(float radius) {
    _obstacleRadius = radius;
}

void Obstacle::calcAnglesFrom(const Position &watcher, float radiusFactor) {
    if(radius() == 0) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold("Obstacle::calcAnglesFrom(args) with radius == 0!") + '\n';
        return;
    }

    // Take objectRadius with factor
    const float R = radius() * radiusFactor;

    // Obstacle angulation
    const float dist = Utils::distance(watcher, position());
    const float fix = atan2(R, dist);

    // Calc angle
    const float angle = Utils::getAngle(watcher, position());

    // Set angles
    setInitialAngle(angle - fix);
    setFinalAngle(angle + fix);
}

void Obstacle::setInitialAngle(float value) {
    _initialAngle = Angle(true, value);
}

void Obstacle::setFinalAngle(float value) {
    _finalAngle = Angle(true, value);
}

Obstacle &Obstacle::operator=(Object object) {
    // Take position from object
    _obstaclePosition = object.getPosition();

    return *this;
}
