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

#include "behavior.h"

#include <src/entities/coach/player/player.h>
#include <src/entities/coach/skill/skill.h>

Behavior::Behavior() {
    _player = nullptr;
    _constants = nullptr;
    _actualSkill = nullptr;
    _initialized = false;
}

Behavior::~Behavior() {
    // Deleting skills
    QList<Skill*> skillList = _skillList.values();
    QList<Skill*>::iterator it;

    for(it = skillList.begin(); it != skillList.end(); it++) {
        delete *it;
    }

    // Cleaning map
    _skillList.clear();
}

bool Behavior::isInitialized() {
    return _initialized;
}

void Behavior::initialize(Constants *constants, WorldMap *worldMap) {
    _constants = constants;
    _worldMap = worldMap;
    configure();
    _initialized = true;
}

void Behavior::setPlayer(Player *player) {
    _player = player;
}

QString Behavior::actualSkillName() {
    _skillMutex.lock();
    QString skillName = (_actualSkill == nullptr) ? "NONE" : _actualSkill->name();
    _skillMutex.unlock();

    return skillName;
}

void Behavior::runBehavior() {
    // Check if behavior have at least one skill
    if(_skillList.size() == 0) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold(this->name().toStdString() + ":Has no skills set!.") + '\n';
        return ;
    }

    // Run behavior (implemented by child inherited method)
    run();
}

void Behavior::addSkill(int id, Skill *skill) {
    if(_skillList.contains(id)) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold(this->name().toStdString() + ":Trying to add a skill with an id already associated.") + '\n';
        return ;
    }

    // If is the first skill added, set it as actualSkill
    _skillMutex.lock();
    if(_actualSkill == nullptr) {
        _actualSkill = skill;
    }
    _skillMutex.unlock();

    _skillList.insert(id, skill);
}

void Behavior::setSkill(int id) {
    if(!_skillList.contains(id)) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold(this->name().toStdString() + ":Trying to set an skill that has not added previously.") + '\n';
        return ;
    }

    _skillMutex.lock();
    _actualSkill = _skillList.value(id);
    _skillMutex.unlock();

    // Check if initialized
    if(!_actualSkill->isInitialized()) {
        _actualSkill->initialize(getConstants());
    }

    // Run skill
    _actualSkill->setPlayer(player());
    _actualSkill->runSkill();
}

Player* Behavior::player() {
    return _player;
}

Constants* Behavior::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Constants with nullptr value at " + this->name().toStdString()) + '\n';
    }
    else {
        return _constants;
    }

    return nullptr;
}

WorldMap* Behavior::getWorldMap() {
    if(_worldMap == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("WorldMap with nullptr value at " + this->name().toStdString()) + '\n';
    }
    else {
        return _worldMap;
    }

    return nullptr;
}
