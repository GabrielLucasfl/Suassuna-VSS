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
#define FLOAT_MARGIN 10e-4f

Constants* Utils::_constants = nullptr;

float Utils::distance(const Position &a, const Position &b) {
    return sqrt(pow(a.x() - b.x() ,2) + pow(a.y() - b.y(), 2));
}

float Utils::scalarProduct(const Position &A, const Position &B) {
    return (A.x() * B.x() + A.y() * B.y());
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

float Utils::getAngle(const Position &a, const Position &b)	{
    return atan2(b.y()-a.y(), b.x()-a.x());
}

float Utils::angleDiff(const float A, const float B) {
    float diff = fabs(B - A);
    if(diff > static_cast<float>(M_PI))
        diff = 2*static_cast<float>(M_PI) - diff;
    return diff;
}

float Utils::to180Range(float angle) {
    angle = fmod(angle, 2 * M_PI);
    if (angle < -M_PI) {
        angle = angle + 2 * M_PI;
    } else if (angle > M_PI) {
        angle = angle - 2 * M_PI;
    }
    return angle;
}

void Utils::angleLimitZeroTwoPi(float *angle) {
    while(*angle < 0)
        *angle += 2.0*M_PI;
    while(*angle > 2.0*M_PI)
        *angle -= 2.0*M_PI;
}

Position Utils::rotatePoint(Position point, float angle){
    float xNew = point.x() * cos(angle) - point.y() * sin(angle);
    float yNew = point.x() * sin(angle) - point.y() * cos(angle);

    return Position(true, xNew, yNew);
}

Position Utils::threePoints(const Position &near, const Position &far, float distance, float beta) {
    Angle alpha(true, atan2(far.y()-near.y(), far.x()-near.x()));
    Angle gama(true, alpha.value()+beta);
    Position p(true, near.x()+distance*cos(gama.value()), near.y()+distance*sin(gama.value()));
    return p;
}

float Utils::angleToBall(const Position &ballPos, const Position &theirGoal, const Position &playerPos){
    //float alpha = (acos((playerPos.x()-ballPos.x())/Utils::distance(ballPos, theirGoal))) + Utils::getAngle(ballPos, theirGoal);

    float alpha =  getAngle(ballPos, theirGoal) - getAngle(playerPos, ballPos);

    return alpha;
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

bool Utils::isPointAtLine(const Position &s1, const Position &s2, const Position &point) {
    const Position projectedPoint = Utils::projectPointAtLine(s1, s2, point);
    const float dist = Utils::distance(point, projectedPoint);

    return (dist <= 0.001f);
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

void Utils::limitValue(float *value, float minValue, float maxValue) {
    if(*value > maxValue)
        *value = maxValue;
    else if(*value < minValue)
        *value = minValue;
}

Position Utils::segmentsIntersect(Position sA1, Position sA2, Position sB1, Position sB2) {
    float denominador = (sB2.y()-sB1.y())*(sA2.x() - sA1.x()) - (sB2.x() - sB1.x())*(sA2.y()-sA1.y());

    if (denominador != 0.0f){ // retas se interceptam
        float u = ((sB2.x() - sB1.x())*(sA1.y() - sB1.y()) - (sB2.y() - sB1.y())*(sA1.x() - sB1.x()));
        float v = ((sA2.x() - sA1.x())*(sA1.y() - sB1.y()) - (sA2.y() - sA1.y())*(sA1.x() - sB1.x()));

        if(u != 0.0f){   //
            float xIntersec = sA1.x() + (u/denominador)*(sA2.x()-sA1.x());
            float yIntersec = sA1.y() + (u/denominador)*(sA2.y()-sA1.y());
            if(xIntersec <= std::min(std::max(sA1.x(), sA2.x()), std::max(sB1.x(), sB2.x()))
               && xIntersec >= std::max(std::min(sA1.x(), sA2.x()), std::min(sB1.x(), sB2.x()))) {
                return Position(true, xIntersec, yIntersec);
            }else {
                return Position(false, xIntersec, yIntersec);
            }
        }else if(v != 0.0f){
            float xIntersec = sB1.x() + (v/denominador)*(sB2.x()-sB1.x());
            float yIntersec = sB1.y() + (v/denominador)*(sB2.y()-sB1.y());
            if(xIntersec <= std::min(std::max(sA1.x(), sA2.x()), std::max(sB1.x(), sB2.x()))
               && xIntersec >= std::max(std::min(sA1.x(), sA2.x()), std::min(sB1.x(), sB2.x()))) {
                return Position(true, xIntersec, yIntersec);
            }else {
                return Position(false, xIntersec, yIntersec);
            }
        }else{ // caso especial -> as linhas sao coincidentes
            return Position(true, sA1.x(), sA1.y());
        }

    }else{ //Os segmentos sao paralelos
        return Position(false, 0.0f,0.0f);
    }
}

Position Utils::hasInterceptionSegments(const Position &s1, const Position &s2, const Position &s3, const Position &s4){
    float denominador = (s4.y()-s3.y())*(s2.x() - s1.x()) - (s4.x() - s3.x())*(s2.y()-s1.y());

    if (denominador!=0.0f){ // segmentos se interceptam
        float u = ((s4.x() - s3.x())*(s1.y() - s3.y()) - (s4.y() - s3.y())*(s1.x() - s3.x()));
        float v = ((s2.x() - s1.x())*(s1.y() - s3.y()) - (s2.y() - s1.y())*(s1.x() - s3.x()));

        if(u !=0.0f){
            float xIntersec = s1.x() + (u/denominador)*(s2.x()-s1.x());
            float yIntersec = s1.y() + (u/denominador)*(s2.y()-s1.y());
            return Position(true, xIntersec, yIntersec);
        }else if(v !=0.0f){
            float xIntersec = s3.x() + (v/denominador)*(s4.x()-s3.x());
            float yIntersec = s3.y() + (v/denominador)*(s4.y()-s3.y());
            return Position(true, xIntersec, yIntersec);
        }else{ // caso especial -> as linhas sao coincidentes
            return Position(true, s1.x(), s1.y());
        }

    }else{ //Os segmentos sao paralelos
        return Position(false, 0.0f,0.0f);
    }
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
