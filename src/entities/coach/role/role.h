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

#ifndef ROLE_H
#define ROLE_H

#include <QObject>

#include <src/entities/referee/referee.h>
#include <src/entities/coach/basecoach.h>
#include <src/constants/constants.h>
#include <src/entities/coach/behavior/moveto/behavior_moveto.h>

class Role : public QObject
{
    Q_OBJECT
public:
    Role();
    virtual ~Role();

    // Role name (for debug)
    virtual QString name() = 0;

    // Init role control
    bool isInitialized();
    void initialize(Constants *constants, Referee *referee, WorldMap *worldMap);
    void setPlayer(Player *player);

    // Actual Behavior name
    QString actualBehaviorName();

    // Method to run in playbook
    void runRole();

    // Placement
    virtual QPair<Position, Angle> getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant) = 0;

protected:
    // Role control methods
    void addBehavior(int id, Behavior *behavior);
    void setBehavior(int id);

    // Player, constants and referee getters
    Player* player();
    Constants* getConstants();
    WorldMap* getWorldMap();
    Referee* getReferee();

private:
    // Virtual implementation in inherited classes
    virtual void configure() = 0;
    virtual void run() = 0;

    // Player access
    Player *_player;
    QMutex _playerMutex;

    // Stucked management
    Behavior_MoveTo *_bh_stuckAvoid;
    Timer _stuckedTimer;
    bool _wasStucked;
    bool isStuckedAtWall();
    Position getProjectionInStuckedWall();

    // Constants
    Constants *_constants;

    // Referee
    Referee *_referee;

    // Worldmap
    WorldMap *_worldMap;

    // Behavior list
    QMap<int, Behavior*> _behaviorList;
    Behavior *_actualBehavior;
    QMutex _behaviorMutex;

    // Initialize control
    bool _initialized;
};

#endif // ROLE_H
