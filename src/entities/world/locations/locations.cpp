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

#include "locations.h"

#define MM2METER 1.0f/1000.0f

Locations::Locations(FieldSide ourSide, Field *field, Constants *constants) {
    _field = field;
    _ourSide = ourSide;
    _constants = constants;

    // Load default dimensions based on defaultField
    const float fieldX = _field->maxX();
    const float fieldY = _field->maxY();
    const float goalY = _field->goalWidth()/2;
    const float defenseAreaWidth = _field->defenseRadius();
    const float defenseAreaLength = _field->defenseStretch();

    // Set constant positions
    _fieldCenter = Position(true, 0.0, 0.0);

    // Set constant values
    _defenseAreaWidth = defenseAreaWidth;
    _defenseAreaLength = defenseAreaLength;

    // Update field corners
    _fieldBottomLeftCorner  = Position(true, -fieldX, -fieldY);
    _fieldBottomRightCorner = Position(true, fieldX, -fieldY);
    _fieldTopLeftCorner     = Position(true, -fieldX, fieldY);
    _fieldTopRightCorner    = Position(true, fieldX, fieldY);

    // Update goal/posts positions
    _rightGoalLeftPost = Position(true, fieldX, -goalY);
    _rightGoalRightPost = Position(true, fieldX, goalY);
    _leftGoalLeftPost = Position(true, -fieldX, goalY);
    _leftGoalRightPost = Position(true, -fieldX, -goalY);

    // Update Defense areas corners
    _leftAreaLeftBackCorner = Position(true, -fieldX, defenseAreaLength/2.0f);
    _leftAreaRightBackCorner = Position(true, -fieldX, -defenseAreaLength/2.0f);
    _rightAreaLeftBackCorner = Position(true, fieldX, -defenseAreaLength/2.0f);
    _rightAreaRightBackCorner = Position(true, fieldX, defenseAreaLength/2.0f);
    _leftAreaLeftFrontCorner = Position(true, -fieldX + 0.15f, _leftAreaLeftBackCorner.y());
    _leftAreaRightFrontCorner = Position(true, -fieldX + 0.15f, _leftAreaRightBackCorner.y());
    _rightAreaLeftFrontCorner = Position(true, fieldX - 0.15f, _rightAreaLeftBackCorner.y());
    _rightAreaRightFrontCorner = Position(true, fieldX - 0.15f, _rightAreaRightBackCorner.y());

    // Penalty marks
    _rightPenaltyMark = Position(true, (fieldX/2.0f - defenseAreaWidth), 0.0);
    _leftPenaltyMark = Position(true, (-fieldX/2.0f + defenseAreaWidth), 0.0);

    // Setting Walls points
    if(getConstants()->teamSide().isLeft()) {
        //right side walls
        _walls.push_back(Wall(Position(true, fieldX, -goalY) , Position(true, fieldX, -fieldY)));
        _walls.push_back(Wall(Position(true, fieldX, goalY) , Position(true, fieldX, fieldY)));
    } else {
        //left side walls
        _walls.push_back(Wall(Position(true, -fieldX, -goalY) , Position(true, -fieldX, -fieldY)));
        _walls.push_back(Wall(Position(true, -fieldX, goalY) , Position(true, -fieldX, fieldY)));
    }
    //bottom and upper walls
    _walls.push_back(Wall(Position(true, -fieldX, -fieldY) , Position(true, fieldX, -fieldY)));
    _walls.push_back(Wall(Position(true, -fieldX, fieldY) , Position(true, fieldX, fieldY)));    
    //triangle walls
    //right triangles
    _walls.push_back(Wall(Position(true, -fieldX, -fieldY + 0.07) , Position(true, -fieldX + 0.07, -fieldY)));
    _walls.push_back(Wall(Position(true, -fieldX, fieldY - 0.07) , Position(true, -fieldX + 0.07 , fieldY)));
    //left triangles
    _walls.push_back(Wall(Position(true, fieldX, -fieldY + 0.07) , Position(true, fieldX - 0.07, -fieldY)));
    _walls.push_back(Wall(Position(true, fieldX, fieldY - 0.07) , Position(true, fieldX - 0.07, fieldY)));
}

Locations::~Locations() {
    delete _field;
}

Constants* Locations::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Constants with nullptr value at Locations") + '\n';
    } else {
        return _constants;
    }

    return nullptr;
}

FieldSide Locations::ourSide() {
    return _ourSide;
}

FieldSide Locations::theirSide() {
    return (_ourSide.isRight() ? Sides::LEFT : Sides::RIGHT);
}

QList<Wall> Locations::getWalls() {
    _mutex.lock();
    QList<Wall> walls = _walls;
    _mutex.unlock();

    return walls;
}

Position Locations::fieldCenter() {
    _mutex.lock();
    Position fieldCenter = _fieldCenter;
    _mutex.unlock();

    return fieldCenter;
}

float Locations::fieldMinX()  {    
    float fieldTopCornerX = ourFieldTopCorner().x();

    _mutex.lock();
    float fMinX = -fabs(fieldTopCornerX);
    _mutex.unlock();

    return fMinX;
}

float Locations::fieldMaxX()  {
    float fieldTopCornerX = ourFieldTopCorner().x();

    _mutex.lock();
    float fMaxX = fabs(fieldTopCornerX);
    _mutex.unlock();

    return fMaxX;
}

float Locations::fieldMinY()  {
    float fieldTopCornerY = ourFieldTopCorner().y();

    _mutex.lock();
    float fMinY = -fabs(fieldTopCornerY);
    _mutex.unlock();

    return fMinY;
}

float Locations::fieldMaxY()  {
    float fieldTopCornerY = ourFieldTopCorner().y();

    _mutex.lock();
    float fMaxY = fabs(fieldTopCornerY);
    _mutex.unlock();

    return fMaxY;
}

float Locations::fieldLength()  {
    float fieldMX = fieldMaxX();

    _mutex.lock();
    float fLength = 2*fieldMX;
    _mutex.unlock();

    return fLength;
}

float Locations::fieldWidth()  {
    float fieldMY = fieldMaxY();

    _mutex.lock();
    float fWidth = 2*fieldMY;
    _mutex.unlock();

    return fWidth;
}

Position Locations::fieldRightTopCorner() {
    float fieldMX = fieldMaxX();
    float fieldMY = fieldMaxY();

    _mutex.lock();
    Position fRightTopCorner = Position(true, fieldMX, fieldMY);
    _mutex.unlock();

    return fRightTopCorner;
}

Position Locations::fieldRightBottomCorner() {
    float fieldMX = fieldMaxX();
    float fieldMY = fieldMaxY();

    _mutex.lock();
    Position fRightBottomCorner = Position(true, fieldMX, fieldMY);
    _mutex.unlock();

    return fRightBottomCorner;
}

Position Locations::fieldLeftTopCorner() {
    float fieldMX = fieldMaxX();
    float fieldMY = fieldMaxY();

    _mutex.lock();
    Position fLeftTopCorner = Position(true, fieldMX, fieldMY);
    _mutex.unlock();

    return fLeftTopCorner;
}

Position Locations::fieldLeftBottomCorner() {
    float fieldMX = fieldMaxX();
    float fieldMY = fieldMaxY();

    _mutex.lock();
    Position fLeftBottomCorner = Position(true, fieldMX, fieldMY);
    _mutex.unlock();

    return fLeftBottomCorner;
}

Position Locations::ourFieldTopCorner() {
    _mutex.lock();
    Position ourFTopCorner = (ourSide().isRight() ? _fieldTopRightCorner : _fieldTopLeftCorner);
    _mutex.unlock();

    return ourFTopCorner;
}

Position Locations::ourFieldBottomCorner() {
    _mutex.lock();
    Position ourFBottomCorner = (ourSide().isRight() ? _fieldBottomRightCorner : _fieldBottomLeftCorner);
    _mutex.unlock();

    return ourFBottomCorner;
}

Position Locations::ourGoal() {
    _mutex.lock();
    Position ourG = Position(true, (ourSide().isRight() ? _rightGoalLeftPost : _leftGoalLeftPost).x(), 0.0);
    _mutex.unlock();

    return ourG;
}

Position Locations::ourGoalRightPost() {
    _mutex.lock();
    Position ourGRightPost = (ourSide().isRight() ? _rightGoalRightPost : _leftGoalRightPost);
    _mutex.unlock();

    return ourGRightPost;
}

Position Locations::ourGoalLeftPost() {
    _mutex.lock();
    Position ourGLeftPost = (ourSide().isRight() ? _rightGoalLeftPost : _leftGoalLeftPost);
    _mutex.unlock();

    return ourGLeftPost;
}

Position Locations::ourPenaltyMark() {
    _mutex.lock();
    Position ourPMark = (ourSide().isRight() ? _rightPenaltyMark : _leftPenaltyMark);
    _mutex.unlock();

    return ourPMark;
}

Position Locations::theirFieldTopCorner() {
    _mutex.lock();
    Position theirFTopCorner = theirSide().isRight() ? _fieldTopRightCorner : _fieldTopLeftCorner;
    _mutex.unlock();

    return theirFTopCorner;
}

Position Locations::theirFieldBottomCorner() {
    _mutex.lock();
    Position theirFBottomCorner = (theirSide().isRight() ? _fieldBottomRightCorner : _fieldBottomLeftCorner);
    _mutex.unlock();

    return theirFBottomCorner;
}

Position Locations::theirGoal() {
    _mutex.lock();
    Position theirG =Position(true, (theirSide().isRight() ? _rightGoalLeftPost : _leftGoalLeftPost).x(), 0.0);
    _mutex.unlock();

    return theirG;
}

Position Locations::theirGoalRightPost() {
    _mutex.lock();
    Position theirGRightPost =(theirSide().isRight() ? _rightGoalRightPost : _leftGoalRightPost);
    _mutex.unlock();

    return theirGRightPost;
}

Position Locations::theirGoalLeftPost() {
    _mutex.lock();
    Position theirGLeftPost = (theirSide().isRight() ? _rightGoalLeftPost : _leftGoalLeftPost);
    _mutex.unlock();

    return theirGLeftPost;
}

Position Locations::theirPenaltyMark() {
    _mutex.lock();
    Position theirPMark = (theirSide().isRight() ? _rightPenaltyMark : _leftPenaltyMark);
    _mutex.unlock();

    return theirPMark;
}

float Locations::fieldCenterRadius() {
    _mutex.lock();
    float fCenterRadius = _fieldCenterRadius;
    _mutex.unlock();

    return fCenterRadius;
}

float Locations::fieldDefenseLength() {
    _mutex.lock();
    float fDefenseLength = _goalLength;
    _mutex.unlock();

    return fDefenseLength;
}

float Locations::fieldDefenseWidth() {
    _mutex.lock();
    float fDefenseWidth = _goalWidth;
    _mutex.unlock();

    return fDefenseWidth;
}

float Locations::fieldGoalDepth() {
    _mutex.lock();
    float fGoalDepth = _goalDepth;
    _mutex.unlock();

    return fGoalDepth;
}

bool Locations::isInsideOurField(const Position &pos) {
    return ((ourSide().isRight() && pos.x()>=0) || (ourSide().isLeft() && pos.x()<=0));
}

bool Locations::isInsideTheirField(const Position &pos) {
    return (isInsideOurField(pos)==false);
}

bool Locations::isInsideOurArea(const Position &pos) {
    if(ourSide().isLeft()) {
        return (pos.x() <= _leftAreaLeftFrontCorner.x()
                && pos.y() <= _leftAreaLeftBackCorner.y() && pos.y() >= _leftAreaRightBackCorner.y());
    } else {
        return (pos.x() >= _rightAreaRightFrontCorner.x()
                && pos.y() <= _rightAreaRightBackCorner.y() && pos.y() >= _rightAreaLeftBackCorner.y());
    }
}

bool Locations::isInsideTheirArea(const Position &pos) {
    if(theirSide().isLeft()) {
        return (pos.x() <= _leftAreaLeftFrontCorner.x()
                && pos.y() <= _leftAreaLeftBackCorner.y() && pos.y() >= _leftAreaRightBackCorner.y());
    } else {
        return (pos.x() >= _rightAreaRightFrontCorner.x()
                && pos.y() <= _rightAreaRightBackCorner.y() && pos.y() >= _rightAreaLeftBackCorner.y());
    }
}

bool Locations::isOutsideField(const Position &pos, float factor) {
    return _isOutsideField(pos, factor*fieldMaxX(), factor*fieldMaxY());
}

bool Locations::isOutsideField(const Position &pos, const float dx, const float dy) {
    return _isOutsideField(pos, fieldMaxX()+dx, fieldMaxY()+dy);
}

bool Locations::isInsideField(const Position &pos, float factor) {
    return (!isOutsideField(pos, factor));
}

bool Locations::isInsideField(const Position &pos, float dx, float dy) {
    return (!isOutsideField(pos, dx, dy));
}

bool Locations::_isOutsideField(const Position &pos, const float maxX, const float maxY) {
    if(fabs(pos.x()) > maxX)
        return true;
    if(fabs(pos.y()) > maxY)
        return true;
    return false;
}

Position Locations::ourAreaLeftBackCorner() {
    Position ourAreaLeftBackCorner;
    _mutex.lock();
    if(theirSide().isRight()) {
        ourAreaLeftBackCorner = _leftAreaLeftBackCorner;
    } else {
        ourAreaLeftBackCorner = _rightAreaLeftBackCorner;
    }
    _mutex.unlock();
    return ourAreaLeftBackCorner;
}

Position Locations::ourAreaRightBackCorner() {
    Position ourAreaRightBackCorner;
    _mutex.lock();
    if(theirSide().isRight()) {
        ourAreaRightBackCorner = _leftAreaRightBackCorner;
    } else {
        ourAreaRightBackCorner = _rightAreaRightBackCorner;
    }
    _mutex.unlock();
    return ourAreaRightBackCorner;
}

Position Locations::theirAreaRightBackCorner() {
    Position theirAreaRightBackCorner;
    _mutex.lock();
    if(theirSide().isRight()) {
        theirAreaRightBackCorner = _rightAreaRightBackCorner;
    } else {
        theirAreaRightBackCorner = _leftAreaRightBackCorner;
    }
    _mutex.unlock();
    return theirAreaRightBackCorner;
}

Position Locations::theirAreaLeftBackCorner() {
    Position theirAreaLeftBackCorner;
    _mutex.lock();
    if(theirSide().isRight()) {
        theirAreaLeftBackCorner = _rightAreaLeftBackCorner;
    } else {
        theirAreaLeftBackCorner = _leftAreaLeftBackCorner;
    }
    _mutex.unlock();
    return theirAreaLeftBackCorner;
}

Position Locations::ourAreaLeftFrontCorner() {
    Position ourAreaLeftFrontCorner;
    _mutex.lock();
    if(theirSide().isRight()) {
        ourAreaLeftFrontCorner = _leftAreaLeftFrontCorner;
    } else {
        ourAreaLeftFrontCorner = _rightAreaLeftFrontCorner;
    }
    _mutex.unlock();
    return ourAreaLeftFrontCorner;
}

Position Locations::ourAreaRightFrontCorner() {
    Position ourAreaRightFrontCorner;
    _mutex.lock();
    if(theirSide().isRight()) {
        ourAreaRightFrontCorner = _leftAreaRightFrontCorner;
    } else {
        ourAreaRightFrontCorner = _rightAreaRightFrontCorner;
    }
    _mutex.unlock();
    return ourAreaRightFrontCorner;
}

Position Locations::theirAreaRightFrontCorner() {
    Position theirAreaRightFrontCorner;
    _mutex.lock();
    if(theirSide().isRight()) {
        theirAreaRightFrontCorner = _rightAreaRightFrontCorner;
    } else {
        theirAreaRightFrontCorner = _leftAreaRightFrontCorner;
    }
    _mutex.unlock();
    return theirAreaRightFrontCorner;
}

Position Locations::theirAreaLeftFrontCorner() {
    Position theirAreaLeftFrontCorner;
    _mutex.lock();
    if(theirSide().isRight()) {
        theirAreaLeftFrontCorner = _rightAreaLeftFrontCorner;
    } else {
        theirAreaLeftFrontCorner = _leftAreaLeftFrontCorner;
    }
    _mutex.unlock();
    return theirAreaLeftFrontCorner;
}

float Locations::defenseAreaWidth() {
    float width;
    _mutex.lock();
    width = _defenseAreaWidth;
    _mutex.unlock();
    return width;
}

float Locations::defenseAreaLength() {
    float length;
    _mutex.lock();
    length = _defenseAreaLength;
    _mutex.unlock();
    return length;
}

void Locations::updateGeometryData(fira_message::Field geometryData) {
    _mutex.lock();

    _lastData = geometryData;

    fira_message::Field field = geometryData;
    //REVIEW DATA URGENTLY (WHEN NEEDED)
    // Calc centerRadius and areaRadius
    float centerRadius = 200.0f;
    float areaRadius = 150.0f;

    // Calc defense area info
    float areaLength = 700.0f;
    float areaWidth = 150.0f;

    // Updating positions
    _fieldCenter = Position(true, 0.0, 0.0);
    _fieldTopRightCorner = Position(true, (field.length()/2.0f)*MM2METER, (field.width()/2.0f)*MM2METER);
    _fieldTopLeftCorner = Position(true, (-field.length()/2.0)*MM2METER, (field.width()/2.0)*MM2METER);
    _fieldBottomLeftCorner = Position(true, (-field.length()/2.0)*MM2METER, (-field.width()/2.0)*MM2METER);
    _fieldBottomRightCorner = Position(true, (field.length()/2.0)*MM2METER, (-field.width()/2.0)*MM2METER);
    _rightPenaltyMark = Position(true, (field.length()/2.0 - areaRadius)*MM2METER, 0.0);
    _leftPenaltyMark = Position(true, (-field.length()/2.0 + areaRadius)*MM2METER, 0.0);
    _leftGoalLeftPost = Position(true, (-field.length()/2.0)*MM2METER, (field.goal_width()/2.0)*MM2METER);
    _leftGoalRightPost = Position(true, (-field.length()/2.0)*MM2METER, (-field.goal_width()/2.0)*MM2METER);
    _rightGoalLeftPost = Position(true, (field.length()/2.0)*MM2METER, (-field.goal_width()/2.0)*MM2METER);
    _rightGoalRightPost = Position(true, (field.length()/2.0)*MM2METER, (field.goal_width()/2.0)*MM2METER);
    _rightAreaRightBackCorner = Position(true, (field.length()/2.0)*MM2METER, (areaLength/2.0f)*MM2METER);
    _rightAreaLeftBackCorner = Position(true, (field.length()/2.0)*MM2METER, (-areaLength/2.0f)*MM2METER);
    _leftAreaRightBackCorner = Position(true, (-field.length()/2.0)*MM2METER, (-areaLength/2.0f)*MM2METER);
    _leftAreaLeftBackCorner = Position(true, (-field.length()/2.0)*MM2METER, (areaLength/2.0f)*MM2METER);
    _fieldCenterRadius = centerRadius*MM2METER;
    _goalLength = areaLength*MM2METER;
    _goalWidth = areaWidth*MM2METER;
    _goalDepth = field.goal_depth()*MM2METER;

    _mutex.unlock();
}
