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

    // Alloc robotData
    _robotData = (robotData **) malloc(QT_TEAMS * sizeof(robotData *));
    for(int i = 0; i < QT_TEAMS; i++) {
        _robotData[i] = (robotData *) malloc(getConstants()->qtPlayers() * sizeof(robotData));
    }

    for(int i = 0; i < QT_TEAMS; i++) {
        for(int j = 0; j < getConstants()->qtPlayers(); j++) {
            _robotData[i][j].isYellow = (i == 0) ? false : true;
            _robotData[i][j].playerId = j;
            _robotData[i][j].vx = 0.0f;
            _robotData[i][j].vw = 0.0f;
            _robotData[i][j].kickPower = 0.0f;
            _robotData[i][j].dribbling = false;
            _robotData[i][j].isUpdated = true;
        }
    }
}

Actuator::~Actuator() {
    for(int i = 0; i < QT_TEAMS; i++) {
        free(_robotData[i]);
    }

    free(_robotData);
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
