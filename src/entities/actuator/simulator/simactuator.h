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

#ifndef SIMACTUATOR_H
#define SIMACTUATOR_H

#include <QUdpSocket>

#include <src/entities/actuator/actuator.h>
#include <src/constants/constants.h>
#include <include/packet.pb.h>
#include <include/command.pb.h>

class SimActuator : public Actuator
{
    Q_OBJECT
public:
    using Actuator::Actuator;

private:
    // Entity inherited methods
    void initialization();
    void loop();
    void finalization();

    // Network info
    QString _actuatorAddress;
    quint16 _actuatorPort;

    // Socket to send data to grSim
    QUdpSocket *_actuatorClient;
    void connectToNetwork();
    void finishConnection();
    void stopAllRobots();
    void sendData(robotData data);

public slots:
    // Internal inherited methods
    void setWheelsSpeed(quint8 playerId, float wheelLeft, float wheelRight);
};

#endif // SIMACTUATOR_H
