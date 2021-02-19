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

Locations::Locations(FieldSide ourSide, Field *field) {
    _field = field;
    _ourSide = ourSide;

    // Load default dimensions based on defaultField
    const float fieldX = _field->maxX();
    const float fieldY = _field->maxY();
    const float goalY = _field->goalWidth()/2;
    const float defenseAreaWidth = _field->defenseRadius();
    const float defenseAreaLength = _field->defenseStretch();

    // Set constant positions
    _fieldCenter = Position(true, 0.0, 0.0);

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
    _rightGoalRightMidPost = Position(true, fieldX, -defenseAreaLength/2.0f);
    _rightGoalLeftMidPost = Position(true, fieldX, defenseAreaLength/2.0f);
    _leftGoalRightMidPost = Position(true, -fieldX, defenseAreaLength/2.0f);
    _leftGoalLeftMidPost = Position(true, -fieldX, -defenseAreaLength/2.0f);

    // Defense areas
    _leftAreaLeftPost = Position(true, -fieldX, -0.35f);
    _leftAreaRightPost = Position(true, -fieldX, 0.35f);
    _rightAreaLeftPost = Position(true, fieldX, 0.35f);
    _rightAreaRightPost = Position(true, fieldX, -0.35f);
    _leftAreaLeftCorner = Position(true, _leftAreaLeftPost.x() + 0.15f, _leftAreaLeftPost.y());
    _leftAreaRightCorner = Position(true, _leftAreaRightPost.x() + 0.15f, _leftAreaRightPost.y());
    _rightAreaLeftCorner = Position(true, _rightAreaLeftPost.x() - 0.15f, _rightAreaLeftPost.y());
    _rightAreaRightCorner = Position(true, _rightAreaRightPost.x() - 0.15f, _rightAreaRightPost.y());
    _defenseAreaWidth = 0.15f;
    _defenseAreaLength = 0.7f;

    // Penalty marks
    _rightPenaltyMark = Position(true, (fieldX/2.0f - defenseAreaWidth), 0.0);
    _leftPenaltyMark = Position(true, (-fieldX/2.0f + defenseAreaWidth), 0.0);
}

Locations::~Locations() {
    delete _field;
}

FieldSide Locations::ourSide() {
    return _ourSide;
}

FieldSide Locations::theirSide() {
    return (_ourSide.isRight() ? Sides::LEFT : Sides::RIGHT);
}

Position Locations::fieldCenter() {
    _mutex.lockForRead();
    Position fieldCenter = _fieldCenter;
    _mutex.unlock();

    return fieldCenter;
}

float Locations::fieldMinX()  {
    _mutex.lockForRead();
    float fMinX = -fabs(ourFieldTopCorner().x());
    _mutex.unlock();

    return fMinX;
}

float Locations::fieldMaxX()  {
    _mutex.lockForRead();
    float fMaxX = fabs(ourFieldTopCorner().x());
    _mutex.unlock();

    return fMaxX;
}

float Locations::fieldMinY()  {
    _mutex.lockForRead();
    float fMinY = -fabs(ourFieldTopCorner().y());
    _mutex.unlock();

    return fMinY;
}

float Locations::fieldMaxY()  {
    _mutex.lockForRead();
    float fMaxY = fabs(ourFieldTopCorner().y());
    _mutex.unlock();

    return fMaxY;
}

float Locations::fieldLength()  {
    _mutex.lockForRead();
    float fLength = 2*fieldMaxX();
    _mutex.unlock();

    return fLength;
}

float Locations::fieldWidth()  {
    _mutex.lockForRead();
    float fWidth = 2*fieldMaxY();
    _mutex.unlock();

    return fWidth;
}

Position Locations::fieldRightTopCorner() {
    _mutex.lockForRead();
    Position fRightTopCorner = Position(true, this->fieldMaxX(), this->fieldMaxY());
    _mutex.unlock();

    return fRightTopCorner;
}

Position Locations::fieldRightBottomCorner() {
    _mutex.lockForRead();
    Position fRightBottomCorner = Position(true, this->fieldMaxX(), this->fieldMinY());
    _mutex.unlock();

    return fRightBottomCorner;
}

Position Locations::fieldLeftTopCorner() {
    _mutex.lockForRead();
    Position fLeftTopCorner = Position(true, this->fieldMinX(), this->fieldMaxY());
    _mutex.unlock();

    return fLeftTopCorner;
}

Position Locations::fieldLeftBottomCorner() {
    _mutex.lockForRead();
    Position fLeftBottomCorner = Position(true, this->fieldMinX(), this->fieldMinY());
    _mutex.unlock();

    return fLeftBottomCorner;
}

Position Locations::ourFieldTopCorner() {
    _mutex.lockForRead();
    Position ourFTopCorner = (ourSide().isRight() ? _fieldTopRightCorner : _fieldTopLeftCorner);
    _mutex.unlock();

    return ourFTopCorner;
}

Position Locations::ourFieldBottomCorner() {
    _mutex.lockForRead();
    Position ourFBottomCorner = (ourSide().isRight() ? _fieldBottomRightCorner : _fieldBottomLeftCorner);
    _mutex.unlock();

    return ourFBottomCorner;
}

Position Locations::ourGoal() {
    _mutex.lockForRead();
    Position ourG = Position(true, (ourSide().isRight() ? _rightGoalLeftPost : _leftGoalLeftPost).x(), 0.0);
    _mutex.unlock();

    return ourG;
}

Position Locations::ourGoalRightPost() {
    _mutex.lockForRead();
    Position ourGRightPost = (ourSide().isRight() ? _rightGoalRightPost : _leftGoalRightPost);
    _mutex.unlock();

    return ourGRightPost;
}

Position Locations::ourGoalLeftPost() {
    _mutex.lockForRead();
    Position ourGLeftPost = (ourSide().isRight() ? _rightGoalLeftPost : _leftGoalLeftPost);
    _mutex.unlock();

    return ourGLeftPost;
}

Position Locations::ourPenaltyMark() {
    _mutex.lockForRead();
    Position ourPMark = (ourSide().isRight() ? _rightPenaltyMark : _leftPenaltyMark);
    _mutex.unlock();

    return ourPMark;
}

Position Locations::theirFieldTopCorner() {
    _mutex.lockForRead();
    Position theirFTopCorner = theirSide().isRight() ? _fieldTopRightCorner : _fieldTopLeftCorner;
    _mutex.unlock();

    return theirFTopCorner;
}

Position Locations::theirFieldBottomCorner() {
    _mutex.lockForRead();
    Position theirFBottomCorner = (theirSide().isRight() ? _fieldBottomRightCorner : _fieldBottomLeftCorner);
    _mutex.unlock();

    return theirFBottomCorner;
}

Position Locations::theirGoal() {
    _mutex.lockForRead();
    Position theirG =Position(true, (theirSide().isRight() ? _rightGoalLeftPost : _leftGoalLeftPost).x(), 0.0);
    _mutex.unlock();

    return theirG;
}

Position Locations::theirGoalRightPost() {
    _mutex.lockForRead();
    Position theirGRightPost =(theirSide().isRight() ? _rightGoalRightPost : _leftGoalRightPost);
    _mutex.unlock();

    return theirGRightPost;
}

Position Locations::theirGoalLeftPost() {
    _mutex.lockForRead();
    Position theirGLeftPost = (theirSide().isRight() ? _rightGoalLeftPost : _leftGoalLeftPost);
    _mutex.unlock();

    return theirGLeftPost;
}

Position Locations::theirPenaltyMark() {
    _mutex.lockForRead();
    Position theirPMark = (theirSide().isRight() ? _rightPenaltyMark : _leftPenaltyMark);
    _mutex.unlock();

    return theirPMark;
}

float Locations::fieldCenterRadius() {
    _mutex.lockForRead();
    float fCenterRadius = _fieldCenterRadius;
    _mutex.unlock();

    return fCenterRadius;
}

float Locations::fieldDefenseLength() {
    _mutex.lockForRead();
    float fDefenseLength = _goalLength;
    _mutex.unlock();

    return fDefenseLength;
}

float Locations::fieldDefenseWidth() {
    _mutex.lockForRead();
    float fDefenseWidth = _goalWidth;
    _mutex.unlock();

    return fDefenseWidth;
}

float Locations::fieldGoalDepth() {
    _mutex.lockForRead();
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
        return (pos.x() <= _leftAreaLeftCorner.x()
                && pos.y() <= _leftAreaRightPost.y() && pos.y() >= _leftAreaLeftPost.y());
    }else {
        return (pos.x() >= _rightAreaRightCorner.x()
                && pos.y() <= _rightAreaLeftPost.y() && pos.y() >= _rightAreaRightPost.y());
    }
}

bool Locations::isInsideTheirArea(const Position &pos) {
    if(theirSide().isLeft()) {
        return (pos.x() <= _leftAreaLeftCorner.x()
                && pos.y() <= _leftAreaRightPost.y() && pos.y() >= _leftAreaLeftPost.y());
    }else {
        return (pos.x() >= _rightAreaRightCorner.x()
                && pos.y() <= _rightAreaLeftPost.y() && pos.y() >= _rightAreaRightPost.y());
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

Position Locations::ourGoalRightMidPost() {
    Position ourGoalRightMidPost;
    _mutex.lockForRead();
    if(ourSide().isRight()) {
        ourGoalRightMidPost = _rightGoalRightMidPost;
    }else {
        ourGoalRightMidPost = _leftGoalRightMidPost;
    }
    _mutex.unlock();
    return ourGoalRightMidPost;
}

Position Locations::ourGoalLeftMidPost() {
    Position ourGoalLeftMidPost;
    _mutex.lockForRead();
    if(ourSide().isRight()) {
        ourGoalLeftMidPost = _rightGoalLeftMidPost;
    }else {
        ourGoalLeftMidPost = _leftGoalLeftMidPost;
    }
    _mutex.unlock();
    return ourGoalLeftMidPost;
}

Position Locations::theirGoalRightMidPost() {
    Position theirGoalRightMidPost;
    _mutex.lockForRead();
    if(theirSide().isRight()) {
        theirGoalRightMidPost = _rightGoalRightMidPost;
    }else {
        theirGoalRightMidPost = _leftGoalRightMidPost;
    }
    _mutex.unlock();
    return theirGoalRightMidPost;
}

Position Locations::theirGoalLeftMidPost() {
    Position theirGoalLeftMidPost;
    _mutex.lockForRead();
    if(theirSide().isRight()) {
        theirGoalLeftMidPost = _rightGoalLeftMidPost;
    }else {
        theirGoalLeftMidPost = _leftGoalLeftMidPost;
    }
    _mutex.unlock();
    return theirGoalLeftMidPost;
}

Position Locations::ourAreaLeftPost() {
    Position ourAreaLeftPost;
    _mutex.lockForRead();
    if(theirSide().isRight()) {
        ourAreaLeftPost = _leftAreaLeftPost;
    }else {
        ourAreaLeftPost = _rightAreaLeftPost;
    }
    _mutex.unlock();
    return ourAreaLeftPost;
}

Position Locations::ourAreaRightPost() {
    Position ourAreaRightPost;
    _mutex.lockForRead();
    if(theirSide().isRight()) {
        ourAreaRightPost = _leftAreaRightPost;
    }else {
        ourAreaRightPost = _rightAreaRightPost;
    }
    _mutex.unlock();
    return ourAreaRightPost;
}

Position Locations::theirAreaRightPost() {
    Position theirAreaRightPost;
    _mutex.lockForRead();
    if(theirSide().isRight()) {
        theirAreaRightPost = _rightAreaRightPost;
    }else {
        theirAreaRightPost = _leftAreaRightPost;
    }
    _mutex.unlock();
    return theirAreaRightPost;
}

Position Locations::theirAreaLeftPost() {
    Position theirAreaLeftPost;
    _mutex.lockForRead();
    if(theirSide().isRight()) {
        theirAreaLeftPost = _rightAreaLeftPost;
    }else {
        theirAreaLeftPost = _leftAreaLeftPost;
    }
    _mutex.unlock();
    return theirAreaLeftPost;
}

Position Locations::ourAreaLeftCorner() {
    Position ourAreaLeftCorner;
    _mutex.lockForRead();
    if(theirSide().isRight()) {
        ourAreaLeftCorner = _leftAreaLeftCorner;
    }else {
        ourAreaLeftCorner = _rightAreaLeftCorner;
    }
    _mutex.unlock();
    return ourAreaLeftCorner;
}

Position Locations::ourAreaRightCorner() {
    Position ourAreaRightCorner;
    _mutex.lockForRead();
    if(theirSide().isRight()) {
        ourAreaRightCorner = _leftAreaRightCorner;
    }else {
        ourAreaRightCorner = _rightAreaRightCorner;
    }
    _mutex.unlock();
    return ourAreaRightCorner;
}

Position Locations::theirAreaRightCorner() {
    Position theirAreaRightCorner;
    _mutex.lockForRead();
    if(theirSide().isRight()) {
        theirAreaRightCorner = _rightAreaRightCorner;
    }else {
        theirAreaRightCorner = _leftAreaRightCorner;
    }
    _mutex.unlock();
    return theirAreaRightCorner;
}

Position Locations::theirAreaLeftCorner() {
    Position theirAreaLeftCorner;
    _mutex.lockForRead();
    if(theirSide().isRight()) {
        theirAreaLeftCorner = _rightAreaLeftCorner;
    }else {
        theirAreaLeftCorner = _leftAreaLeftCorner;
    }
    _mutex.unlock();
    return theirAreaLeftCorner;
}

float Locations::defenseAreaWidth() {
    float width;
    _mutex.lockForRead();
    width = _defenseAreaWidth;
    _mutex.unlock();
    return width;
}

float Locations::defenseAreaLength() {
    float length;
    _mutex.lockForRead();
    length = _defenseAreaLength;
    _mutex.unlock();
    return length;
}

void Locations::updateGeometryData(fira_message::Field geometryData) {
    _mutex.lockForWrite();

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
    _leftGoalLeftPost = Position(true, (-field.length()/2.0)*MM2METER, (-field.goal_width()/2.0)*MM2METER);
    _leftGoalRightPost = Position(true, (-field.length()/2.0)*MM2METER, (field.goal_width()/2.0)*MM2METER);
    _rightGoalLeftPost = Position(true, (field.length()/2.0)*MM2METER, (field.goal_width()/2.0)*MM2METER);
    _rightGoalRightPost = Position(true, (field.length()/2.0)*MM2METER, (-field.goal_width()/2.0)*MM2METER);
    _rightGoalRightMidPost = Position(true, (field.length()/2.0)*MM2METER, (-areaLength/2.0f)*MM2METER);
    _rightGoalLeftMidPost = Position(true, (field.length()/2.0)*MM2METER, (areaLength/2.0f)*MM2METER);
    _leftGoalRightMidPost = Position(true, (-field.length()/2.0)*MM2METER, (areaLength/2.0f)*MM2METER);
    _leftGoalLeftMidPost = Position(true, (-field.length()/2.0)*MM2METER, (-areaLength/2.0f)*MM2METER);
    _fieldCenterRadius = centerRadius*MM2METER;
    _goalLength = areaLength*MM2METER;
    _goalWidth = areaWidth*MM2METER;
    _goalDepth = field.goal_depth()*MM2METER;

    _mutex.unlock();
}
