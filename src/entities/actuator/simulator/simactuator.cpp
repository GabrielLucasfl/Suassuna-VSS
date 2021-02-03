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
    for(int i = 0; i < QT_TEAMS; i++) {
        for(int j = 0; j < getConstants()->qtPlayers(); j++) {
            _dataMutex.lockForRead();
            if(!_robotData[i][j].isUpdated) {
                sendData(_robotData[i][j]);
                _robotData[i][j].isUpdated = true;
            }
            _dataMutex.unlock();
        }
    }
}

void SimActuator::finalization() {
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

void SimActuator::sendData(robotData data) {
    // Creating packet
    fira_message::sim_to_ref::Packet packet;
    fira_message::sim_to_ref::Command *command = packet.mutable_cmd()->add_robot_commands();

    // Setting macro informations (team and timestamp)
    command->set_id(data.playerId);
    command->set_yellowteam(data.isYellow);

    // Setting whells speed
    double L = 0.075;
    double r = 0.0325;
    double wl = ((2 * data.vx) - (L * data.vw)) / (2 * r);
    double wr = ((2 * data.vx) + (L * data.vw)) / (2 * r);

    command->set_wheel_left(wl);
    command->set_wheel_right(wr);

    // Sending data to simulator
    std::string buffer;
    packet.SerializeToString(&buffer);
    if(_actuatorClient->write(buffer.c_str(), buffer.length()) == -1) {
        std::cout << Text::cyan("[ACTUATOR] " , true) << Text::red("Failed to write to socket: ", true) << Text::red(_actuatorClient->errorString().toStdString(), true) + '\n';
    }
}

void SimActuator::setLinearSpeed(int teamId, int playerId, float vx) {
    _dataMutex.lockForWrite();
    _robotData[teamId][playerId].vx = vx;
    _robotData[teamId][playerId].isUpdated = false;
    _dataMutex.unlock();
}

void SimActuator::setAngularSpeed(int teamId, int playerId, float vw) {
    _dataMutex.lockForWrite();
    _robotData[teamId][playerId].vw = vw;
    _robotData[teamId][playerId].isUpdated = false;
    _dataMutex.unlock();
}

void SimActuator::dribble(int teamId, int playerId, bool enable) {
    _dataMutex.lockForWrite();
    _robotData[teamId][playerId].dribbling = enable;
    _robotData[teamId][playerId].isUpdated = false;
    _dataMutex.unlock();
}

void SimActuator::kick(int teamId, int playerId, float power) {
    _dataMutex.lockForWrite();
    _robotData[teamId][playerId].kickPower = power;
    _robotData[teamId][playerId].isUpdated = false;
    _dataMutex.unlock();
}
