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

#ifndef VERTEX_H
#define VERTEX_H

#include <QList>
#include <src/utils/types/position/position.h>
#include <src/utils/graph/graphbase.h>

class Vertex {
public:
    Vertex(Position pos);

    // Add edge
    void addEdge(Edge *edge);

    // Getters
    int id() const { return _id; }
    int numEdges() const { return _edges.size(); }
    QList<Edge*>* edges() { return &_edges; }
    bool connectedTo(Vertex *vertex) const;
    Position getPosition() const { return _position; }

private:
    QList<Edge*> _edges;
    int _id;
    Position _position;

    // Global vertex id
    static int _vertexId;
};

#endif // VERTEX_H
