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

#include "role_attacker.h"
#include <src/utils/utils.h>

#define ATKFACTOR 0.2f

Role_Attacker::Role_Attacker() {
    _avoidTeammates = false;
    _avoidOpponents = false;
    _avoidBall = false;
    _avoidOurGoalArea = false;
    _avoidTheirGoalArea = false;
    _offsetRange = 0.3f;
    _charge = true;
    _gameInterrupted = false;
}

QString Role_Attacker::name() {
    return "Role_Attacker";
}

void Role_Attacker::configure() {
    // Starting behaviors
    _bhv_moveTo = new Behavior_MoveTo();
    _bhv_goToBall = new Behavior_GoToBall();

    // Adding behaviors to behaviors list
    addBehavior(BHV_MOVETO, _bhv_moveTo);
    addBehavior(BHV_GOTOBALL, _bhv_goToBall);

    _state = GOTOBALL;
    _push = false;
    //setBehavior(BHV_GOTOBALL);
}

void Role_Attacker::run() {

    // Ball projection
    Position ballPos = getWorldMap()->getBall().getPosition();
    Velocity ballVel = getWorldMap()->getBall().getVelocity();
    Position ballDirection, ballProj;
    if(ballVel.abs() > 0.03f) {
        ballDirection = Position(true, ballVel.vx()/ballVel.abs(), ballVel.vy()/ballVel.abs());
    } else {
        ballDirection = Position(true, 0, 0);
    }
    float factor = std::min(ATKFACTOR * ballVel.abs(), 0.5f);
    ballProj = Position(true, ballPos.x() + factor*ballDirection.x(), ballPos.y() + factor*ballDirection.y());

    // Bhv goToBall parameters
    float bhvGoToBallOffset = 0.25f;
    Position theirGoal = getWorldMap()->getLocations()->theirGoal();
    Position bhvGoToBallRef;
    if(theirGoal.x() < 0) {
        bhvGoToBallRef.setPosition(true, theirGoal.x() - 0.12f, theirGoal.y());
    }else {
        bhvGoToBallRef.setPosition(true, theirGoal.x() + 0.12f, theirGoal.y());
    }

    Colors::Color ourColor = getConstants()->teamColor();
    if(getWorldMap()->getPlayer(ourColor, player()->playerId()).getVelocity().abs() < 0.02f
                                && getWorldMap()->getBall().getVelocity().abs() < 0.02f) {
        _offsetRange = 0.1f;
        _charge = true;
    }else {
        _offsetRange = 0.25f;
        _charge = false;
    }

    //check if player is behind ball based on its reference position
    bool isInRange = inRangeToPush(ballProj) && (Utils::distance(ballProj, player()->position()) > _offsetRange);

    _avoidTheirGoalArea = hasAllyInTheirArea();

    if(getReferee()->isGameOn() && _gameInterrupted == true) {
        _interuption.start();
        _gameInterrupted = false;
        _state = MOVETO;
    }

    switch (_state) {
        case GOTOBALL: {
            _avoidBall = true;
            _avoidTeammates = true;
            _avoidOpponents = true;
            _avoidOurGoalArea = true;
            _bhv_goToBall->setReferencePosition(getWorldMap()->getLocations()->theirGoal());
            _bhv_goToBall->setOffsetBehindBall(bhvGoToBallOffset);
            _bhv_goToBall->setAvoidFlags(_avoidBall, _avoidTeammates, _avoidOpponents, _avoidOurGoalArea,
                                         _avoidTheirGoalArea);
            _bhv_goToBall->setLinearError(0.02f);
            setBehavior(BHV_GOTOBALL);
            if(isInRange) {
                _state = MOVETO;
            }
            break;
        }
        case MOVETO: {
            _avoidBall = false;
            _avoidTeammates = false;
            _avoidOpponents = false;
            _avoidOurGoalArea = true;
            _bhv_moveTo->setAvoidFlags(_avoidBall, _avoidTeammates, _avoidOpponents, _avoidOurGoalArea, _avoidTheirGoalArea);
            if(!_push) {
                _bhv_moveTo->setBaseSpeed(getConstants()->playerBaseSpeed());
                _bhv_moveTo->setTargetPosition(ballProj);
            }else {
                _bhv_moveTo->setBaseSpeed(50);
                _bhv_moveTo->setTargetPosition(ballProj);
            }

            if(Utils::distance(player()->position(), ballPos) < 0.06f && !_push) {
                _push = true;
            }
            _bhv_moveTo->setLinearError(0.02f);
            setBehavior(BHV_MOVETO);

            //transitions
            _interuption.stop();
            if((Utils::distance(player()->position(), ballProj) >= 0.3f || !player()->isBehindBallXCoord(player()->position()))
                                && _interuption.getSeconds() > 1) {
                _push = false;
                _state = GOTOBALL;
            }
            break;
        }
        case AVOIDBALL: {
            _avoidBall = true;
            _avoidTeammates = true;
            _avoidOpponents = false;
            _avoidOurGoalArea = true;
            _bhv_moveTo->setAvoidFlags(_avoidBall, _avoidTeammates, _avoidOpponents, _avoidOurGoalArea, _avoidTheirGoalArea);
            float moveX = getWorldMap()->getLocations()->ourSide().isLeft()? -0.3f : 0.3f;
            float moveY = (ballProj.y() > 0)? 0.4f : -0.4f;
            _bhv_moveTo->setTargetPosition(Position(true, moveX, moveY));
            setBehavior(MOVETO);

            if((abs(ballPos.y()) > 0.4f) || player()->isBehindBallXCoord(player()->position())) {
                _state = GOTOBALL;
            }
            break;
        }
        default: {
            break;
        }
    }
}

bool Role_Attacker::hasAllyInTheirArea() {
    Colors::Color ourColor = getConstants()->teamColor();
    QList<quint8> ourPlayers = getWorldMap()->getAvailablePlayers(ourColor);
    for(int i=0; i<ourPlayers.size(); i++) {
        Position posPlayer = getWorldMap()->getPlayer(ourColor, ourPlayers[i]).getPosition();
        if((ourPlayers[i] != player()->playerId()) && getWorldMap()->getLocations()->isInsideTheirArea(posPlayer)) {
            return true;
        }
    }
    return false;
}

bool Role_Attacker::inRangeToPush(Position ballPos) {
    Position firstPost;
    Position secondPost;
    float offsetX = 0.12f;

    firstPost = getWorldMap()->getLocations()->theirAreaRightPost();
    secondPost = getWorldMap()->getLocations()->theirAreaLeftPost();

    //firstpost's treatment
    if(firstPost.y() < 0) {
        firstPost.setPosition(true, firstPost.x(), firstPost.y() - 0.1f);
    }else {
        firstPost.setPosition(true, firstPost.x(), firstPost.y() + 0.1f);
    }
    if(firstPost.x() < 0) {
        firstPost.setPosition(true, firstPost.x() - offsetX, firstPost.y());
    }else {
        firstPost.setPosition(true, firstPost.x() + offsetX, firstPost.y());
    }

    //secondpost's treatment
    secondPost = getWorldMap()->getLocations()->theirAreaLeftPost();
    if(secondPost.y() < 0) {
        secondPost.setPosition(true, secondPost.x(), secondPost.y() - 0.1f);
    }else {
        secondPost.setPosition(true, secondPost.x(), secondPost.y() + 0.1f);
    }
    if(secondPost.x() < 0) {
        secondPost.setPosition(true, secondPost.x() - offsetX, secondPost.y());
    }else {
        secondPost.setPosition(true, secondPost.x() + offsetX, secondPost.y());
    }

    // Angle from ball to posts of their goal
    float ballFirstPost = normAngle(Utils::getAngle(ballPos, firstPost) - static_cast<float>(M_PI));
    float ballSecondPost = normAngle(Utils::getAngle(ballPos, secondPost) - static_cast<float>(M_PI));

    // Angle from ball to player
    float ballPlayer = Utils::getAngle(ballPos, player()->position());

    // Compare angles to know if player is in the angle range to push the ball to their goal
    if(getWorldMap()->getLocations()->theirSide().isRight()) {
        if((ballSecondPost < 0 && ballFirstPost > 0) || (ballSecondPost > 0 && ballFirstPost < 0)) {
            if((ballPlayer > std::max(ballSecondPost, ballFirstPost))
                || (ballPlayer < std::min(ballSecondPost, ballFirstPost))) {
                return true;
            }else {
                return false;
            }
        }
    }
    if(ballPlayer < std::max(ballSecondPost, ballFirstPost)
        && (ballPlayer > std::min(ballSecondPost, ballFirstPost))) {
        return true;
    } else {
        return false;
    }

}

float Role_Attacker::normAngle(float angleRadians) {
    if (angleRadians > static_cast<float>(M_PI)) {
        return angleRadians = angleRadians - 2 * static_cast<float>(M_PI);
    } else if (angleRadians < -static_cast<float>(M_PI)) {
        return angleRadians = angleRadians + 2 * static_cast<float>(M_PI);
    } else {
        return angleRadians;
    }
}

QPair<Position, Angle> Role_Attacker::getPlacementPosition(VSSRef::Foul foul, VSSRef::Color forTeam,
                                                           VSSRef::Quadrant atQuadrant) {
    Position standardPosition;
    if (getWorldMap()->getLocations()->ourSide().isRight()) {
        standardPosition = Position(true, 0.25f, 0.0f);
    } else {
        standardPosition = Position(true, -0.25f, 0.0f);
    }

    Position foulPosition;
    Angle foulAngle;

    switch (foul) {
    case VSSRef::Foul::PENALTY_KICK: {
        if (VSSRef::Color(getConstants()->teamColor()) == forTeam) {
            penaltyKick(OURTEAM, &_penaltyPlacement);
            foulPosition = _penaltyPlacement.first;
            foulAngle = _penaltyPlacement.second;
            _gameInterrupted = true;
        } else {
            penaltyKick(THEIRTEAM, &_penaltyPlacement);
            foulPosition = _penaltyPlacement.first;
            foulAngle = _penaltyPlacement.second;
        }
    } break;
    case VSSRef::Foul::KICKOFF: {
        if (VSSRef::Color(getConstants()->teamColor()) == forTeam) {
            kickOff(OURTEAM,&_penaltyPlacement);
            foulPosition = _penaltyPlacement.first;
            foulAngle = _penaltyPlacement.second;
            _gameInterrupted = true;
        }else{
            kickOff(THEIRTEAM,&_penaltyPlacement);
            foulPosition = _penaltyPlacement.first;
            foulAngle = _penaltyPlacement.second;
        }
    } break;
    case VSSRef::Foul::FREE_BALL: {
        freeBall(&_penaltyPlacement, atQuadrant);
        foulPosition = _penaltyPlacement.first;
        foulAngle = _penaltyPlacement.second;
        _gameInterrupted = true;
    } break;
    case VSSRef::Foul::GOAL_KICK: {
        if (static_cast<VSSRef::Color>(getConstants()->teamColor()) == forTeam) {
            goalKick(OURTEAM, &_penaltyPlacement);
            foulPosition = _penaltyPlacement.first;
            foulAngle = _penaltyPlacement.second;
        } else {
            foulPosition = standardPosition;
            foulAngle = Angle(true, 90);
        }
    } break;
    default: {
        foulPosition = standardPosition;
        foulAngle = Angle(true, 90);
    }
    }

    return QPair<Position, Angle>(foulPosition, foulAngle);
}

void Role_Attacker:: penaltyKick(quint8 _teamPriority,QPair<Position, Angle> *_penaltyPlacement){

    Colors::Color enemyColor;
    if (getConstants()->teamColor() == Colors::BLUE) {
        enemyColor = Colors::YELLOW;
    } else {
        enemyColor = Colors::BLUE;
    }

    Position gkPos;
    QList<quint8> enemyPlayers = getWorldMap()->getAvailablePlayers(enemyColor);

    for (int i = 0; i < enemyPlayers.size(); i++) {
        Position enemyPlayerPosition = getWorldMap()->getPlayer(enemyColor, enemyPlayers[i]).getPosition();
        if(getWorldMap()->getLocations()->isInsideTheirArea(enemyPlayerPosition)){
            gkPos = enemyPlayerPosition;
            break;
        }
    }

    if(_teamPriority == 1){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            Position _amPenaltyLeft(true, getWorldMap()->getLocations()->theirGoal().x(),
                                    getWorldMap()->getLocations()->theirGoalLeftPost().y() - 0.05);
            Position _amPenaltyRight(true, getWorldMap()->getLocations()->theirGoal().x(),
                                     getWorldMap()->getLocations()->theirGoalRightPost().y() + 0.05);
            if(gkPos.y() > 0 ){
                _penaltyPlacement->first  = Utils::threePoints(Position(true,-0.375, 0), _amPenaltyLeft, 0.1f, Angle::pi);
            }else{
                _penaltyPlacement->first = Utils::threePoints(Position(true,-0.375, 0), _amPenaltyRight, 0.1f, Angle::pi);
            }
            _penaltyPlacement->second = Angle(true,Utils::getAngle(_penaltyPlacement->first, Position(true,-0.375, 0)));
        }else{
            Position _amPenaltyLeft(true, getWorldMap()->getLocations()->theirGoal().x(),
                                    getWorldMap()->getLocations()->theirGoalLeftPost().y() + 0.05);
            Position _amPenaltyRight(true, getWorldMap()->getLocations()->theirGoal().x(),
                                     getWorldMap()->getLocations()->theirGoalRightPost().y() - 0.05);
            if(gkPos.y() >= 0 ){
                _penaltyPlacement->first = Utils::threePoints(Position(true,0.375, 0), _amPenaltyRight, 0.1f, Angle::pi);
            }else{
                _penaltyPlacement->first = Utils::threePoints(Position(true,0.375, 0), _amPenaltyLeft, 0.1f, Angle::pi);
            }
            _penaltyPlacement->second = Angle(true,Utils::getAngle(_penaltyPlacement->first, Position(true,0.375, 0)));
        }

    }else{
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            _penaltyPlacement->first = Position(true, -0.2f,-0.165f);
            _penaltyPlacement->second = Angle(true, 0);
        }else{
            _penaltyPlacement->first = Position(true, 0.2f,0.165f);
            _penaltyPlacement->second = Angle(true, 0);
        }
    }
}

void Role_Attacker:: kickOff(quint8 _teamPriority, QPair<Position, Angle> *_penaltyPlacement){
    if(_teamPriority == OURTEAM){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            //assistant near the middle of the field
            _penaltyPlacement->first = Utils::threePoints(getWorldMap()->getLocations()->fieldCenter(),
                                                          Position(true, -0.75,-0.35),
                                                          0.1f, Angle::pi);
            _penaltyPlacement->second = Angle(true, Utils::getAngle(_penaltyPlacement->first,
                                                                    getWorldMap()->getLocations()->fieldCenter()));
        }else{
            //assistant near the middle of the field
            _penaltyPlacement->first = Utils::threePoints(getWorldMap()->getLocations()->fieldCenter(),
                                                          Position(true, 0.75, 0.35),
                                                          0.1f, Angle::pi);
            _penaltyPlacement->second = Angle(true, Utils::getAngle(_penaltyPlacement->first,
                                                                    getWorldMap()->getLocations()->fieldCenter()));
        }
    }else{
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            _penaltyPlacement->first = Position(true, 0.25,0.0);
            _penaltyPlacement->second = Angle(true, 0.0);
        }else{
            _penaltyPlacement->first = Position(true, -0.25,0.0);
            _penaltyPlacement->second = Angle(true, 0.0);
        }
    }
}

void Role_Attacker::freeBall(QPair<Position, Angle> *_penaltyPlacement, VSSRef::Quadrant quadrant){
    float freeBallXabs = (getWorldMap()->getLocations()->fieldMaxX() - 0.375);
    float freeBallYabs = (getWorldMap()->getLocations()->fieldMaxY() - 0.25f);
    float freeBallOffset = 0.2f;

    if(quadrant == VSSRef::QUADRANT_1){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            //player in quadrant 1 near our goal (right)
            _penaltyPlacement->first = Position(true, (freeBallXabs + freeBallOffset), freeBallYabs);
            _penaltyPlacement->second = Angle(true, Angle::pi);
        }else{
            //player in quadrant 1 near our goal (left)
            _penaltyPlacement->first = Position(true, (freeBallXabs - freeBallOffset), freeBallYabs);
            _penaltyPlacement->second = Angle(true, 0);
        }
    }else if(quadrant == VSSRef::QUADRANT_2){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            //player in quadrant 2 near our goal (right)
            _penaltyPlacement->first = Position(true, (-1*freeBallXabs + freeBallOffset), freeBallYabs);
            _penaltyPlacement->second = Angle(true, Angle::pi);
        }else{
            //player in quadrant 2 near our goal (left)
            _penaltyPlacement->first = Position(true, (-1*freeBallXabs - freeBallOffset), freeBallYabs);
            _penaltyPlacement->second = Angle(true, 0);
        }
    }else if(quadrant == VSSRef::QUADRANT_3){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            //player in quadrant 3 near our goal (right)
            _penaltyPlacement->first = Position(true, (-1*freeBallXabs + freeBallOffset), -1*freeBallYabs);
            _penaltyPlacement->second = Angle(true, Angle::pi);
        }else{
            //player in quadrant 3 near our goal (left)
            _penaltyPlacement->first = Position(true, (-1*freeBallXabs - freeBallOffset), -1*freeBallYabs);
            _penaltyPlacement->second = Angle(true, 0);
        }
    }else if(quadrant == VSSRef::QUADRANT_4){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            //player in quadrant 4 near our goal (right)
            _penaltyPlacement->first = Position(true, (freeBallXabs + freeBallOffset), -1*freeBallYabs);
            _penaltyPlacement->second = Angle(true, Angle::pi);
        }else{
            //player in quadrant 4 near our goal (left)
            _penaltyPlacement->first = Position(true, (freeBallXabs - freeBallOffset), -1*freeBallYabs);
            _penaltyPlacement->second = Angle(true, 0);
        }
    }
}

void Role_Attacker:: goalKick(quint8 _teamPriority ,QPair<Position, Angle> *_penaltyPlacement){
    float defenseXabs = (getWorldMap()->getLocations()->fieldMaxX() - 0.55f);
    float defenseYabs = 0.0f;
    float nearTheMiddleXabs = (getWorldMap()->getLocations()->fieldMaxX()/2 - 0.15f);

    if(_teamPriority == 1){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            _penaltyPlacement->first = Position(true, defenseXabs, -1*defenseYabs);
            _penaltyPlacement->second = Angle(true, 0);
        }else{
            _penaltyPlacement->first = Position(true, -1*defenseXabs, defenseYabs);
            _penaltyPlacement->second = Angle(true, Angle::pi);
        }
    }else{
        //assistant always in front of our goal and near the middle of the field
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            _penaltyPlacement->first = Position(true, nearTheMiddleXabs, 0);
            _penaltyPlacement->second = Angle(true, Angle::pi);
        }else{
            _penaltyPlacement->first = Position(true, -1*nearTheMiddleXabs, 0);
            _penaltyPlacement->second = Angle(true, 0);
        }

    }
}
