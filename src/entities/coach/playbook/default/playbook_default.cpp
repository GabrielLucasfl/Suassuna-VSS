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

#include "playbook_default.h"

Playbook_Default::Playbook_Default() {
    _rl_default = nullptr;
    _rl_gk = nullptr;
    _rl_df = nullptr;
    _rl_sup = nullptr;
    _rl_atk = nullptr;

    _attackerID = 0;
    _goalkeeperID = 1;
    _lastID = 2;
}

QString Playbook_Default::name() {
    return "Playbook_Default";
}

void Playbook_Default::configure(int numPlayers) {
    // For each player, register an role and call usesRole() to register it in the table
    usesRole(_rl_default = new Role_Default());
    usesRole(_rl_gk = new Role_Goalkeeper());
    usesRole(_rl_df = new Role_Defender());
    usesRole(_rl_sup = new Role_Supporter());
    usesRole(_rl_atk = new Role_Attacker());

    setPlayerRole(_attackerID, _rl_atk);
    setPlayerRole(_goalkeeperID, _rl_df);
    setPlayerRole(_lastID, _rl_gk);
}

void Playbook_Default::run(int numPlayers) {
    // Defining robot IDs
    selectGoalkeeperID();
    selectAttackerID();

    // Setting roles
    setPlayerRole(_goalkeeperID, _rl_gk);
    setPlayerRole(_attackerID, _rl_atk);
    if (isDefenderSituation()) {
        setPlayerRole(_lastID, _rl_df);
    } else {
        setPlayerRole(_lastID, _rl_sup);
    }
}

void Playbook_Default::selectAttackerID() {
    QList<QPair<quint8, float>> playerData;
    bool hasSomeoneBehindBall = false;

    // Generating list of possible goalkeepers
    QList<quint8> players = getPlayers();
    for (int i = 0; i < players.size(); i++) {
        Position playerPosition = getWorldMap()->getPlayer(getConstants()->teamColor(), players[i]).getPosition();
        float ballDistance = Utils::distance(playerPosition, getWorldMap()->getBall().getPosition());
        if (players[i] != _goalkeeperID) {
            playerData.push_back(QPair<quint8, float>(players[i], ballDistance));
            if (isBehindBallXcoord(playerPosition)) {
                hasSomeoneBehindBall = true;
            }
        }
    }

    // Ordering the best goalkeepers
    for (int i = 0; i < playerData.size() - 1; i++) {
        for (int j = i; j < playerData.size(); j++) {
            if (playerData[i].second > playerData[j].second) {
                playerData.swap(i,j);
            }
        }
    }

    Colors::Color enemyColor;
    if (getConstants()->teamColor() == Colors::BLUE) {
        enemyColor = Colors::YELLOW;
    } else {
        enemyColor = Colors::BLUE;
    }
    bool isEnemyNearBall = false;

    // Generating enemy list
    QList<quint8> enemyPlayers = getWorldMap()->getAvailablePlayers(enemyColor);
    for (int i = 0; i < enemyPlayers.size(); i++) {
        Position enemyPlayerPosition = getWorldMap()->getPlayer(enemyColor, enemyPlayers[i]).getPosition();
        float enemyBalDistance = Utils::distance(enemyPlayerPosition, getWorldMap()->getBall().getPosition());
        if (playerData[0].second > enemyBalDistance) {
            isEnemyNearBall = true;
            break;
        }
    }

    float ballDistanceToOurGoal = Utils::distance(getWorldMap()->getBall().getPosition(), getWorldMap()->getLocations()->ourGoal());
    float ballDistanceToTheirGoal = Utils::distance(getWorldMap()->getBall().getPosition(), getWorldMap()->getLocations()->theirGoal());
    if (ballDistanceToOurGoal < ballDistanceToTheirGoal) {
        if (isEnemyNearBall) {
            if (hasSomeoneBehindBall) {
                if (isBehindBallXcoord(getWorldMap()->getPlayer(getConstants()->teamColor(), playerData[0].first).getPosition())) {
                    _lastID = playerData[0].first;
                    _attackerID = playerData[1].first;
                } else {
                    _lastID = playerData[1].first;
                    _attackerID = playerData[0].first;
                }
            } else {
                float goalDistanceZero = Utils::distance(getWorldMap()->getLocations()->ourGoal(),
                            getWorldMap()->getPlayer(getConstants()->teamColor(), playerData[0].first).getPosition());
                float goalDistanceOne = Utils::distance(getWorldMap()->getLocations()->ourGoal(),
                            getWorldMap()->getPlayer(getConstants()->teamColor(), playerData[1].first).getPosition());
                if (goalDistanceZero < goalDistanceOne) {
                    _lastID = playerData[0].first;
                    _attackerID = playerData[1].first;
                } else {
                    _lastID = playerData[1].first;
                    _attackerID = playerData[0].first;
                }
            }
        } else {
            if (hasSomeoneBehindBall) {
                _attackerID = playerData[0].first;
                _lastID = playerData[1].first;
            } else {
                _lastID = playerData[1].first;
                _attackerID = playerData[0].first;
            }
        }
    } else {
        if (hasSomeoneBehindBall) {
            if (isBehindBallXcoord(getWorldMap()->getPlayer(getConstants()->teamColor(), playerData[0].first).getPosition())) {
                _attackerID = playerData[0].first;
                _lastID = playerData[1].first;
            } else {
                _attackerID = playerData[1].first;
                _lastID = playerData[0].first;
            }
        } else {
            _lastID = playerData[0].first;
            _attackerID = playerData[1].first;
        }
    }
}

void Playbook_Default::selectGoalkeeperID() {
    QList<quint8> players = getPlayers();
    QList<float> goalDistance;

    // Generating list of possible goalkeepers
    for (int i = 0; i < players.size(); i++) {
        Position playerPosition = getWorldMap()->getPlayer(getConstants()->teamColor(), players[i]).getPosition();
        goalDistance.push_back(Utils::distance(playerPosition, getWorldMap()->getLocations()->ourGoal()));
    }

    // Ordering the best goalkeepers
    for (int i = 0; i < goalDistance.size() - 1; i++) {
        for (int j = i; j < goalDistance.size(); j++) {
            if (goalDistance[i] > goalDistance[j]) {
                goalDistance.swap(i,j);
                players.swap(i,j);
            }
        }
    }

    _goalkeeperID = players[0];
}

bool Playbook_Default::isDefenderSituation() {
    Colors::Color enemyColor;
    if (getConstants()->teamColor() == Colors::BLUE) {
        enemyColor = Colors::YELLOW;
    } else {
        enemyColor = Colors::BLUE;
    }
    bool result = false;

    // Defining Defender situation
    QList<quint8> enemyPlayers = getWorldMap()->getAvailablePlayers(enemyColor);
    for (int i = 0; i < enemyPlayers.size(); i++) {
        Position enemyPlayerPosition = getWorldMap()->getPlayer(enemyColor, enemyPlayers[i]).getPosition();
        float enemyAngleToOurGoal = Utils::getAngle(getWorldMap()->getLocations()->ourGoal(), enemyPlayerPosition);
        float enemyAngleToBall = Utils::getAngle(getWorldMap()->getBall().getPosition(), enemyPlayerPosition);
        float enemyOrientation = getWorldMap()->getPlayer(enemyColor, enemyPlayers[i]).getOrientation().value();
        float enemyBalDistance = Utils::distance(enemyPlayerPosition, getWorldMap()->getLocations()->ourGoal());
        if ((enemyBalDistance < 0.07f && abs(enemyAngleToBall - enemyAngleToOurGoal) < static_cast<float>(M_PI) / 6)
                && abs(enemyAngleToOurGoal - enemyOrientation) < static_cast<float>(M_PI) / 12) {
            result = true;
        } else {
            result = false;
        }
    }
    return result;
}

bool Playbook_Default::isBehindBallXcoord(Position pos) {
    Position posBall = getWorldMap()->getBall().getPosition();
    float robotRadius = 0.035f;
    bool isBehindObjX;
    if(getWorldMap()->getLocations()->ourSide().isLeft()) {
        isBehindObjX = pos.x() < (posBall.x() - robotRadius);
    }else {
        isBehindObjX = pos.x() > (posBall.x() + robotRadius);
    }
    return isBehindObjX;
}
