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

#include "role.h"

#include <src/entities/coach/player/player.h>
#include <src/entities/coach/behavior/behavior.h>

Role::Role() {
    _player = nullptr;
    _constants = nullptr;
    _actualBehavior = nullptr;
    _initialized = false;
    _wasStucked = false;
}

Role::~Role() {
    // Deleting skills
    QList<Behavior*> behaviorList = _behaviorList.values();
    QList<Behavior*>::iterator it;

    for(it = behaviorList.begin(); it != behaviorList.end(); it++) {
        delete *it;
    }

    // Delete behavior stuck
    delete _bh_stuckAvoid;

    // Cleaning map
    _behaviorList.clear();
}

bool Role::isInitialized() {
    return _initialized;
}

void Role::initialize(Constants *constants, Referee *referee, WorldMap *worldMap) {
    // Take pointers
    _constants = constants;
    _referee = referee;
    _worldMap = worldMap;

    // Create moveto behavior to stucked control
    _bh_stuckAvoid = new Behavior_MoveTo();

    // Call virtual configure()
    configure();

    // Mark as initialized
    _initialized = true;
}

void Role::setPlayer(Player *player) {
    _playerMutex.lock();
    _player = player;
    _playerMutex.unlock();
}

QString Role::actualBehaviorName() {
    _behaviorMutex.lock();
    QString behaviorName = (_actualBehavior == nullptr) ? "NONE" : _actualBehavior->name();
    _behaviorMutex.unlock();

    return behaviorName;
}

void Role::runRole() {
    // Check if behavior have at least one skill
    if(_behaviorList.size() == 0) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold(this->name().toStdString() + ":Has no behaviors set!.") + '\n';
        return ;
    }

    // TODO: check if this is the cause
    if(_player == nullptr) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold(this->name().toStdString() + ":Player is null!.") + '\n';
        return ;
    }

    // Run behavior (implemented by child inherited method)
    if(getReferee()->isGameOn()) {
        if((!isStuckedAtWall() || name() == "Role_Goalkeeper") && !_wasStucked) {
            run();

            // Check if initialized
            if(!_actualBehavior->isInitialized()) {
                _actualBehavior->initialize(getConstants(), getWorldMap());
            }

            // Run skill
            _actualBehavior->setPlayer(player());
            _actualBehavior->runBehavior();

            // Reset stucked timer
            _stuckedTimer.start();
        }
        else {
            if(_wasStucked) {
                // Stop stucked timer
                _stuckedTimer.stop();

                if(_stuckedTimer.getSeconds() >= getConstants()->timeToWaitStuckMovement()) {
                    _wasStucked = false;
                }
                else {
                    _bh_stuckAvoid->runBehavior();
                }
            }
            else {
                // Stop stucked timer
                _stuckedTimer.stop();

                if(_stuckedTimer.getSeconds() >= getConstants()->timeToConsiderStuck()) {
                    // Check if stucked bhv is initialized
                    if(!_bh_stuckAvoid->isInitialized()) {
                        _bh_stuckAvoid->initialize(getConstants(), getWorldMap());
                    }

                    // Set desired position
                    Position targetPosition = Utils::threePoints(getProjectionInStuckedWall(), player()->position(), 0.3f, 0.0);
                    _bh_stuckAvoid->setTargetPosition(targetPosition);

                    // Set player and run
                    _bh_stuckAvoid->setPlayer(player());
                    _bh_stuckAvoid->runBehavior();

                    // Mark as stucked
                    _wasStucked = true;
                    _stuckedTimer.start();
                }
            }
        }
    }
    else {
        if(player() != nullptr) {
            player()->idle();
        }
    }
}

void Role::addBehavior(int id, Behavior *behavior) {
    if(_behaviorList.contains(id)) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold(this->name().toStdString() + ":Trying to add a behavior with an id already associated.") + '\n';
        return ;
    }

    // If is the first skill added, set it as actualSkill
    _behaviorMutex.lock();
    if(_actualBehavior == nullptr) {
        _actualBehavior = behavior;
    }
    _behaviorMutex.unlock();

    _behaviorList.insert(id, behavior);
}

bool Role::isStuckedAtWall() {
    Position projectedPosition = getProjectionInStuckedWall();

    return !projectedPosition.isInvalid();
}

Position Role::getProjectionInStuckedWall() {
    QList<Wall> walls = getWorldMap()->getLocations()->getWalls();

    Position projection = Position(false, 0.0, 0.0);
    float playerSpeed = getWorldMap()->getPlayer(getConstants()->teamColor(), player()->playerId()).getVelocity().abs();
    for(int i = 0; i < walls.size(); i++) {
        Wall actualWall = walls.at(i);
        if(actualWall.getDistanceToWall(player()->position()) <= getConstants()->distToConsiderStuck() && playerSpeed <= player()->getLinearError()) {
            projection = actualWall.getProjectionAtWall(player()->position());
            break;
        }
    }

    return projection;
}

void Role::setBehavior(int id) {
    if(!_behaviorList.contains(id)) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold(this->name().toStdString() + ":Trying to set an behavior that has not added previously.") + '\n';
        return ;
    }

    _behaviorMutex.lock();
    _actualBehavior = _behaviorList.value(id);
    _behaviorMutex.unlock();
}

Player* Role::player() {
    return _player;
}

Constants* Role::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold("Constants with nullptr value at " + this->name().toStdString()) + '\n';
    }
    else {
        return _constants;
    }

    return nullptr;
}

WorldMap* Role::getWorldMap() {
    if(_worldMap == nullptr) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold("WorldMap with nullptr value at " + this->name().toStdString()) + '\n';
    }
    else {
        return _worldMap;
    }

    return nullptr;
}

Referee* Role::getReferee() {
    if(_referee == nullptr) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold("Referee with nullptr value at " + this->name().toStdString()) + '\n';
    }
    else {
        return _referee;
    }

    return nullptr;
}
