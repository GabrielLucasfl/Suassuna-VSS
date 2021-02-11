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

#ifndef RRTTREE_H
#define RRTTREE_H

#include <QList>
#include <math.h>
#include <src/entities/coach/player/navigation/rrt/rrtnode.h>

class RRTTree {
public:
    RRTTree();
    ~RRTTree();

    // Methods
    void append(RRTNode *leaf);
    RRTNode* getNN(Position myPos);
    QList<Position> getPath();

private:
    RRTNode* _root;
    QList<RRTNode*> _tree;
    QList<RRTNode*> _stackOfNodes;

    // Method for optimize distance comparison
    float squareDistance(Position posA, Position posB) { return pow(posA.x()-posB.x(), 2) + pow(posA.y()-posB.y(), 2); }
};
#endif // RRTTREE_H
