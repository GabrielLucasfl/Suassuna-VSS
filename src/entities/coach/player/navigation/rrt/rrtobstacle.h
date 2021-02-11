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

#ifndef RRTOBSTACLE_H
#define RRTOBSTACLE_H

#include <src/utils/types/position/position.h>
#include <src/utils/utils.h>

class RRTObstacle {
public:
    RRTObstacle(const Position &pos, float radius);
    ~RRTObstacle();

    void setPos(const Position &pos);
    void setPos(float x, float y);
    void setRadius(float radius);

    Position getPos() const;
    float getRadius() const;
private:
    Position _pos;
    float _radius;
};

#endif // RRTOBSTACLE_H
