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

#include "actuator.h"

Actuator::Actuator(Constants *constants) : Entity(ENT_ACTUATOR) {
    // Set constants
    _constants = constants;

    // Set data as nullptr
    _robotsData = nullptr;

    // Init data
    initData();
}

Actuator::~Actuator() {
    deleteData();
}

void Actuator::initData() {
    // Call delete data
    deleteData();

    // Assign new data
    _robotsData = (robotData*) malloc(getConstants()->qtPlayers() * sizeof(robotData));

    // Set default values
    for(int i = 0; i < getConstants()->qtPlayers(); i++) {
        _robotsData[i].playerId = i;
        _robotsData[i].wheelLeft = 0.0f;
        _robotsData[i].wheelRight = 0.0f;
        _robotsData[i].isUpdated = true;
    }
}

void Actuator::deleteData() {
    // Free pointer
    if(_robotsData != nullptr) {
        free(_robotsData);
    }

    // Set as nullptr for convenience
    _robotsData = nullptr;
}

QString Actuator::name() {
    return "Actuator";
}

Constants* Actuator::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Constants with nullptr value at Actuator") + '\n';
    }
    else {
        return _constants;
    }

    return nullptr;
}
