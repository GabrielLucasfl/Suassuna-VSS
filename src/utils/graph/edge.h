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

#ifndef EDGE_H
#define EDGE_H

#include <src/utils/graph/graphbase.h>

class Edge {
public:
    Edge(Vertex *v1, Vertex *v2, float weight);

    // Getters
    Vertex* getVertex1() const { return _v1; }
    Vertex* getVertex2() const { return _v2; }
    QList<Vertex*> getVerticesList();
    float getWeight() const { return _weight; }

private:
    Vertex *_v1;
    Vertex *_v2;
    float _weight;
};

#endif // EDGE_H
