#ifndef WALL_H
#define WALL_H

#include <src/utils/types/position/position.h>

class Wall {
public:
    Wall(Position firstPosition, Position secondPosition);

    // Setters
    void setPositions(Position firstPostiion, Position secondPosition);

    // Getters
    Position getProjectionAtWall(Position position);
    float getDistanceToWall(Position position);

private:
    Position _firstPosition;
    Position _secondPosition;
};

#endif // WALL_H
