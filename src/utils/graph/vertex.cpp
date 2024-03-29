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

#include "vertex.h"

int Vertex::_vertexId = 0;

Vertex::Vertex(Position pos) {
    _position = pos;
    Vertex::_vertexId++;
    _id = Vertex::_vertexId;
}

void Vertex::addEdge(Edge *edge) {
    // Add edge
    _edges.push_back(edge);
}

bool Vertex::connectedTo(Vertex *vertex) const {
    // Search for edge
    for(int i=0; i<_edges.size(); i++) {
        Edge *edge = _edges.at(i);

        // Return true if conected
        if(edge->getVertex1()==vertex || edge->getVertex2()==vertex)
            return true;
    }

    // False otherwise
    return false;
}

