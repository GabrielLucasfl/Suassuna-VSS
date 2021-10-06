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
    _defenderState = true;
}

QString Playbook_Default::name() {
    return "Default";
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

    updatePlayerStuck(_attackerID);
    updatePlayerStuck(_lastID);
    updatePlayerStuck(_goalkeeperID);
    //switchPlayersIDs();
    //thirdPlayerState();

    // Setting roles
    setPlayerRole(_goalkeeperID, _rl_gk);
    setPlayerRole(_attackerID, _rl_atk);
    if (_defenderState) {
        Position ellipseCenter = getWorldMap()->getLocations()->ourGoal();
        if(getWorldMap()->getLocations()->ourGoal().x() > 0){
            ellipseCenter.setPosition(true, ellipseCenter.x() - getConstants()->ellipseCenterOffset(), ellipseCenter.y());
        }
        else{
            ellipseCenter.setPosition(true, ellipseCenter.x() + getConstants()->ellipseCenterOffset(), ellipseCenter.y());
        }
        setDefenderEllipse(ellipseCenter, getConstants()->ellipseParameters());

        /*if (isBallInsideDefenderEllipse(0.07f, 0.43f)) {
            _rl_df->setElipseParameters(0.1f, 0.25f);
        } else {
            _rl_df->setElipseParameters(0.07f, 0.43f);
        }*/
        setPlayerRole(_lastID, _rl_df);
    } else {
        setPlayerRole(_lastID, _rl_sup);
    }
}

void Playbook_Default::setDefenderEllipse(Position center, std::pair<float, float> ellipseParameters){
    _rl_df->setEllipseCenter(center);
    _rl_df->setEllipseParameters(ellipseParameters);

    _rl_gk->setDefenderEllipseCenter(center);
    _rl_gk->setDenfenderEllipseParameters(ellipseParameters);
}

float Playbook_Default::minDistPlayerObstacle(quint8 id) {
    Colors::Color ourColor = getConstants()->teamColor();
    Colors::Color theirColor;
    if(ourColor == Colors::YELLOW) {
        theirColor = Colors::BLUE;
    }else {
        theirColor = Colors::YELLOW;
    }
    float minDist = 1000;
    QList<quint8> opPlayers = getWorldMap()->getAvailablePlayers(theirColor);
    for(int i=0; i<opPlayers.size(); i++) {
        Position thisPos = getWorldMap()->getPlayer(ourColor, id).getPosition();
        Position otherPos = getWorldMap()->getPlayer(ourColor, opPlayers[i]).getPosition();
        float dist = Utils::distance(thisPos, otherPos);
        if(dist < minDist) {
            minDist = dist;
        }
    }
    QList<quint8> allyPlayers = getWorldMap()->getAvailablePlayers(theirColor);
    for(int i=0; i<allyPlayers.size(); i++) {
        if(allyPlayers[i] != id) {
            Position thisPos = getWorldMap()->getPlayer(ourColor, id).getPosition();
            Position otherPos = getWorldMap()->getPlayer(theirColor, allyPlayers[i]).getPosition();
            float dist = Utils::distance(thisPos, otherPos);
            if(dist < minDist) {
                minDist = dist;
            }
        }
    }
    return minDist;
}

void Playbook_Default::updatePlayerStuck(quint8 id) {
    Colors::Color ourColor = getConstants()->teamColor();
    //float ballVel = getWorldMap()->getBall().getVelocity().abs();
    float playerVel = getWorldMap()->getPlayer(ourColor, id).getVelocity().abs();
    // If has stopped (possibly stuck)
    if(playerVel <= 0.02f && minDistPlayerObstacle(id) < 0.1f && getReferee()->isGameOn()) {
        if(!playersState[id]->stuckState) {
            playersState[id]->stuckState = true;
            playersState[id]->timer.start();
        }
    }else {
        playersState[id]->stuckState = false;
    }
}

void Playbook_Default::switchPlayersIDs() {
    if(playersState[_attackerID]->isStuck()) {
        if(!_switchedPlayers) {
            quint8 attId = _attackerID;
            _attackerID = _lastID;
            _lastID = attId;
            _switchedPlayers = true;
            _switchPlayersTimer.start();
        }
    }
    Position ballPos = getWorldMap()->getBall().getPosition();
    Colors::Color ourColor = getConstants()->teamColor();

    Position attackerPos = getWorldMap()->getPlayer(ourColor, _attackerID).getPosition();
    Position thirdPlayerPos = getWorldMap()->getPlayer(ourColor, _lastID).getPosition();
    // If our attacker isn't behind ball, but our third player is (and we can swicth them now): switch them!!!
    // OR
    // If both players are behind ball, but the thirdPlayer is closer to it (and we can switch them now): switch them!!!
    // OR
    // If the third player is closer to ball: switch them!!!
    if((!isBehindBallXcoord(attackerPos) && (isBehindBallXcoord(thirdPlayerPos)))
        || (Utils::distance(attackerPos, ballPos) > Utils::distance(thirdPlayerPos, ballPos))) {
        if(!_switchedPlayers && (!playersState[_lastID]->isStuck() || (playersState[_attackerID]->isStuck() && playersState[_lastID]->isStuck()))) {
            quint8 attId = _attackerID;
            _attackerID = _lastID;
            _lastID = attId;
            _switchedPlayers = true;
            _switchPlayersTimer.start();
            _rl_df->setSwitch(true);
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
        for (int j = i+1; j < goalDistance.size(); j++) {
            if (goalDistance[i] > goalDistance[j]) {
                goalDistance.swap(i, j);
                players.swap(i,j);
            }
        }
    }

    if(players.size() >= 1) {
        _goalkeeperID = players[0];
        playersState.insert(_goalkeeperID, new PlayerState(_goalkeeperID));
        if(players.size() >= 2) {
            _lastID = players[1];
            playersState.insert(_lastID, new PlayerState(_lastID));
            if(players.size() >= 3) {
                _attackerID = players[2];
                playersState.insert(_attackerID, new PlayerState(_attackerID));
            }
        }
    }
}

void Playbook_Default::thirdPlayerState() {
    Position selfPosition = getWorldMap()->getPlayer(getConstants()->teamColor(), _lastID).getPosition();
    if(!_switchedPlayers && !_replacedSecRole) {
        if (_defenderState) {
            if(!playersState[_goalkeeperID]->isStuck()) {
                if ((((getWorldMap()->getLocations()->ourSide().isRight() && getWorldMap()->getBall().getVelocity().vx() < 0.0f)
                        || (getWorldMap()->getLocations()->ourSide().isLeft() && getWorldMap()->getBall().getVelocity().vx() > 0.0f))
                        && getWorldMap()->getLocations()->isInsideOurField(getWorldMap()->getBall().getPosition()))
                        || (getWorldMap()->getLocations()->isInsideTheirField(selfPosition) && _switchedPlayers)) {
                    _defenderState = false;
                    _replacedSecRole = true;
                    _replaceSecRoleTimer.start();
                }
            }
        } else {
            Position atkPosition = getWorldMap()->getPlayer(getConstants()->teamColor(), _attackerID).getPosition();
            if (playersState[_attackerID]->isStuck() || (((Utils::distance(atkPosition, getWorldMap()->getBall().getPosition()) > 0.3f
                    && isBehindBallXcoord(atkPosition)) || !isBehindBallXcoord(atkPosition))
                    && ((getWorldMap()->getLocations()->ourSide().isRight() && getWorldMap()->getBall().getVelocity().vx() > 0.0f)
                    || (getWorldMap()->getLocations()->ourSide().isLeft() && getWorldMap()->getBall().getVelocity().vx() < 0.0f)))
                    || (getWorldMap()->getLocations()->isInsideOurField(selfPosition) && _switchedPlayers)
                    || playersState[_goalkeeperID]->isStuck()) {
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

bool Playbook_Default::isBallInsideDefenderEllipse(std::pair<float, float> ellipseParameters) {
    Position ballPosition = getWorldMap()->getBall().getPosition();
    float alpha;
    if (getConstants()->teamSide().isRight()) {
        alpha = static_cast<float>(M_PI) - Utils::getAngle(getWorldMap()->getLocations()->ourGoal(), ballPosition);
    } else {
        alpha = -Utils::getAngle(getWorldMap()->getLocations()->ourGoal(), ballPosition);
    }

    float ellipseDist = sqrt((ellipseParameters.first * ellipseParameters.second) / (ellipseParameters.second
                             * powf(cosf(alpha), 2) + ellipseParameters.first * powf(sinf(alpha), 2)));
    float ballDist = Utils::distance(ballPosition, getWorldMap()->getLocations()->ourGoal());

    if (ballDist < ellipseDist) {
        return true;
    } else {
        return  false;
    }
}
