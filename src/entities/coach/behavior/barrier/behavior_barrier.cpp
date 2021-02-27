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

#include "behavior_barrier.h"

#define INTERCEPT_MINBALLVELOCITY 0.03f//hyperparameter
#define INTERCEPT_MINBALLDIST 0.2f //hyperparameter
#define ERROR_GOAL_OFFSET 0.02f//hyperparameter

#define BALLPREVISION_MINVELOCITY 0.02f
#define BALLPREVISION_VELOCITY_FACTOR 3.0f
#define BALLPREVISION_FACTOR_LIMIT 0.15f

Behavior_Barrier::Behavior_Barrier() {
    setRadius(0.29f);

    _avoidTeammates = true;
    _avoidOpponents = false;
    _avoidBall = false;
    _avoidOurGoalArea = true;
    _avoidTheirGoalArea = true;
}

QString Behavior_Barrier::name() {
    return "Behavior_Barrier";
}

void Behavior_Barrier::configure() {
    _desiredBaseSpeed = getConstants()->playerBaseSpeed();

    // Starting skills
    _skill_goTo = new Skill_GoTo();
    _skill_spin = new Skill_Spin();

    // Adding to behavior skill list
    addSkill(SKILL_GOTO, _skill_goTo);
    addSkill(SKILL_SPIN, _skill_spin);

}

void Behavior_Barrier::run() {
    Position goalProjection;
    Position projectedBall = getWorldMap()->getBall().getPosition();

    goalProjection.setInvalid();

    //considering ball velocity to define player position
    if(getWorldMap()->getBall().getVelocity().abs() > BALLPREVISION_MINVELOCITY){
        //calc unitary vector of velocity
        const Position velUni(true, getWorldMap()->getBall().getVelocity().vx() /getWorldMap()->getBall().getVelocity().abs(),
                              getWorldMap()->getBall().getVelocity().vy()/getWorldMap()->getBall().getVelocity().abs());

        //calc velocity factor
        float factor = BALLPREVISION_VELOCITY_FACTOR*getWorldMap()->getBall().getVelocity().abs();
        Utils::limitValue(&factor, 0.0f, BALLPREVISION_FACTOR_LIMIT);

        //calc projected position
        const Position delta(true, factor*velUni.x(), factor*velUni.y());
        Position projectedPos(true, getWorldMap()->getBall().getPosition().x()+delta.x(),
                              getWorldMap()->getBall().getPosition().y()+delta.y());
        projectedBall = projectedPos;

        goalProjection = Utils::hasInterceptionSegments( getWorldMap()->getBall().getPosition(),
                                                  projectedBall,
                                                  getWorldMap()->getLocations()->ourGoalRightPost(),
                                                  getWorldMap()->getLocations()->ourGoalLeftPost());
    }
    //if interception isn't inside our defense area: consider only ball position
    if((abs(goalProjection.y()) > getWorldMap()->getLocations()->fieldDefenseLength()/2.0f) || goalProjection.isInvalid()){
        goalProjection = Utils::projectPointAtSegment(getWorldMap()->getLocations()->ourGoalRightPost(),
                                               getWorldMap()->getLocations()->ourGoalLeftPost(),
                                               getWorldMap()->getBall().getPosition());
    }

    //Pos barrier
    Position desiredPosition = Utils::threePoints(goalProjection, projectedBall, _radius, 0.0f);

    // Error goal (desiredPosition sometimes goes off the field)
    if(getWorldMap()->getLocations()->ourSide().isRight() && desiredPosition.x() > (getWorldMap()->getLocations()->ourGoal().x()-ERROR_GOAL_OFFSET)){
        desiredPosition.setPosition(getWorldMap()->getLocations()->ourGoal().x()-ERROR_GOAL_OFFSET, desiredPosition.y(), 0.0);
    }else if(getWorldMap()->getLocations()->ourSide().isLeft() && desiredPosition.x() < (getWorldMap()->getLocations()->ourGoal().x()+ERROR_GOAL_OFFSET)){
        desiredPosition.setPosition(getWorldMap()->getLocations()->ourGoal().x()+ERROR_GOAL_OFFSET, desiredPosition.y(), 0.0);
    }

    //if ball is inside our defense area (function isInsideOurArea doesn't consider the area inside the goal)
    if(fabs(desiredPosition.x())>(getWorldMap()->getLocations()->fieldMaxX() - getWorldMap()->getLocations()->fieldDefenseWidth())
            && fabs(desiredPosition.y()) < getWorldMap()->getLocations()->fieldDefenseLength()/2.0f &&
            getWorldMap()->getLocations()->isInsideOurField(desiredPosition)){
        desiredPosition = projectPosOutsideGoalArea(desiredPosition, true, false);
    }

    //setting goto
    _skill_goTo->setTargetPosition(desiredPosition);
    _skill_goTo->setAvoidOurGoalArea(true);
    //setting skill goTo velocity factor
    _skill_goTo->setMovementBaseSpeed(getConstants()->playerBaseSpeed());

    //setting intercept
    float multFactor = 1.0;
    if(getWorldMap()->getLocations()->ourSide().isLeft()){
        multFactor = -1.0;
    }
    Position interceptPoinLeft(true, multFactor * (getWorldMap()->getLocations()->fieldMaxX() - getWorldMap()->getLocations()->fieldDefenseWidth()), multFactor * getWorldMap()->getLocations()->fieldDefenseLength()/2.0f);
    Position interceptPointRight(true, multFactor * (getWorldMap()->getLocations()->fieldMaxX() - getWorldMap()->getLocations()->fieldDefenseWidth()), -1.0f * multFactor * getWorldMap()->getLocations()->fieldDefenseLength()/2.0f);

    //getWorldMap()->getBall().getPosition().y();

    if(getWorldMap()->getLocations()->ourSide().isLeft()){//se for lado esquerdo
          if(player()->position().y() >=0){
              if(getWorldMap()->getBall().getPosition().y() < (getWorldMap()->getLocations()->fieldMaxY())/2){
                  _skill_spin->setClockWise(false);
              }else{
                  _skill_spin->setClockWise(true);
              }
          }
          else{
              if(getWorldMap()->getBall().getPosition().y() < (getWorldMap()->getLocations()->fieldMinY())/2){
                  _skill_spin->setClockWise(false);
              }else{
                  _skill_spin->setClockWise(true);
              }
          }

      }
      else{//lado direito tem que inverter o giro
          if(player()->position().y() >=0){
              if(getWorldMap()->getBall().getPosition().y() < (getWorldMap()->getLocations()->fieldMaxY())/2){
                  _skill_spin->setClockWise(true);
              }else{
                  _skill_spin->setClockWise(false);
              }
          }
          else{
              if(getWorldMap()->getBall().getPosition().y() < (getWorldMap()->getLocations()->fieldMinY())/2){
                  _skill_spin->setClockWise(true);
              }else{
                  _skill_spin->setClockWise(false);
              }
          }
      }

    _skill_goTo->setAvoidBall(_avoidBall);
    _skill_goTo->setAvoidTeammates(_avoidTeammates);
    _skill_goTo->setAvoidOpponents(_avoidOpponents);
    _skill_goTo->setAvoidOurGoalArea(_avoidOurGoalArea);
    _skill_goTo->setAvoidTheirGoalArea(_avoidTheirGoalArea);
    setSkill(SKILL_GOTO);

    if( Utils::distance(player()->position(), getWorldMap()->getBall().getPosition()) <=0.1f
            && (Utils::distance(player()->position(), getWorldMap()->getLocations()->ourGoal()) < Utils::distance(getWorldMap()->getBall().getPosition(), getWorldMap()->getLocations()->ourGoal()))){ //hyperparameter
        //std::cout<<"SPIN\n";
        setSkill(SKILL_SPIN);
    }
}

void Behavior_Barrier::setAvoidFlags(bool avoidBall, bool avoidTeammates, bool avoidOpponents, bool avoidOurGoalArea, bool avoidTheirGoalArea) {
    _avoidBall = avoidBall;
    _avoidTeammates = avoidTeammates;
    _avoidOpponents = avoidOpponents;
    _avoidOurGoalArea = avoidOurGoalArea;
    _avoidTheirGoalArea = avoidTheirGoalArea;
}

Position Behavior_Barrier::projectPosOutsideGoalArea(Position pos, bool avoidOurArea, bool avoidTheirArea){
    Position L1, L2, R1, R2, goal;
    bool shouldProjectPos = false, isOurArea = false;
    float smallMargin = 0.05f;

    if(fabs(pos.x()) > (getWorldMap()->getLocations()->fieldMaxX() - getWorldMap()->getLocations()->fieldDefenseWidth()) &&
            fabs(pos.y()) < getWorldMap()->getLocations()->fieldDefenseLength()/2){

        shouldProjectPos = true;
        //check if desiredPosition is inside our defense area and if we should avoid it
        if(getWorldMap()->getLocations()->isInsideOurField(pos) && avoidOurArea){
            shouldProjectPos = true;
            isOurArea = true;
            goal = getWorldMap()->getLocations()->ourGoal();
        }
        //check if desiredPosition is inside their defense area and if we should avoid it
        else if(getWorldMap()->getLocations()->isInsideTheirField(pos) && avoidTheirArea){
            shouldProjectPos = true;
            isOurArea = false;
            goal = getWorldMap()->getLocations()->theirGoal();
        }else{
            shouldProjectPos = false;
        }
    }
    //if should project position outside a defense area
    if(shouldProjectPos){
        //multiplying factor changes if area's team is the left team or the right team
        float mult = -1.0;
        if(isOurArea){
            if(getWorldMap()->getLocations()->ourSide().isLeft()) mult = 1.0;
        }else{
            if(getWorldMap()->getLocations()->theirSide().isLeft()) mult = 1.0;
        }
        //getting segments

        //left segment points (defense area)
        L1 = Position(true, -mult*getWorldMap()->getLocations()->fieldMaxX(), getWorldMap()->getLocations()->fieldDefenseLength()/2 + smallMargin);
        L2 = Position(true, L1.x() + mult * (getWorldMap()->getLocations()->fieldDefenseWidth() + smallMargin), L1.y());
        //right segment points (defense area)
        R1 = Position(true, -mult*getWorldMap()->getLocations()->fieldMaxX(), -(getWorldMap()->getLocations()->fieldDefenseLength()/2 + smallMargin));
        R2 = Position(true, R1.x() + mult * (getWorldMap()->getLocations()->fieldDefenseWidth() + smallMargin), R1.y());
        //front segment is composed by L2 and R2 (defense area)

        //projecting position on segments L1->L2, R1->R2, L2->R2
        Position pointProjLeft = Utils::projectPointAtSegment(L1, L2, pos);
        Position pointProjRight = Utils::projectPointAtSegment(R1, R2, pos);
        Position pointProjFront = Utils::projectPointAtSegment(L2, R2, pos);

        //interception points between the segment playerPos->pos and defense area segments (L1->L2, R1->R2, L2->R2)
        Position left = Utils::hasInterceptionSegments(L1, L2, goal, pos);
        Position right = Utils::hasInterceptionSegments(R1, R2, goal, pos);
        Position front = Utils::hasInterceptionSegments(L2, R2, goal, pos);

        //if there is an interception between playerPos->pos and L1->L2 on L1->L2
        if(!(left.isInvalid()) &&
                fabs(left.x()) >= (getWorldMap()->getLocations()->fieldMaxX() - (getWorldMap()->getLocations()->fieldDefenseWidth()+smallMargin)) &&
                fabs(left.x()) <= getWorldMap()->getLocations()->fieldMaxX()){
            return pointProjLeft;
        }
        //if there is an interception between playerPos->pos and R1->R2 on R1->R2
        else if(!(right.isInvalid()) &&
                fabs(right.x()) >= (getWorldMap()->getLocations()->fieldMaxX() - (getWorldMap()->getLocations()->fieldDefenseWidth()+smallMargin)) &&
                fabs(right.x()) <= getWorldMap()->getLocations()->fieldMaxX()){
            return pointProjRight;
        }
        //if there is an interception between playerPos->pos and L2->R2 on L2->R2
        else if(!(front.isInvalid()) &&
                fabs(front.x()) >= (getWorldMap()->getLocations()->fieldMaxX() - (getWorldMap()->getLocations()->fieldDefenseWidth()+smallMargin)) && fabs(front.x()) <= getWorldMap()->getLocations()->fieldMaxX()){
            return pointProjFront;
        }else{
            return pos;
        }
    }else{
        return pos;
    }
}
