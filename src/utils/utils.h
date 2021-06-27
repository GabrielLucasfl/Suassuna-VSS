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

#ifndef UTILS_H
#define UTILS_H

#include <src/constants/constants.h>
#include <src/utils/types/position/position.h>
#include <src/utils/types/color/color.h>
#include <src/utils/types/angle/angle.h>

class Utils {
public:
    // Distance related
    static float distance(const Position &a, const Position &b);
    static float scalarProduct(const Position &A, const Position &B);
    static float distanceToLine(const Position &s1, const Position &s2, const Position &point);
    static float distanceToSegment(const Position &s1, const Position &s2, const Position &point);

    // Angle related
    static float getAngle(const Position &a, const Position &b);
    static float angleDiff(const float A, const float B);
    static float to180Range(float angle);
    static void angleLimitZeroTwoPi(float *angle);
    static Position rotatePoint(Position point, float angle);

    // Projection related
    static Position threePoints(const Position &near, const Position &far, float distance, float beta);
    static Position projectPointAtLine(const Position &s1, const Position &s2, const Position &point);
    static Position projectPointAtSegment(const Position &s1, const Position &s2, const Position &point);

    // Checkers
    static bool isPointAtLine(const Position &s1, const Position &s2, const Position &point);
    static bool isPointAtSegment(const Position &s1, const Position &s2, const Position &point);

    // Others
    static void limitValue(float *value, float minValue, float maxValue);
    static Position segmentsIntersect(Position sA1, Position sA2, Position sB1, Position sB2);
    static Position hasInterceptionSegments(const Position &s1, const Position &s2, const Position &s3, const Position &s4);

    static void setConstants(Constants *constants);

private:
    static Constants *_constants;
    static Constants* getConstants();
};

#endif // UTILS_H
