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

#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <QReadWriteLock>
#include <src/entities/entity.h>
#include <src/constants/constants.h>

#define QT_TEAMS 2

class Actuator : public Entity
{
    Q_OBJECT
public:
    Actuator(Constants *constants);
    ~Actuator();

protected:
    // Internal
    typedef struct {
        int isYellow;
        int playerId;
        float vx, vw;
        float kickPower;
        bool dribbling;
        bool isUpdated;
    } robotData;
    robotData **_robotData;
    QReadWriteLock _dataMutex;

protected:
    Constants* getConstants();

private:
    // Entity inherited methods
    virtual void initialization() = 0;
    virtual void loop() = 0;
    virtual void finalization() = 0;

    // Constants
    Constants *_constants;

public slots:
    // Internal
    virtual void setLinearSpeed(int teamId, int playerId, float vx) = 0;
    virtual void setAngularSpeed(int teamId, int playerId, float vw) = 0;
    virtual void dribble(int teamId, int playerId, bool enable) = 0;
    virtual void kick(int teamId, int playerId, float power) = 0;
};

#endif // ACTUATOR_H
