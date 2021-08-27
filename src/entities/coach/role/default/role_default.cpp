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
    _state = STAYING;
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
    Position base(true, getWorldMap()->getLocations()->theirGoal().x(), player()->position().y());
    Position stop(true, 0.5f, player()->position().y());
    float playerVelocity = getWorldMap()->getPlayer(getConstants()->teamColor(), player()->playerId()).getVelocity().abs();
    _bhv_moveTo->setBaseSpeed((6 / 6) * getConstants()->playerBaseSpeed());

    switch(_state) {
    case ACCELERATING: {
        std::cout << "Player velocity: " << playerVelocity << std::endl;
        player()->setPlayerDesiredPosition(base);
        if (player()->position().x() < 0.0f) {
            _timer.stop();
            std::cout << "\nTime: " << _timer.getSeconds() << std::endl;
            _state = BREAKING;
            std::cout << "\nSTOP\n\n";
        }
        break;
    }
    case BREAKING: {
        std::cout << "Player velocity: " << playerVelocity << std::endl;
        player()->setPlayerDesiredPosition(stop);
        if (playerVelocity < 0.05f) {
            _state = STAYING;
        }
        break;
    }
    case STAYING: {
        _timer.stop();
        std::cout << "\nTime: " << _timer.getSeconds() << std::endl;
        player()->setPlayerDesiredPosition(player()->position());
        if (player()->position().x() > 0.6f) {
            _state = ACCELERATING;
            std::cout << "\nSTART\n\n";
            _timer.start();
        }
        break;
    }
    }
    setBehavior(BHV_MOVETO);
}

QPair<Position, Angle> Role_Default::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    //// TODO: Configure this in new roles!!!!!!!!!!!!!!!!!!!!!!!!!
    return QPair<Position, Angle>(Position(true, 0.0, 0.0), Angle(true, 0.0));
}
