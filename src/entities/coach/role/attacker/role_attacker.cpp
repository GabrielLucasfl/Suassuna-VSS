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

Role_Attacker::Role_Attacker() {
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
}

void Role_Attacker::run() {
    Position playerPos = player()->position();
    Position ballPos = player()->getWorldMap()->getBall().getPosition();

    //ball possession
    bool ballPossession;
    if(player()->getPlayerDistanceTo(ballPos) < 0.04f) {
        ballPossession = true;
    } else {
        ballPossession = false;
    }

    if(_bhv_goToBall->isBehindBall(playerPos) && ballPossession){

    }

    switch (_state) {
        case GOTOBALL : {
            _bhv_goToBall->setReferencePosition(player()->getWorldMap()->getLocations()->theirGoal());
            _bhv_goToBall->setOffsetBehindBall(0.13f);


            setBehavior(BHV_GOTOBALL);


        } break;
        case MOVETO : {
            setBehavior(BHV_MOVETO);



        } break;
        default : {

        } break;
    }

    //_bhv_attacker->setTargetPosition(player()->getWorldMap()->getBall().getPosition());

    //_bhv_moveTo->setTargetPosition(player()->getWorldMap()->getBall().getPosition());

    //_behavior_moveTo->setMinimalVelocity();
    //_bhv_intercept->setInterceptSegment(Position(true, 0.6f, 0.35f), Position(true, 0.6f, -0.35f));
    //_bhv_intercept->setObjectPosition(player()->getWorldMap()->getBall().getPosition());
    //_bhv_intercept->setObjectVelocity(player()->getWorldMap()->getBall().getVelocity());

    setBehavior(BHV_GOTOBALL);
}

