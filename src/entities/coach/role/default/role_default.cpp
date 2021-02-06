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

#include "role_default.h"

Role_Default::Role_Default() {
}

QString Role_Default::name() {
    return "Role_Default";
}

void Role_Default::configure() {
    // Starting behaviors
    _bhv_moveTo = new Behavior_MoveTo();
    _bhv_intercept = new Behavior_Intercept();

    // Adding behaviors to behaviors list
    addBehavior(BHV_MOVETO, _bhv_moveTo);
    addBehavior(BHV_INTERCEPT, _bhv_intercept);
}

void Role_Default::run() {
    _bhv_moveTo->setTargetPosition(player()->position());

    //_behavior_moveTo->setMinimalVelocity();
    //_bhv_intercept->setInterceptSegment(Position(true, 0.6f, 0.35f), Position(true, 0.6f, -0.35f));
    //_bhv_intercept->setObjectPosition(player()->getWorldMap()->getBall().getPosition());
    //_bhv_intercept->setObjectVelocity(player()->getWorldMap()->getBall().getVelocity());

    setBehavior(BHV_MOVETO);
}

