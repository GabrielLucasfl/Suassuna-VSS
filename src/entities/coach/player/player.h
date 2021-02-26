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
    float getPlayerDistanceTo(Position &targetPosition);
    float getPlayerRotateAngleTo(Position &targetPosition);
    float getVxToTarget(Position targetPosition);
    float getRotateSpeed(float angleRobotToTarget);

    // Utils
    std::pair<float, float> getWheelsSpeed(float angleToObject, float baseSpeed);
    float to180Range(float angle);
    float smallestAngleDiff(float target, float source);

    // Player checkers
    bool isLookingTo(Position &pos, float error);

    // Player error
    float getLinearError();
    float getAngularError();

    // Role management
    void setRole(Role* role);

    // Skills
    void goTo(Position &targetPosition, float desiredBaseSpeed, float desiredLinearError, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea , bool avoidTheirGoalArea);
    void rotateTo(Position &targetPosition);
    void spin(bool isClockWise);
    void idle();

    // Path planning
    QLinkedList<Position> getPath() const;
    std::pair<Angle,float> getNavDirectionDistance(const Position &destination, const Angle &positionToLook, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea, bool avoidTheirGoalArea);
    Position limitPosInsideField(Position dest);

private:
    float _lastError;
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

    // Path Planning
    Navigation *_nav;
    Position projectPosOutsideGoalArea(Position targetPosition, bool avoidOurArea, bool avoidTheirArea);
    float _displacement;

signals:
    void setWheelsSpeed(quint8 playerId, float wheelLeft, float wheelRight);
    void sendPlacement(quint8 playerId, Position desiredPosition, Angle desiredOrientation);

public slots:
    void receiveFoul(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant);
};

#endif // PLAYER_H
