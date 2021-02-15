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

#ifndef PLAYER_H
#define PLAYER_H

#include <src/entities/entity.h>
#include <src/constants/constants.h>
#include <src/entities/coach/basecoach.h>
#include <src/entities/world/worldmap.h>
#include <include/vssref_common.pb.h>

class Player : public Entity
{
    Q_OBJECT
public:
    Player(quint8 playerId, Constants *constants, Referee *referee, WorldMap *worldMap);

    // Player internal getters
    quint8 playerId();
    Position position();
    Angle orientation();
    float getPlayerRotateAngleTo(Position &targetPosition);
    float getPlayerDistanceTo(Position &targetPosition);

    // Player checkers
    bool isLookingTo(Position &pos, float error);

    // Player error
    float getLinearError();
    float getAngularError();

    // Role management
    void setRole(Role *role);

    // Skills
    void goTo(Position &targetPosition, float minVel);
    void rotateTo(Position &targetPosition);
    void spin(bool isClockWise);
    void move(float linearSpeed, float angularSpeed);
    void idle();

private:
    // Entity inherited methods
    void initialization();
    void loop();
    void finalization();

    // Constants
    Constants* getConstants();
    Constants *_constants;

    // Worldmap
    WorldMap *_worldMap;
    WorldMap* getWorldMap();

    // Referee
    Referee *_referee;
    Referee* getReferee();

    // Player internal
    quint8 _playerId;

    // Role management
    Role *_playerRole;
    QMutex _mutexRole;

signals:
    void setLinearSpeed(int teamId, int playerId, float vx);
    void setAngularSpeed(int teamId, int playerId, float vw);
    void dribble(int teamId, int playerId, bool enable);
    void kick(int teamId, int playerId, float power);
    void chipKick(int teamId, int playerId, float power);signals:
    void sendPlacement(quint8 playerId, Position desiredPosition, Angle desiredOrientation);

public slots:
    void receiveFoul(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant);
};

#endif // PLAYER_H
