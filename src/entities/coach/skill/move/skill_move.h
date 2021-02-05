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

#ifndef SKILL_MOVE_H
#define SKILL_MOVE_H

#include <src/entities/coach/skill/skill.h>

class Skill_Move : public Skill
{
public:
    Skill_Move();
    QString name();

    void setMovementSpeed (float linearSpeed, float angularSpeed) { _linearSpeed = linearSpeed; _angulrSpeed = angularSpeed; }

private:
    // Skill inherited methods
    void configure();
    void run();

    // Parameters
    float _linearSpeed;
    float _angulrSpeed;
};

#endif // SKILL_MOVE_H