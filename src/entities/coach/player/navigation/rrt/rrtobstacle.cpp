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

#include "rrtobstacle.h"

RRTObstacle::RRTObstacle(const Position &pos, float radius) {
    _pos = pos;
    _radius = radius;
}

RRTObstacle::~RRTObstacle() {
}

void RRTObstacle::setPos(const Position &pos) {
    _pos = pos;
}

void RRTObstacle::setPos(float x, float y) {
    _pos.setPosition(true, x, y);
}

void RRTObstacle::setRadius(float radius) {
    _radius = radius;
}

Position RRTObstacle::getPos() const {
    return _pos;
}

float RRTObstacle::getRadius() const {
    return _radius;
}
