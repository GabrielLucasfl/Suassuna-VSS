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

class Utils {
public:
    static float scalarProduct(const Position &A, const Position &B);
    static float distance(const Position &a, const Position &b);
    static bool isPointAtLine(const Position &s1, const Position &s2, const Position &point);
    static bool isPointAtSegment(const Position &s1, const Position &s2, const Position &point);
    static Position projectPointAtLine(const Position &s1, const Position &s2, const Position &point);
    static Position projectPointAtSegment(const Position &s1, const Position &s2, const Position &point);
    static float distanceToLine(const Position &s1, const Position &s2, const Position &point);
    static float distanceToSegment(const Position &s1, const Position &s2, const Position &point);
    static Position rotatePoint(Position point, float angle);
    static void setConstants(Constants *constants);
    static float getAngle(const Position &a, const Position &b);
private:
    static Constants *_constants;
    static Constants* getConstants();
};

#endif // UTILS_H
