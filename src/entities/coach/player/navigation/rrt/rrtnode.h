/***
 * Warthog Robotics
 * University of Sao Paulo (USP) at Sao Carlos
 * http://www.warthog.sc.usp.br/
 *
 * This file is part of WRCoach project.
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

#ifndef NODE_H
#define NODE_H

#include <src/utils/types/position/position.h>
#include <cstdlib>

class RRTNode {
public:
    RRTNode();
    RRTNode(Position pos);

    // Setters
    void setNodeL (RRTNode* node) { _nodeL  = node; }
    void setNodeR (RRTNode* node) { _nodeR  = node; }
    void setNodeNN(RRTNode* node) { _nodeNN = node; }
    void setPosition(Position pos) { _pos = pos; }
    void setCompareByX(bool compareByX) { _compareByX = compareByX; }

    // Getters
    RRTNode* getNodeL()  { return _nodeL; }
    RRTNode* getNodeR()  { return _nodeR; }
    RRTNode* getNodeNN() { return _nodeNN; }
    Position getPosition() { return _pos; }
    bool getCompareByX() { return _compareByX; }

private:
    // Bool for control if this node will position it's leaves compare x or y
    bool _compareByX;

    // Pointers for left, rigth and nearest leaf
    RRTNode* _nodeL;
    RRTNode* _nodeR;
    RRTNode* _nodeNN;

    // Position stored
    Position _pos;
};

#endif // NODE_H
