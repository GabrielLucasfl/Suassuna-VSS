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

#ifndef LOCATIONS_H
#define LOCATIONS_H

#include <QReadWriteLock>
#include <QList>

#include <math.h>
#include <src/utils/types/wall/wall.h>
#include <src/utils/types/field/field.h>
#include <src/utils/types/position/position.h>
#include <src/utils/types/fieldside/fieldside.h>
#include <include/packet.pb.h>
#include <src/constants/constants.h>

class Locations
{
public:
    Locations(FieldSide ourSide, Field *field, Constants *constants);
    ~Locations();

    // Side
    FieldSide ourSide();
    FieldSide theirSide();

    // Walls
    QList<Wall> getWalls();

    // Field
    Position fieldCenter();
    Position fieldRightTopCorner();
    Position fieldRightBottomCorner();
    Position fieldLeftTopCorner();
    Position fieldLeftBottomCorner();

    float fieldMinX();
    float fieldMaxX();
    float fieldMinY();
    float fieldMaxY();
    float fieldLength();
    float fieldWidth();
    float fieldCenterRadius();
    float fieldDefenseLength();
    float fieldDefenseWidth();
    float fieldGoalDepth();
    float defenseAreaLength();
    float defenseAreaWidth();

    // Our
    Position ourFieldTopCorner();
    Position ourFieldBottomCorner();
    Position ourGoal();
    Position ourGoalRightPost();
    Position ourGoalLeftPost();
    Position ourPenaltyMark();
    Position ourAreaLeftBackCorner();
    Position ourAreaRightBackCorner();
    Position ourAreaLeftFrontCorner();
    Position ourAreaRightFrontCorner();

    // Their
    Position theirFieldTopCorner();
    Position theirFieldBottomCorner();
    Position theirGoal();
    Position theirGoalRightPost();
    Position theirGoalLeftPost();
    Position theirPenaltyMark();
    Position theirAreaLeftBackCorner();
    Position theirAreaRightBackCorner();
    Position theirAreaLeftFrontCorner();
    Position theirAreaRightFrontCorner();

    // Auxiliar functions
    bool isInsideOurField(const Position &pos);
    bool isInsideTheirField(const Position &pos);
    bool isInsideOurArea(const Position &pos);
    bool isInsideTheirArea(const Position &pos);
    bool isOutsideField(const Position &pos, float factor = 1.0);
    bool isOutsideField(const Position &pos, float dx, float dy);
    bool isInsideField(const Position &pos, float factor = 1.0);
    bool isInsideField(const Position &pos, float dx, float dy);

    // Update geometry
    void updateGeometryData(fira_message::Field geometryData);

private:    
    // Constants
    Constants *_constants;
    Constants* getConstants();

    // Field
    Field *_field;

    // FieldSide
    FieldSide _ourSide;

    // Geometry data
    fira_message::Field _lastData;

    // Walls
    QList<Wall> _walls;

    // Field data
    Position _fieldCenter;
    Position _fieldTopRightCorner;
    Position _fieldTopLeftCorner;
    Position _fieldBottomLeftCorner;
    Position _fieldBottomRightCorner;
    Position _rightPenaltyMark;
    Position _leftPenaltyMark;
    Position _rightGoalLeftPost;
    Position _rightGoalRightPost;
    Position _leftGoalLeftPost;
    Position _leftGoalRightPost;
    Position _leftAreaLeftBackCorner;
    Position _leftAreaRightBackCorner;
    Position _leftAreaLeftFrontCorner;
    Position _leftAreaRightFrontCorner;
    Position _rightAreaLeftBackCorner;
    Position _rightAreaRightBackCorner;
    Position _rightAreaLeftFrontCorner;
    Position _rightAreaRightFrontCorner;
    float _defenseAreaWidth;
    float _defenseAreaLength;
    float _fieldCenterRadius;
    float _goalLength;
    float _goalWidth;
    float _goalDepth;

    // Internal
    bool _isOutsideField(const Position &pos, const float maxX, const float maxY);

    // Mutex for data management
    QMutex _mutex;
};

#endif // LOCATIONS_H
