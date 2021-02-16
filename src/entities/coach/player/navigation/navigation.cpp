/***
 * Warthog Robotics
 * University of Sao Paulo (USP) at Sao Carlos
 * http://www.warthog.sc.usp.br/
 *
 * This file is part of WRCoach project.
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

#include "navigation.h"
#include <src/entities/world/locations/locations.h>

Navigation::Navigation(Player *player, NavigationAlgorithm *navAlg, Constants *constants, WorldMap* worldMap) {
    _player = player;
    _navAlg = navAlg;
    _wm = worldMap;
    _loc = _wm->getLocations();
    _ourTeamColor = constants->teamColor();
    if(_ourTeamColor == Colors::YELLOW) {
        _theirTeamColor = Colors::BLUE;
    }else {
        _theirTeamColor = Colors::YELLOW;
    }

    if(_navAlg!=nullptr)
        _navAlg->initialize(_loc, constants);
}

Navigation::~Navigation() {
    if(_navAlg!=nullptr) {
        delete _navAlg;
    }
}

void Navigation::setGoal(const Position &destination, const Angle &orientation, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea, bool avoidTheirGoalArea) {
    QMutexLocker locker(&_navAlgMutex);

    // Reset algorithm
    _navAlg->reset();

    // Set origin position and orientation
    _navAlg->setOrigin(_player->position(), _player->orientation());

    // Sets goal position and orientation
    _navAlg->setGoal(destination, orientation);

    // Add ball as repulsive
    if(avoidBall)
        _navAlg->addBall(_wm->getBall().getPosition());

    // Add teammates as repulsive
    if(avoidTeammates) {
        QList<quint8> avPlayers = _wm->getAvailablePlayers(_ourTeamColor);
        for(int i=0; i<avPlayers.size(); i++) {
            Player* teammate = _wm->getPlayerPointer(avPlayers.at(i));
            if(teammate->playerId() == _player->playerId()) {
                continue;
            }
            // Add robot as repulsive
            _navAlg->addOwnRobot(teammate->position());
        }
    }

    // Add opponents as repulsive
    if(avoidOpponents) {
        QList<quint8> avPlayers = _wm->getAvailablePlayers(_theirTeamColor);
        for(int i=0; i<avPlayers.size(); i++) {
            Object opponent = _wm->getPlayer(_theirTeamColor, avPlayers.at(i));
            // Add robot as repulsive
            _navAlg->addEnemyRobot(opponent.getPosition());
        }
    }

    //Adds goal area as repulsive
    if(avoidOurGoalArea || avoidTheirGoalArea) {
        // Check restritions
        if(_loc->isOutsideField(_player->position()))
            return;
        if(_loc->isInsideOurArea(_player->position()))
            return;
        if(_loc->isInsideTheirArea(_player->position()))
            return;

        // Our goal area
        if(avoidOurGoalArea) {
            // Add our goal area
            Position far = Utils::projectPointAtSegment(_loc->ourGoalLeftPost(), _loc->ourGoalRightPost(), _player->position());
            Position repulsion = Utils::threePoints(far, _player->position(), _loc->fieldDefenseWidth(), 0.0f);
            _navAlg->addGoalArea(repulsion);
        }

        // Their goal area
        if(avoidTheirGoalArea) {
            // Add their goal area
            Position far = Utils::projectPointAtSegment(_loc->theirGoalLeftPost(), _loc->theirGoalRightPost(), _player->position());
            Position repulsion = Utils::threePoints(far, _player->position(), _loc->fieldDefenseWidth(), 0.0f);
            _navAlg->addGoalArea(repulsion);
        }
    }

}

Angle Navigation::getDirection() const {
    QMutexLocker locker(&_navAlgMutex);

    // Execute algorithm
    _navAlg->runNavigationAlgorithm();

    return _navAlg->getDirection();
}

float Navigation::getDistance() const {
    QMutexLocker locker(&_navAlgMutex);
    return _navAlg->getDistance();
}

QLinkedList<Position> Navigation::getPath() const {
    QMutexLocker locker(&_navAlgMutex);
    return _navAlg->getPath();
}

void Navigation::setNavigationAlgorithm(NavigationAlgorithm *navAlg) {
    QMutexLocker locker(&_navAlgMutex);
    delete _navAlg;
    _navAlg = navAlg;
}
