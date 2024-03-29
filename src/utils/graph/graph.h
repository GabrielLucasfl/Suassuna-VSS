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

#ifndef GRAPH_H
#define GRAPH_H

#include <QList>
#include <QLinkedList>
#include <QHash>
#include <QMap>
#include <src/utils/graph/vertex.h>
#include <src/utils/graph/edge.h>

class Graph {
public:
    Graph();
    ~Graph();

    // Modifiers
    void addVertex(Vertex *vertex);
    void connect(Vertex *v1, Vertex *v2, float weight);

    // Dijkstra algorithm: finds the shortest path between source and dest
    QLinkedList<Vertex *> getShortestPath(Vertex *source, Vertex *dest);

private:
    QList<Vertex*> _vertices;
    QLinkedList<Edge*> _edges;
};

#endif // GRAPH_H
