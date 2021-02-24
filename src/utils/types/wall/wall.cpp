#include "wall.h"

#include <src/utils/utils.h>

Wall::Wall(Position firstPostiion, Position secondPosition) {
    setPositions(firstPostiion, secondPosition);
}

void Wall::setPositions(Position firstPostiion, Position secondPosition) {
    _firstPosition = firstPostiion;
    _secondPosition = secondPosition;
}

Position Wall::getProjectionAtWall(Position position) {
    return Utils::projectPointAtLine(_firstPosition, _secondPosition, position);
}

float Wall::getDistanceToWall(Position position) {
    return Utils::distanceToLine(_firstPosition, _secondPosition, position);
}
