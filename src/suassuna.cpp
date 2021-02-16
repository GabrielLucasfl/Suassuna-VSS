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

#include "suassuna.h"

#include <src/entities/coach/player/player.h>
#include <src/entities/coach/role/default/role_default.h>
#include <src/utils/types/field/vss/field_vssb.h>
#include <src/entities/coach/coordinator/ssl/coordinator_vss.h>

Suassuna::Suassuna(Constants *constants) {
    // Setting up constants
    _constants = constants;

    // Creating world
    _world = new World(getConstants());

    // Register meta types
    qRegisterMetaType<Object>("Object");
    qRegisterMetaType<Position>("Position");
    qRegisterMetaType<Angle>("Angle");
    qRegisterMetaType<Colors::Color>("Colors::Color");
    qRegisterMetaType<VSSRef::Foul>("VSSRef::Foul");
    qRegisterMetaType<VSSRef::Color>("VSSRef::Color");
    qRegisterMetaType<VSSRef::Quadrant>("VSSRef::Quadrant");
}

void Suassuna::start() {
    // Creating World Map (set here the map that u desire)
    _worldMap = new WorldMap(getConstants(), getConstants()->teamSide(), new Field_VSSB());

    // Creating referee
    _referee = new Referee(getConstants(), getWorldMap());
    _world->addEntity(getReferee(), 0);

    // Creating and adding vision to world
    _vision = new Vision(getConstants());
    _world->addEntity(getVision(), 0);

    // Vision-WorldMap connection
    QObject::connect(getVision(), SIGNAL(sendPlayer(Colors::Color, quint8, Object)), getWorldMap(), SLOT(updatePlayer(Colors::Color, quint8, Object)), Qt::DirectConnection);
    QObject::connect(getVision(), SIGNAL(sendBall(Object)), getWorldMap(), SLOT(updateBall(Object)), Qt::DirectConnection);
    QObject::connect(getVision(), SIGNAL(sendGeometryData(fira_message::Field)), getWorldMap(), SLOT(updateGeometry(fira_message::Field)), Qt::DirectConnection);

    // Creating and adding actuator to world
    _actuator = new SimActuator(getConstants());
    _world->addEntity(getActuator(), 1); // needs to be an higher priority than the last player priority

    // Adding players
    for(int i = 0; i < getConstants()->qtPlayers(); i++) {
        Player *player = new Player(i, getConstants(), _referee, _worldMap);
        QObject::connect(player, SIGNAL(setLinearSpeed(quint8, float)), getActuator(), SLOT(setLinearSpeed(quint8, float)));
        QObject::connect(player, SIGNAL(setAngularSpeed(quint8, float)), getActuator(), SLOT(setAngularSpeed(quint8, float)));

        QObject::connect(player, SIGNAL(sendPlacement(quint8, Position, Angle)), getReferee(), SLOT(receivePlacement(quint8, Position, Angle)), Qt::DirectConnection);
        QObject::connect(getReferee(), SIGNAL(sendFoul(VSSRef::Foul, VSSRef::Color, VSSRef::Quadrant)), player, SLOT(receiveFoul(VSSRef::Foul, VSSRef::Color, VSSRef::Quadrant)), Qt::DirectConnection);

        _worldMap->addPlayer(i, player);
        _world->addEntity(player, 2);
    }

    // Creating coach
    _coach = new Coach(getConstants(), _referee, _worldMap);
    _world->addEntity(getCoach(), 3);

    // Setting coordinator to coach
    _coach->setCoordinator(new Coordinator_VSS());

    // Starting world
    _world->start();

    // Disabling world thread (loop() won't run anymore)
    _world->disableLoop();
}

void Suassuna::stop() {
    // Stopping and waiting world
    _world->stopEntity();
    _world->wait();

    // Deleting world (it also delete all other entities added to it)
    delete _world;

    // Delete worldmap
    delete _worldMap;
}

Constants* Suassuna::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Constants with nullptr value at Suassuna") + '\n';
    }
    else {
        return _constants;
    }

    return nullptr;
}

Vision* Suassuna::getVision() {
    if(_vision == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Vision with nullptr value at Suassuna") + '\n';
    }
    else {
        return _vision;
    }

    return nullptr;
}

Actuator* Suassuna::getActuator() {
    if(_actuator == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Actuator with nullptr value at Suassuna") + '\n';
    }
    else {
        return _actuator;
    }

    return nullptr;
}

World* Suassuna::getWorld() {
    if(_world == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("World with nullptr value at Suassuna") + '\n';
    }
    else {
        return _world;
    }

    return nullptr;
}

Referee* Suassuna::getReferee() {
    if(_referee == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Referee with nullptr value at Suassuna") + '\n';
    }
    else {
        return _referee;
    }

    return nullptr;
}

Coach* Suassuna::getCoach() {
    if(_coach == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Coach with nullptr value at Suassuna") + '\n';
    }
    else {
        return _coach;
    }

    return nullptr;
}

WorldMap* Suassuna::getWorldMap() {
    if(_worldMap == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("WorldMap with nullptr value at Suassuna") + '\n';
    }
    else {
        return _worldMap;
    }

    return nullptr;
}
