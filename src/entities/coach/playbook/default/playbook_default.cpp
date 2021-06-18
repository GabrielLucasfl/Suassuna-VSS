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
    //_rl_tmp = nullptr;
    _rl_sup = nullptr;
    _rl_atk = nullptr;

    _switchedPlayers = true;
    _replacedSecRole = true;
    _atkStuck = false;
    _defenderState = true;
}

QString Playbook_Default::name() {
    return "Playbook_Default";
}

void Playbook_Default::configure(int numPlayers) {
    // For each player, register an role and call usesRole() to register it in the table
    _rl_default = new Role_Default();
    _rl_gk = new Role_Goalkeeper();
    _rl_df = new Role_Defender();
    _rl_sup = new Role_Supporter();
    _rl_atk = new Role_Attacker();

    usesRole(_rl_default);
    usesRole(_rl_gk);
    usesRole(_rl_df);
    usesRole(_rl_sup);
    usesRole(_rl_atk);
}

void Playbook_Default::run(int numPlayers) {
    // Defining robot IDs
        if(_first){
            _switchPlayersTimer.start();
            _replaceSecRoleTimer.start();
            selectInitialIDs();
            _first = false;//this if is done only one time
        }
        _switchPlayersTimer.stop();
        if(_switchedPlayers && _switchPlayersTimer.getSeconds() > 2) {
            _switchedPlayers = false;
        }

        _replaceSecRoleTimer.stop();
        if(_replacedSecRole && _replaceSecRoleTimer.getSeconds() > 2) {
            _replacedSecRole = false;
        }

        switchPlayersIDs();
        thirdPlayerState();

        // Setting roles
        setPlayerRole(_goalkeeperID, _rl_gk);
        setPlayerRole(_attackerID, _rl_atk);
        if (_defenderState) {
            setPlayerRole(_lastID, _rl_df);
        } else {
            setPlayerRole(_lastID, _rl_sup);
        }
}

void Playbook_Default::switchPlayersIDs() {
    Position ballPos = getWorldMap()->getBall().getPosition();
    float ballVel = getWorldMap()->getBall().getVelocity().abs();
    Colors::Color ourColor = getConstants()->teamColor();
    float attackerVel = getWorldMap()->getPlayer(ourColor, _attackerID).getVelocity().abs();
    // If our attacker has stopped (possibly stuck)
    if(attackerVel <= 0.02f && !(ballVel <= 0.02f)) {
        // If it already was stuck
        if(_atkStuck) {
            _atkStuckTimer.stop();
            // If it is stuck for more than 2 seconds: switch players if we can
            if(_atkStuckTimer.getSeconds() > 2) {
                if(!_switchedPlayers) {
                    quint8 attId = _attackerID;
                    _attackerID = _lastID;
                    _lastID = attId;
                    _switchedPlayers = true;
                    _switchPlayersTimer.start();
                }
            }
        }
        // If it wasn't stuck: start the timer!
        else {
            _atkStuck = true;
            _atkStuckTimer.start();
        }
    }else {
        _atkStuck = false;
    }

    Position attackerPos = getWorldMap()->getPlayer(ourColor, _attackerID).getPosition();
    Position thirdPlayerPos = getWorldMap()->getPlayer(ourColor, _lastID).getPosition();
    // If our attacker isn't behind ball, but our third player is (and we can swicth them now): switch them!!!
    // OR
    // If both players are behind ball, but the thirdPlayer is closer to it (and we can switch them now): switch them!!!
    // OR
    // If the third player is closer to ball: switch them!!!
    if((!isBehindBallXcoord(attackerPos) && (isBehindBallXcoord(thirdPlayerPos)))
        || (Utils::distance(attackerPos, ballPos) > Utils::distance(thirdPlayerPos, ballPos))) {
        _atkStuckTimer.stop();
        if(!_switchedPlayers && !_atkStuck) {
            quint8 attId = _attackerID;
            _attackerID = _lastID;
            _lastID = attId;
            _switchedPlayers = true;
            _switchPlayersTimer.start();
        }
    }
}

void Playbook_Default::selectInitialIDs() {
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
    _lastID = players[1];
    _attackerID = players[2];
}

void Playbook_Default::thirdPlayerState() {
    Position selfPosition = getWorldMap()->getPlayer(getConstants()->teamColor(), _lastID).getPosition();
    if(!_switchedPlayers && !_replacedSecRole) {
        if (_defenderState) {
            if ((((getWorldMap()->getLocations()->ourSide().isRight() && getWorldMap()->getBall().getVelocity().vx() < 0.0f)
                    || (getWorldMap()->getLocations()->ourSide().isLeft() && getWorldMap()->getBall().getVelocity().vx() > 0.0f))
                    && getWorldMap()->getLocations()->isInsideOurField(getWorldMap()->getBall().getPosition()))
                    || (getWorldMap()->getLocations()->isInsideTheirField(selfPosition) && _switchedPlayers)) {
                _defenderState = false;
                _replacedSecRole = true;
                _replaceSecRoleTimer.start();
            }
        } else {
            Position atkPosition = getWorldMap()->getPlayer(getConstants()->teamColor(), _attackerID).getPosition();
            if (_atkStuck || (((Utils::distance(atkPosition, getWorldMap()->getBall().getPosition()) > 0.3f
                    && isBehindBallXcoord(atkPosition)) || !isBehindBallXcoord(atkPosition))
                    && ((getWorldMap()->getLocations()->ourSide().isRight() && getWorldMap()->getBall().getVelocity().vx() > 0.0f)
                    || (getWorldMap()->getLocations()->ourSide().isLeft() && getWorldMap()->getBall().getVelocity().vx() < 0.0f)))
                    || (getWorldMap()->getLocations()->isInsideOurField(selfPosition) && _switchedPlayers)) {
                _defenderState = true;
                _replacedSecRole = true;
                _replaceSecRoleTimer.start();
            }
        }
    }
}

bool Playbook_Default::isBehindBallXcoord(Position pos) {
    Position posBall = getWorldMap()->getBall().getPosition();
    float robotRadius = 0.08f;
    float ballRadius = 0.0215f;
    bool isBehindObjX;
    if(getWorldMap()->getLocations()->ourSide().isLeft()) {
        isBehindObjX = pos.x() < (posBall.x() + robotRadius + ballRadius);
    }else {
        isBehindObjX = pos.x() > (posBall.x() - robotRadius - ballRadius);
    }
    return isBehindObjX;
}
