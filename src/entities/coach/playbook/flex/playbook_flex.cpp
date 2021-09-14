#include "playbook_flex.h"

Playbook_Flex::Playbook_Flex() {
    _switchedPlayers = true;
    _replacedSecRole = true;
    _defenderState = true;
}

QString Playbook_Flex::name() {
    return "Playbook_Flex";
}

void Playbook_Flex::configure(int numPlayers) {
    for(int i = 0; i < numPlayers; i++) {
        _roles_att.push_back(new Role_Attacker());
        usesRole(_roles_att.at(i));
    }
    for(int i = 0; i < numPlayers; i++) {
        _roles_sup.push_back(new Role_Supporter());
        usesRole(_roles_sup.at(i));
    }
    for(int i = 0; i < numPlayers; i++) {
        _roles_def.push_back(new Role_Defender());
        usesRole(_roles_def.at(i));
    }

    _role_gk = new Role_Goalkeeper();
    usesRole(_role_gk);
}

void Playbook_Flex::run(int numPlayers) {
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
    setPlayerRole(_goalkeeperID, _role_gk);
    setPlayerRole(_attackerID, _roles_att.at(0));

    definePunctuation();

    if (_defenderState) {
        if(getWorldMap()->getLocations()->ourGoal().x() > 0){
            _roles_def.at(0)->setEllipseCenter(Position(true, 0.72f, 0.0f));
        }
        else{
            _roles_def.at(0)->setEllipseCenter(Position(true, -0.72f, 0.0f));
        }
        _roles_def.at(0)->setElipseParameters(0.1f, 0.25f);
        setPlayerRole(_lastID, _roles_def.at(0));
    } else {
        setPlayerRole(_lastID, _roles_sup.at(0));
    }
    float deriv;
    if(punctuations.size() > 179) {
        deriv = (punctuations.last() - punctuations.first())/punctuations.size();
        //std::cout << deriv << std::endl;
        if(deriv >= 0.0f) {
            _roles_att.at(1)->setPriority(false);
            setPlayerRole(_lastID, _roles_att.at(1));
            _replacedSecRole = true;
            _replaceSecRoleTimer.start();
        }
    }

}

void Playbook_Flex::definePunctuation() {
    Position objPos = getWorldMap()->getLocations()->ourGoal();
    Colors::Color teamColor = getConstants()->teamColor();
    if(teamColor == Colors::BLUE) teamColor = Colors::YELLOW;
    else teamColor = Colors::BLUE;
    FieldSide teamSide = getConstants()->teamSide();
    QList<quint8> avPlayers = getWorldMap()->getAvailablePlayers(teamColor);
    QList<float> dists;
    for(int i=0; i<avPlayers.size(); i++) {
        Position playerPos = getWorldMap()->getPlayer(teamColor, avPlayers.at(i)).getPosition();
        float dist = Utils::distance(playerPos, objPos);
        dists.append(dist);
    }
    std::sort(dists.begin(), dists.end());
    if(dists.size() > 2) {
        dists.pop_back();
    }
    dists.append(Utils::distance(getWorldMap()->getBall().getPosition(), objPos));
    float totalDists = 0;
    for(int i=0; i< dists.size(); i++){
        totalDists += dists.at(i);
        //std::cout << "dists at " << i << " " << dists.at(i) << std::endl;
    }
    float punct = totalDists/(dists.size()*1.6f);
    punctuations.append(punct);
    if(punctuations.size() > 180) {
        punctuations.pop_front();
    }
}

void Playbook_Flex::updatePlayerStuck(quint8 id) {
    Colors::Color ourColor = getConstants()->teamColor();
    //float ballVel = getWorldMap()->getBall().getVelocity().abs();
    float playerVel = getWorldMap()->getPlayer(ourColor, id).getVelocity().abs();
    // If has stopped (possibly stuck)
    if(playerVel <= 0.02f && minDistPlayerObstacle(id) < 0.1f) {
        if(!playersState[id]->stuckState) {
            playersState[id]->stuckState = true;
            playersState[id]->timer.start();
        }
    }else {
        playersState[id]->stuckState = false;
    }
}

float Playbook_Flex::minDistPlayerObstacle(quint8 id) {
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

void Playbook_Flex::switchPlayersIDs() {
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
        }
    }
}

void Playbook_Flex::selectInitialIDs() {
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

void Playbook_Flex::thirdPlayerState() {
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

bool Playbook_Flex::isBehindBallXcoord(Position pos) {
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

bool Playbook_Flex::isBallInsideDefenderEllipse(float ellipseA, float ellipseB) {
    Position ballPosition = getWorldMap()->getBall().getPosition();
    float alpha;
    if (getConstants()->teamSide().isRight()) {
        alpha = static_cast<float>(M_PI) - Utils::getAngle(getWorldMap()->getLocations()->ourGoal(), ballPosition);
    } else {
        alpha = -Utils::getAngle(getWorldMap()->getLocations()->ourGoal(), ballPosition);
    }

    float ellipseDist = sqrt((ellipseA * ellipseB) / (ellipseB * powf(cosf(alpha), 2) + ellipseA * powf(sinf(alpha), 2)));
    float ballDist = Utils::distance(ballPosition, getWorldMap()->getLocations()->ourGoal());

    if (ballDist < ellipseDist) {
        return true;
    } else {
        return  false;
    }
}
