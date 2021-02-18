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

#include "simactuator.h"

#include <math.h>
#include <src/utils/text/text.h>

void SimActuator::initialization() {
    // Setting network data from constants and connecting
    _actuatorAddress = getConstants()->simActuatorAddress();
    _actuatorPort = getConstants()->simActuatorPort();
    connectToNetwork();

    std::cout << Text::cyan("[ACTUATOR] ", true) + Text::bold("Started at address '" + _actuatorAddress.toStdString() + "' and port '" + std::to_string(_actuatorPort) + "'.") + '\n';
}

void SimActuator::loop() {
    for(int i = 0; i < getConstants()->qtPlayers(); i++) {
        _dataMutex.lockForRead();
        robotData data = _robotsData[i];
        if(!data.isUpdated) {
            sendData(data);
            data.isUpdated = true;
        }
        _dataMutex.unlock();
    }
}

void SimActuator::finalization() {
    stopAllRobots();
    finishConnection();
    std::cout << Text::cyan("[ACTUATOR] " , true) + Text::bold("Client finished.") + '\n';
}

void SimActuator::connectToNetwork() {
    _actuatorClient = new QUdpSocket();

    if(_actuatorClient->isOpen()) {
        _actuatorClient->close();
    }

    _actuatorClient->connectToHost(_actuatorAddress, _actuatorPort, QIODevice::WriteOnly, QAbstractSocket::IPv4Protocol);
}

void SimActuator::finishConnection() {
    if(_actuatorClient->isOpen()) {
        _actuatorClient->close();
    }

    delete _actuatorClient;
}

void SimActuator::stopAllRobots() {
    // Init data again (set all speeds to 0)
    initData();

    // For each robot, send data
    for(int i = 0; i < getConstants()->qtPlayers(); i++) {
        robotData data = _robotsData[i];
        sendData(data);
    }

    // Delete data
    deleteData();
}

void SimActuator::sendData(robotData data) {
    // Creating packet
    fira_message::sim_to_ref::Packet packet;
    fira_message::sim_to_ref::Command *command = packet.mutable_cmd()->add_robot_commands();

    // Setting macro informations (team and timestamp)
    command->set_id(data.playerId);
    command->set_yellowteam(getConstants()->teamColor());

    // Set wheels speed
    command->set_wheel_left(data.wheelLeft);
    command->set_wheel_right(data.wheelRight);

    // Sending data to simulator
    std::string buffer;
    packet.SerializeToString(&buffer);
    if(_actuatorClient->write(buffer.c_str(), buffer.length()) == -1) {
        std::cout << Text::cyan("[ACTUATOR] " , true) << Text::red("Failed to write to socket: ", true) << Text::red(_actuatorClient->errorString().toStdString(), true) + '\n';
    }
}

void SimActuator::setWheelsSpeed(quint8 playerId, float wheelLeft, float wheelRight) {
    _dataMutex.lockForWrite();
    _robotsData[playerId].wheelLeft = wheelLeft;
    _robotsData[playerId].wheelRight = wheelRight;
    _robotsData[playerId].isUpdated = false;
    _dataMutex.unlock();
}
