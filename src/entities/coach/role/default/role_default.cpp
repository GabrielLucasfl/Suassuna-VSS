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
    Position playerPosition = player()->position();
    Position ballPosition(player()->getWorldMap()->getBall().getPosition());
    bool result = player()->getWorldMap()->getLocations()->isInsideOurArea(playerPosition);
    if (result) std::cout << "Sucesso\n";
    else std::cout << "Fracasso\n";
//    Position leftPost = player()->getWorldMap()->getLocations()->ourGoalLeftPost();
//    std::cout << "Coordenada x do poste esquerdo: " << leftPost.x() << "\n";
//    std::cout << "Coordenada y do poste esquerdo: " << leftPost.y() << "\n";
//    std::cout << "\n";
//    Position rightPost = player()->getWorldMap()->getLocations()->ourGoalRightPost();
//    std::cout << "Coordenada x do poste direito: " << rightPost.x() << "\n";
//    std::cout << "Coordenada y do poste direito: " << rightPost.y() << "\n";
//    std::cout << "\n";
//    Position leftPost2 = player()->getWorldMap()->getLocations()->theirGoalLeftPost();
//    std::cout << "Coordenada x do poste esquerdo: " << leftPost2.x() << "\n";
//    std::cout << "Coordenada y do poste esquerdo: " << leftPost2.y() << "\n";
//    std::cout << "\n";
//    Position rightPost2 = player()->getWorldMap()->getLocations()->theirGoalRightPost();
//    std::cout << "Coordenada x do poste direito: " << rightPost2.x() << "\n";
//    std::cout << "Coordenada y do poste direito: " << rightPost2.y() << "\n";
}

QPair<Position, Angle> Role_Default::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) {
    //// TODO: Configure this in new roles!!!!!!!!!!!!!!!!!!!!!!!!!
    return QPair<Position, Angle>(Position(true, 0.0, 0.0), Angle(true, 0.0));
}
