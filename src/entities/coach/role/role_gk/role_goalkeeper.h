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

#ifndef ROLE_GOALKEEPER_H
#define ROLE_GOALKEEPER_H

#include <src/entities/coach/role/role.h>
#include <src/entities/coach/behavior/behaviors.h>

class Role_Goalkeeper : public Role
{
public:
    Role_Goalkeeper();
    QString name();

    // Setters
    void setDenfenderElipseParameters(float ellipseA, float ellipseB) { _defenderEllipseA = ellipseA; _defenderEllipseB = ellipseB; };
    void setDefenderEllipseCenter(Position ellipseCenter){ _defenderEllipseCenter = ellipseCenter; };

private:
    // Role inherited methods
    void configure();
    void run();

    // Behaviors enum
    enum {
        BHV_MOVETO,
        BHV_INTERCEPT
    };

    // Parameters
    bool _gkOverlap;
    Timer _overlapTimer;
    bool _isOverlapTimerInit;


    // Behaviors pointers
    Behavior_MoveTo *_bhv_moveTo;
    Behavior_Intercept *_bhv_intercept;

    // Role Parameters
    float _defenderEllipseA;
    float _defenderEllipseB;
    Position _defenderEllipseCenter;

    // Methods
    bool spinOrientarion();
    bool isBallInsideEllipse();

    // Replacer
    QPair<Position, Angle> getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant);
};

#endif // ROLE_GOALKEEPER_H
