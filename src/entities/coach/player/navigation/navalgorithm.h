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

#ifndef NAVIGATIONALGORITHM_H
#define NAVIGATIONALGORITHM_H

#include <QMutex>
#include <QLinkedList>
#include <src/constants/constants.h>
#include <src/utils/types/angle/angle.h>
#include <src/utils/types/position/position.h>
#include <src/utils/timer/timer.h>
#include <src/entities/world/locations/locations.h>
#include <src/utils/utils.h>

class NavigationAlgorithm {
public:
    NavigationAlgorithm();
    NavigationAlgorithm(const NavigationAlgorithm &copy);
    virtual ~NavigationAlgorithm();

    virtual QString name();

    // Access to Navigation
    void initialize(Locations *loc, Constants *mrcconstants);
    void runNavigationAlgorithm();

    // Reset algorithm
    virtual void reset() = 0;

    // Set origin and goal
    void setOrigin(const Position &pos, const Angle &ori);
    void setGoal(const Position &pos, const Angle &ori);

    // Add obstacles
    virtual void addBall(const Position &pos) = 0;
    virtual void addGoalArea(const Position &pos) = 0;
    virtual void addOwnRobot(const Position &pos) = 0;
    virtual void addEnemyRobot(const Position &pos) = 0;

    // Return results
    virtual Angle getDirection() const = 0;
    virtual float getDistance() const;
    virtual QLinkedList<Position> getPath() const;

protected:
    Locations* loc() { return _loc; }
    Constants * getConstants();

    // Origin access
    Position originPos() const { return _originPos; }
    Angle originOri() const { return _originOri; }

    // Goal access
    Position goalPos() const { return _goalPos; }
    Angle goalOri() const { return _goalOri; }

private:
    // Execute algorithm
    virtual void run() = 0;

    // Copy
    virtual NavigationAlgorithm* clone() const;

    // Locations access
    Locations *_loc;
    Constants *_mrcconstants;

    // NavigationAlgorithm positions
    // Origin
    Position _originPos;
    Angle _originOri;
    // Goal
    Position _goalPos;
    Angle _goalOri;

    // Path generation
    void generatePath();
    bool _generatePath;
    QLinkedList<Position> _path;
    mutable QMutex _pathMutex;

    // Distance
    float _distance;
};

#endif // NAVIGATIONALGORITHM_H
