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
    _positiveAngle = true;
    _prior = true;
    _lastPos = Position(false, 0.0f, 0.0f);
    _pos = Position(false, 0.0f, 0.0f);
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
    _lastSpeed  = getConstants()->playerBaseSpeed();
}

void Role_Attacker::run() {

    // Ball projection
    Position ballPos = getWorldMap()->getBall().getPosition();
    Velocity ballVel = getWorldMap()->getBall().getVelocity();
    Position ballDirection;
    if(ballVel.abs() > 0.03f) {
        ballDirection = Position(true, ballVel.vx()/ballVel.abs(), ballVel.vy()/ballVel.abs());
    } else {
        ballDirection = Position(true, 0, 0);
    }
    //Position ballPred = getWorldMap()->getBall().getMatch(getWorldMap()->getBall());
    Position ballPred = getWorldMap()->getBall().getPredPosition(20);
    float ballPlayerDist = Utils::distance(ballPred, player()->position());
    // to check if player is in range to push
    Colors::Color ourColor = getConstants()->teamColor();

    _avoidTheirGoalArea = hasAllyInTheirArea();

    if(getReferee()->isGameOn() && _gameInterrupted == true) {
        _interuption.start();
        _gameInterrupted = false;
        _state = MOVETO;
    }

    if(!_pos.isInvalid()){
        _lastPos = _pos;
    }

    Position referencePos = defineReferencePosition();
    float angle = normAngle(Utils::getAngle(ballPred, referencePos) - Utils::getAngle(player()->position(), ballPred));
    float dist = getDist(angle);
    float targetAngle = getAngle(angle);
    _pos = Utils::threePoints(ballPred, referencePos, dist, targetAngle);
    if(fabs(ballPos.y()) >= 0.450f){
        _pos = ballPred;
    }

    if(fabs(angle) < static_cast<float>(M_PI)/11.25f && _prior){
        _push = true;
    }

    float posAngle = 0.0f;
    if(!_lastPos.isInvalid()){
        posAngle = fabs(normAngle(Utils::getAngle(_pos, referencePos) - Utils::getAngle(_lastPos, ballPred)));
    }

    std::cout << "posAngle: " << posAngle*180/static_cast<float>(M_PI) << std::endl;

    switch (_state) {
        case GOTOBALL: {
            _avoidBall = false;
            _avoidTeammates = false;
            _avoidOpponents = false;
            _avoidOurGoalArea = true;
            _bhv_moveTo->setAvoidFlags(_avoidBall, _avoidTeammates, _avoidOpponents, _avoidOurGoalArea, _avoidTheirGoalArea);
            //_bhv_moveTo->setBaseSpeed(getConstants()->playerBaseSpeed() + 5.0f);
            _bhv_moveTo->setBaseSpeed(0.0f);
            player()->setPlayerDesiredPosition(_pos);
            _bhv_moveTo->setLinearError(0.02f);
            setBehavior(BHV_MOVETO);
            //std::cout << "GOTOBALL\n";
            // if player is in range or: if it is near the ball and the angle between them is small
            //if(((fabs(angle) < static_cast<float>(M_PI)/11.25) && Utils::distance(ballPos, player()->position()) < 0.2f) && _prior && inRangeToPush(ballPred)) {
            if(((fabs(angle) < static_cast<float>(M_PI)/11.25f) && _prior && inRangeToPush(ballPred)) ) {
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
                _bhv_moveTo->setBaseSpeed(getConstants()->playerBaseSpeed()+5.0f);
                player()->setPlayerDesiredPosition(getPushPosition(ballPred));
            } else {
                _bhv_moveTo->setBaseSpeed(pushSpeed(ballPlayerDist));
                player()->setPlayerDesiredPosition(getPushPosition(ballPred));
            }
            _bhv_moveTo->setBaseSpeed(0.0f);
            _bhv_moveTo->setLinearError(0.02f);
            setBehavior(BHV_MOVETO);
            //std::cout << "MOVETO\n";
            //transitions
            _interuption.stop();
            float velX = getWorldMap()->getPlayer(ourColor, player()->playerId()).getMavgVelocity().vx();
            bool goingAgainst = false;
            if((getConstants()->teamSide().isRight() && velX > 0) || (getConstants()->teamSide().isLeft() && velX < 0)) {
                goingAgainst = true;
            }
            if(((Utils::distance(ballPos, player()->position()) >= 0.3f) || (goingAgainst && Utils::distance(ballPos, player()->position()) <= 0.1f))
                    && _interuption.getSeconds() > 1) {
                _push = false;
                _state = GOTOBALL;
                _lastSpeed = getConstants()->playerBaseSpeed();
            }
            break;
        }
        default: {
            break;
        }
    }
}

Position Role_Attacker::defineReferencePosition() {
    Position theirGoal = getWorldMap()->getLocations()->theirGoal();
    Position ballPos = getWorldMap()->getBall().getPosition();
    Position referencePos;
    float extraMargin = 0.2f;
    if(!_prior) {
        if(getWorldMap()->getLocations()->theirSide().isRight()) {
            if(ballPos.y() > 0){
                referencePos = getWorldMap()->getLocations()->theirFieldTopCorner();
                referencePos = Position(true, referencePos.x(), referencePos.y() + extraMargin);
            }else {
                referencePos = getWorldMap()->getLocations()->theirFieldBottomCorner();
                referencePos = Position(true, referencePos.x(), referencePos.y() - extraMargin);
            }
        }else {
            if(ballPos.y() > 0) {
                referencePos = getWorldMap()->getLocations()->theirFieldTopCorner();
                referencePos = Position(true, referencePos.x(), referencePos.y() + extraMargin);
            }else {
                referencePos = getWorldMap()->getLocations()->theirFieldBottomCorner();
                referencePos = Position(true, referencePos.x(), referencePos.y() - extraMargin);
            }
        }
    }else {
        referencePos = theirGoal;
    }
    return referencePos;
}

Position Role_Attacker::getPushPosition(Position ballPos){
    Position pos = Utils::threePoints(ballPos, getWorldMap()->getLocations()->theirGoal(), 0.1f, 0.0f);
    if(fabs(ballPos.y()) >= 0.450f){
        pos = ballPos;
    }
    return pos;
}

float Role_Attacker::getAngle(float angle){
    int signal = static_cast<int>(angle/fabs(angle));
    if(_positiveAngle) {
        if(signal < 0 && fabs(angle) < 0.9f*static_cast<float>(M_PI)) {
            signal = 1;
            _positiveAngle = false;
        }
    }else {
        if(signal > 0 && fabs(angle) < 0.9f*static_cast<float>(M_PI)) {
            signal = -1;
            _positiveAngle = true;
        }
    }
    float halfAngle = fabs(angle)/2;
    return signal * (static_cast<float>(M_PI-M_PI/22) - halfAngle);
}


float Role_Attacker::getDist(float angle){
    float maxAngle = static_cast<float>(M_PI)/2;
    //std::cout << "angle: " << angle * 180/static_cast<float>(M_PI) << std::endl;

    if(angle > 0){
        angle = fmin(angle, maxAngle);
    }
    else{
        angle = fmax(angle, -maxAngle);
    }

    float maxDist = 0.45f, delta = 0.3f;
    float dist = maxDist - delta*((maxAngle - fabs(angle))/(maxAngle));
    if(!_prior) {
        dist += 0.3f;
    }else {
        // Avoid stucking in wall
        Position ballPos = getWorldMap()->getBall().getPosition();
        float ballPlayerDist = Utils::distance(player()->position(), ballPos);
        float maxDistAllowed = std::min(fabs(dist), fabs(getWorldMap()->getLocations()->fieldMaxY()) - fabs(ballPos.y()));
        maxDistAllowed = std::min(maxDistAllowed, fabs(getWorldMap()->getLocations()->fieldMaxX()) - fabs(ballPos.x()));
        if(maxDistAllowed - 0.035f < 0.025f) {
            maxDistAllowed = 0.025f;
        }else {
            maxDistAllowed -= 0.035f;
        }
        dist = std::min(dist, maxDistAllowed);

    }

    return dist;
}

float Role_Attacker::pushSpeed(float ballPlayerDist){
    if(ballPlayerDist < 0.07f){
        //std::cout << "Vel max\n";
        return 45;
    }
    float factor = std::cbrt((ballPlayerDist-0.11f)/0.15f);
    float speed = 45, delta = speed - getConstants()->playerBaseSpeed() - 5.0f;
    _lastSpeed = std::max(speed-delta*factor, _lastSpeed);
    //std::cout << "Vel variavel: " << std::max(_lastSpeed, speed-delta*factor)<< std::endl;
    return _lastSpeed;
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

bool Role_Attacker::inRangeToPush(Position ballPos, float postIncrement) {
    Position firstPost;
    Position secondPost;
    float offsetX = 0.12f;

    firstPost = getWorldMap()->getLocations()->theirGoalRightPost();
    secondPost = getWorldMap()->getLocations()->theirGoalLeftPost();

    //firstpost's treatment
    if(firstPost.y() < 0) {
        firstPost.setPosition(true, firstPost.x(), firstPost.y() - postIncrement);
    } else {
        firstPost.setPosition(true, firstPost.x(), firstPost.y() + postIncrement);
    }
    if(firstPost.x() < 0) {
        firstPost.setPosition(true, firstPost.x() - offsetX, firstPost.y());
    } else {
        firstPost.setPosition(true, firstPost.x() + offsetX, firstPost.y());
    }

    //secondpost's treatment
    if(secondPost.y() < 0) {
        secondPost.setPosition(true, secondPost.x(), secondPost.y() - postIncrement);
    } else {
        secondPost.setPosition(true, secondPost.x(), secondPost.y() + postIncrement);
    }
    if(secondPost.x() < 0) {
        secondPost.setPosition(true, secondPost.x() - offsetX, secondPost.y());
    } else {
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
            penaltyKick(OURTEAM, &_placement);
            foulPosition = _placement.first;
            foulAngle = _placement.second;
            _gameInterrupted = true;
        } else {
            penaltyKick(THEIRTEAM, &_placement);
            foulPosition = _placement.first;
            foulAngle = _placement.second;
        }
    } break;
    case VSSRef::Foul::KICKOFF: {
        if (VSSRef::Color(getConstants()->teamColor()) == forTeam) {
            kickOff(OURTEAM,&_placement);
            foulPosition = _placement.first;
            foulAngle = _placement.second;
            _gameInterrupted = true;
        } else {
            kickOff(THEIRTEAM,&_placement);
            foulPosition = _placement.first;
            foulAngle = _placement.second;
        }
    } break;
    case VSSRef::Foul::FREE_BALL: {
        freeBall(&_placement, atQuadrant);
        foulPosition = _placement.first;
        foulAngle = _placement.second;
        _gameInterrupted = true;
    } break;
    case VSSRef::Foul::GOAL_KICK: {
        if (static_cast<VSSRef::Color>(getConstants()->teamColor()) == forTeam) {
            goalKick(OURTEAM, &_placement);
            foulPosition = _placement.first;
            foulAngle = _placement.second;
        } else {
            goalKick(THEIRTEAM, &_placement);
            foulPosition = _placement.first;
            foulAngle = _placement.second;
        }
    } break;
    default: {
        foulPosition = standardPosition;
        foulAngle = Angle(true, 90);
    }
    }

    return QPair<Position, Angle>(foulPosition, foulAngle);
}

void Role_Attacker:: penaltyKick(quint8 _teamPriority,QPair<Position, Angle> *_placement){

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
            if(_prior) {
                Position _amPenaltyLeft(true, getWorldMap()->getLocations()->theirGoal().x(),
                                        getWorldMap()->getLocations()->theirGoalLeftPost().y() - 0.05f);
                Position _amPenaltyRight(true, getWorldMap()->getLocations()->theirGoal().x(),
                                         getWorldMap()->getLocations()->theirGoalRightPost().y() + 0.05f);
                if(gkPos.y() > 0 ){
                    _placement->first  = Utils::threePoints(Position(true,-0.375, 0), _amPenaltyLeft, 0.1f, Angle::pi);
                } else {
                    _placement->first = Utils::threePoints(Position(true,-0.375, 0), _amPenaltyRight, 0.1f, Angle::pi);
                }
                _placement->second = Angle(true,Utils::getAngle(_placement->first, Position(true,-0.375, 0)));
            }else {
                _placement->first = Position(true, 0.08f, 0);
                _placement->second = Angle(true, Angle::pi/2);
            }
        } else {
            if(_prior) {
                Position _amPenaltyLeft(true, getWorldMap()->getLocations()->theirGoal().x(),
                                        getWorldMap()->getLocations()->theirGoalLeftPost().y() + 0.05f);
                Position _amPenaltyRight(true, getWorldMap()->getLocations()->theirGoal().x(),
                                         getWorldMap()->getLocations()->theirGoalRightPost().y() - 0.05f);
                if(gkPos.y() >= 0 ){
                    _placement->first = Utils::threePoints(Position(true,0.375, 0), _amPenaltyRight, 0.1f, Angle::pi);
                } else {
                    _placement->first = Utils::threePoints(Position(true,0.375, 0), _amPenaltyLeft, 0.1f, Angle::pi);
                }
                _placement->second = Angle(true,Utils::getAngle(_placement->first, Position(true,0.375, 0)));
            }else {
                _placement->first = Position(true, -1*0.08f, 0);
                _placement->second = Angle(true, Angle::pi/2);
            }
        }

    } else {
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            if(_prior) {
                _placement->first = Position(true, -0.2f,-0.165f);
                _placement->second = Angle(true, Angle::pi/2);
            }else {
                _placement->first = Position(true, -0.2f,0.165f);
                _placement->second = Angle(true, Angle::pi/2);
            }
        } else {
            if(_prior) {
                _placement->first = Position(true, 0.2f,0.165f);
                _placement->second = Angle(true, Angle::pi/2);
            }else {
                _placement->first = Position(true, 0.2f,-1*0.165f);
                _placement->second = Angle(true, Angle::pi/2);
            }
        }
    }
}

void Role_Attacker:: kickOff(quint8 _teamPriority, QPair<Position, Angle> *_placement){
    float secondAttXabs = 0.3f;
    if(_teamPriority == OURTEAM){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            //near the middle of the field
            if(_prior) {
                _placement->first = Utils::threePoints(getWorldMap()->getLocations()->fieldCenter(),
                                                              Position(true, -0.75, -0.35f),
                                                              0.1f, Angle::pi);
                _placement->second = Angle(true, Utils::getAngle(_placement->first,
                                                                        getWorldMap()->getLocations()->fieldCenter()));
            }else {
                _placement->first = Position(true, secondAttXabs, 0);
                _placement->second = Angle(true, Angle::pi/2);
            }
        } else {
            //near the middle of the field
            if(_prior) {
                _placement->first = Utils::threePoints(getWorldMap()->getLocations()->fieldCenter(),
                                                              Position(true, 0.75, 0.35f),
                                                              0.1f, Angle::pi);
                _placement->second = Angle(true, Utils::getAngle(_placement->first,
                                                                        getWorldMap()->getLocations()->fieldCenter()));
            }else {
                _placement->first = Position(true, -1*secondAttXabs, 0);
                _placement->second = Angle(true, Angle::pi/2);
            }
        }
    } else {
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            if(_prior) {
                _placement->first = Position(true, 0.25f,0.0);
                _placement->second = Angle(true, 0.0);
            }else {
                _placement->first = Position(true, 0.45f,0.0);
                _placement->second = Angle(true, Angle::pi/2);
            }
        } else {
            if(_prior) {
                _placement->first = Position(true, -0.25f,0.0);
                _placement->second = Angle(true, 0.0);
            }else {
                _placement->first = Position(true, -0.45f,0.0);
                _placement->second = Angle(true, Angle::pi/2);
            }
        }
    }
}

void Role_Attacker::freeBall(QPair<Position, Angle> *_placement, VSSRef::Quadrant quadrant){
    float freeBallXabs = (getWorldMap()->getLocations()->fieldMaxX() - 0.375f);
    float freeBallYabs = (getWorldMap()->getLocations()->fieldMaxY() - 0.25f);
    float freeBallOffset = 0.2f;
    float quadrantDistOffset = 0.08f;

    if(quadrant == VSSRef::QUADRANT_1){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            //player in quadrant 1 near our goal (right)
            if(_prior) {
                _placement->first = Position(true, (freeBallXabs + freeBallOffset), freeBallYabs);
                _placement->second = Angle(true, Angle::pi);
            }else {
                // Remain at the same position (only changes its coordinates if it is inside the quadrant)
                Position playerPos = player()->position();
                if(playerPos.y() >= 0 && playerPos.x() >= 0) {
                    playerPos.setPosition(true, playerPos.x(), -quadrantDistOffset);
                }
                _placement->first = playerPos;
                _placement->second = Angle(true, Angle::pi/2);
            }
        } else {
            //player in quadrant 1 near their goal (right)
            if(_prior) {
                _placement->first = Position(true, (freeBallXabs - freeBallOffset), freeBallYabs);
                _placement->second = Angle(true, 0);
            }else {
                // Remain at the same position (only changes its coordinates if it is inside the quadrant)
                Position playerPos = player()->position();
                if(playerPos.y() >= 0 && playerPos.x() >= 0) {
                    playerPos.setPosition(true, -quadrantDistOffset, playerPos.y());
                }
                _placement->first = playerPos;
                _placement->second = Angle(true, Angle::pi/2);
            }
        }
    } else if(quadrant == VSSRef::QUADRANT_2){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            //player in quadrant 2 near their goal (left)
            if(_prior) {
                _placement->first = Position(true, (-1*freeBallXabs + freeBallOffset), freeBallYabs);
                _placement->second = Angle(true, Angle::pi);
            }else {
                // Remain at the same position (only changes its coordinates if it is inside the quadrant)
                Position playerPos = player()->position();
                if(playerPos.y() >= 0 && playerPos.x() <= 0) {
                    playerPos.setPosition(true, quadrantDistOffset, playerPos.y());
                }
                _placement->first = playerPos;
                _placement->second = Angle(true, Angle::pi/2);
            }
        } else {
            //player in quadrant 2 near our goal (left)
            if(_prior) {
                _placement->first = Position(true, (-1*freeBallXabs - freeBallOffset), freeBallYabs);
                _placement->second = Angle(true, 0);
            }else {
                // Remain at the same position (only changes its coordinates if it is inside the quadrant)
                Position playerPos = player()->position();
                if(playerPos.y() >= 0 && playerPos.x() <= 0) {
                    playerPos.setPosition(true, playerPos.x(), -quadrantDistOffset);
                }
                _placement->first = playerPos;
                _placement->second = Angle(true, Angle::pi/2);
            }
        }
    } else if(quadrant == VSSRef::QUADRANT_3){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            //player in quadrant 3 near their goal (left)
            if(_prior) {
                _placement->first = Position(true, (-1*freeBallXabs + freeBallOffset), -1*freeBallYabs);
                _placement->second = Angle(true, Angle::pi);
            }else {
                // Remain at the same position (only changes its coordinates if it is inside the quadrant)
                Position playerPos = player()->position();
                if(playerPos.y() <= 0 && playerPos.x() <= 0) {
                    playerPos.setPosition(true, quadrantDistOffset, playerPos.y());
                }
                _placement->first = playerPos;
                _placement->second = Angle(true, Angle::pi/2);
            }
        } else {
            //player in quadrant 3 near our goal (left)
            if(_prior) {
                _placement->first = Position(true, (-1*freeBallXabs - freeBallOffset), -1*freeBallYabs);
                _placement->second = Angle(true, 0);
            }else {
                // Remain at the same position (only changes its coordinates if it is inside the quadrant)
                Position playerPos = player()->position();
                if(playerPos.y() <= 0 && playerPos.x() <= 0) {
                    playerPos.setPosition(true, playerPos.x(), quadrantDistOffset);
                }
                _placement->first = playerPos;
                _placement->second = Angle(true, Angle::pi/2);
            }
        }
    } else if(quadrant == VSSRef::QUADRANT_4){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            //player in quadrant 4 near our goal (right)
            if(_prior) {
                _placement->first = Position(true, (freeBallXabs + freeBallOffset), -1*freeBallYabs);
                _placement->second = Angle(true, Angle::pi);
            }else {
                // Remain at the same position (only changes its coordinates if it is inside the quadrant)
                Position playerPos = player()->position();
                if(playerPos.y() <= 0 && playerPos.x() >= 0) {
                    playerPos.setPosition(true, playerPos.x(), quadrantDistOffset);
                }
                _placement->first = playerPos;
                _placement->second = Angle(true, Angle::pi/2);
            }
        } else {
            //player in quadrant 4 near their goal (right)
            if(_prior) {
                _placement->first = Position(true, (freeBallXabs - freeBallOffset), -1*freeBallYabs);
                _placement->second = Angle(true, 0);
            }else {
                // Remain at the same position (only changes its coordinates if it is inside the quadrant)
                Position playerPos = player()->position();
                if(playerPos.y() <= 0 && playerPos.x() >= 0) {
                    playerPos.setPosition(true, -quadrantDistOffset, playerPos.y());
                }
                _placement->first = playerPos;
                _placement->second = Angle(true, Angle::pi/2);
            }
        }
    }
}

void Role_Attacker:: goalKick(quint8 _teamPriority ,QPair<Position, Angle> *_placement){
    float defenseXabs = (getWorldMap()->getLocations()->fieldMaxX() - 0.55f);
    float defenseYabs = 0.0f;
    float nearTheMiddleXabs = (getWorldMap()->getLocations()->fieldMaxX()/2 - 0.15f);

    if(_teamPriority == 1){
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            if(_prior) {
                _placement->first = Position(true, defenseXabs, -1*defenseYabs);
                _placement->second = Angle(true, Angle::pi/2);
            }else {
                _placement->first = Position(true, defenseXabs + 0.25f, -1*defenseYabs);
                _placement->second = Angle(true, Angle::pi/2);
            }
        } else {
            if(_prior) {
                _placement->first = Position(true, -1*defenseXabs, defenseYabs);
                _placement->second = Angle(true, Angle::pi/2);
            }else {
                _placement->first = Position(true, -1*defenseXabs - 0.25f, defenseYabs);
                _placement->second = Angle(true, Angle::pi/2);
            }
        }
    } else {
        //always in front of our goal and near the middle of the field
        if(getWorldMap()->getLocations()->ourSide().isRight()){
            if(_prior) {
                _placement->first = Position(true, nearTheMiddleXabs - 0.25f, 0);
                _placement->second = Angle(true, Angle::pi/2);
            }else {
                _placement->first = Position(true, nearTheMiddleXabs, 0);
                _placement->second = Angle(true, Angle::pi/2);
            }
        } else {
            if(_prior) {
                _placement->first = Position(true, -1*nearTheMiddleXabs + 0.25f, 0);
                _placement->second = Angle(true, Angle::pi/2);
            }else {
                _placement->first = Position(true, -1*nearTheMiddleXabs, 0);
                _placement->second = Angle(true, Angle::pi/2);
            }
        }

    }
}
