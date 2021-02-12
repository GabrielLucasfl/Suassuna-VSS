/***
 * Warthog Robotics
 * University of Sao Paulo (USP) at Sao Carlos
 * http://www.warthog.sc.usp.br/
 *
 * This file is part of WRCoach project.
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

#include "utils.h"
#include <math.h>

Constants* Utils::_constants = nullptr;

float Utils::distance(const Position &a, const Position &b) {
    return sqrt(pow(a.x() - b.x() ,2) + pow(a.y() - b.y(), 2));
}

bool Utils::isPointAtLine(const Position &s1, const Position &s2, const Position &point) {
    const Position projectedPoint = Utils::projectPointAtLine(s1, s2, point);
    const float dist = Utils::distance(point, projectedPoint);

    return (dist <= 0.001f);
}

float Utils::scalarProduct(const Position &A, const Position &B) {
    return (A.x() * B.x() + A.y() * B.y());
}

bool Utils::isPointAtSegment(const Position &s1, const Position &s2, const Position &point) {
    const bool isAtLine = Utils::isPointAtLine(s1, s2, point);
    if(isAtLine == false) {
        return false;
    }
    else {
        Position min, max;
        if(s2.x() >= s1.x()) {
            min = Position(true, s1.x(), min.y());
            max = Position(true, s2.x(), max.y());
        } else {
            min = Position(true, s2.x(), min.y());
            max = Position(true, s1.x(), max.y());
        }
        if(s2.y() >= s1.y()) {
            min = Position(true, min.x(), s1.y());
            max = Position(true, max.x(), s2.y());
        } else {
            min = Position(true, min.x(), s2.y());
            max = Position(true, max.x(), s1.y());
        }
        return (point.x() >= min.x() && point.x() <= max.x() && point.y() >= min.y() && point.y() <= max.y());
    }
}

Position Utils::projectPointAtLine(const Position &s1, const Position &s2, const Position &point) {
    const Position a(true, point.x() - s1.x(), point.y() - s1.y());
    const Position b(true, s2.x() - s1.x(), s2.y() - s1.y());
    const float bModule = sqrt(pow(b.x(), 2) + pow(b.y(), 2));
    const Position bUnitary(true, b.x() / bModule, b.y() / bModule);
    const float scalar = Utils::scalarProduct(a, bUnitary);

    return Position(true, s1.x() + scalar * bUnitary.x(), s1.y() + scalar * bUnitary.y());
}

Position Utils::projectPointAtSegment(const Position &s1, const Position &s2, const Position &point) {
    const Position projectedPoint = Utils::projectPointAtLine(s1, s2, point);
    if(Utils::isPointAtSegment(s1, s2, projectedPoint)) {
        return projectedPoint;
    } else {
        const float d1 = Utils::distance(projectedPoint, s1);
        const float d2 = Utils::distance(projectedPoint, s2);
        return (d1 <= d2) ? s1 : s2;
    }
}

float Utils::distanceToLine(const Position &s1, const Position &s2, const Position &point) {
    const Position projectedPoint = Utils::projectPointAtLine(s1, s2, point);
    const float distance = Utils::distance(point, projectedPoint);

    return (distance <= 0.001f) ? 0 : distance;
}

float Utils::distanceToSegment(const Position &s1, const Position &s2, const Position &point) {
    const Position projectedPoint = Utils::projectPointAtLine(s1, s2, point);
    if(Utils::isPointAtSegment(s1, s2, projectedPoint)) {
        return distanceToLine(s1, s2, point);
    } else {
        const float d1 = Utils::distance(point, s1);
        const float d2 = Utils::distance(point, s2);
        return (d1 <= d2)? d1 : d2;
    }
}

Position Utils::rotatePoint(Position point, float angle){
    float xNew = point.x() * cos(angle) - point.y() * sin(angle);
    float yNew = point.x() * sin(angle) - point.y() * cos(angle);

    return Position(true, xNew, yNew);
}

void Utils::setConstants(Constants *constants){
    _constants = constants;
}

Constants* Utils::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Constants with nullptr value at Utils") + '\n';
    }
    else {
        return _constants;
    }

    return nullptr;
}

float Utils::getAngle(const Position &a, const Position &b)	{
    return atan2(b.y()-a.y(), b.x()-a.x());
}

Position Utils::threePoints(const Position &near, const Position &far, float distance, float beta) {
    Angle alpha(true, atan2(far.y()-near.y(), far.x()-near.x()));
    Angle gama(true, alpha.value()+beta);
    Position p(true, near.x()+distance*cos(gama.value()), near.y()+distance*sin(gama.value()));
    return p;
}
