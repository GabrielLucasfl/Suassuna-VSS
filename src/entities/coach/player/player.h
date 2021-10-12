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
#include <src/entities/coach/player/pid/pid.h>

class NavigationAlgorithm;
class Navigation;

class Player : public Entity
{
    Q_OBJECT
public:
    Player(quint8 playerId, Constants *constants, Referee *referee, WorldMap *worldMap, NavigationAlgorithm *navAlg);

    // Player internal getters
    QString name();
    quint8 playerId();
    Position position();
    Angle orientation();
    Position getPlayerDesiredPosition();
    Position getAttackerReference();
    float getPlayerDistanceTo(Position &targetPosition);
    float getPlayerRotateAngleTo(Position &targetPosition);

    // Utils
    std::pair<float, float> getWheelsSpeed(float angleToObject, float baseSpeed);
    float getSmallestAngleDiff(float target, float source);

    // Player error
    float getLinearError();
    float getAngularError();

    // Player checkers
    bool isLookingTo(Position &pos, float error);
    bool isBehindBallXCoord (Position playerPosition, float inc = 0);

    // Player setters
    void setPlayerDesiredPosition(Position desiredPosition) { _desiredPosition = desiredPosition; }
    void setAttackerReference(Position desiredPosition) { _attackerReference = desiredPosition; }

    // Role management
    QString roleName();
    QString behaviorName();
    void setRole(Role* role);

    // Path Planning
    QLinkedList<Position> getPath() const;

    // Skills
    void goTo(float desiredBaseSpeed, float desiredLinearError, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea , bool avoidTheirGoalArea);
    void rotateTo();
    void spin(bool isClockWise);
    void idle();

    // Repositioners
    Position projectPosOutsideGoalArea(Position targetPosition, bool avoidOurArea, bool avoidTheirArea);
    Position limitPosInsideField(Position dest);

private:
    float _lastError;
    // Entity inherited methods
    void initialization();
    void loop();
    void finalization();

    // GUI Parameters
    Position _desiredPosition;
    Position _attackerReference;

    // Constants
    Constants* getConstants();
    Constants *_constants;

    // Worldmap
    WorldMap *_worldMap;
    WorldMap* getWorldMap();

    // Referee
    Referee *_referee;
    Referee* getReferee();

    // Path Planning
    Navigation *_nav;
    std::pair<Angle,float> getNavDirectionDistance(const Position &destination, const Angle &positionToLook, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea, bool avoidTheirGoalArea);

    // Player internal
    quint8 _playerId;

    // Role management
    Role *_playerRole;
    QMutex _mutexRole;

signals:
    void setWheelsSpeed(quint8 playerId, float wheelLeft, float wheelRight);
    void sendPlacement(quint8 playerId, Position desiredPosition, Angle desiredOrientation);

public slots:
    void receiveFoul(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant);
};

#endif // PLAYER_H
