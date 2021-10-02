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

#include "constants.h"

// Including vision filters to access their static functions
#include <src/entities/vision/filters/loss/lossfilter.h>
#include <src/entities/vision/filters/noise/noisefilter.h>

Constants::Constants(QString fileName) {
    _fileName = fileName;

    // Opening and reading file content in buffer
    _file.setFileName(_fileName);
    _file.open(QIODevice::ReadOnly | QIODevice::Text);
    _fileBuffer = _file.readAll();
    _file.close();

    // Parsing buffer to json objects
    _document = QJsonDocument::fromJson(_fileBuffer.toUtf8());
    _documentMap = _document.object().toVariantMap();

    // Read data
    readThreadConstants();
    readVisionConstants();
    readSimActuatorConstants();
    readRefereeConstants();
    readTeamConstants();
}

void Constants::readThreadConstants() {
    // Taking thread mapping in json
    QVariantMap threadMap = documentMap()["Thread"].toMap();

    // Filling vars
    _threadFrequency = threadMap["threadFrequency"].toInt();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded threadFrequency: " + std::to_string(_threadFrequency)) + '\n';
}

void Constants::readVisionConstants() {
    // Taking vision mapping in json
    QVariantMap visionMap = documentMap()["Vision"].toMap();

    // Filling vars
    _visionAddress = visionMap["visionAddress"].toString();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded visionAddress: " + _visionAddress.toStdString()) + '\n';

    _visionPort = visionMap["visionPort"].toUInt();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded visionPort: " + std::to_string(_visionPort)) + '\n';

    _lossFilterTime = visionMap["lossFilterTime"].toUInt();
    LossFilter::setLossTime(_lossFilterTime);
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded lossFilterTime: " + std::to_string(_lossFilterTime)) + '\n';

    _noiseFilterTime = visionMap["noiseFilterTime"].toUInt();
    NoiseFilter::setNoiseTime(_noiseFilterTime);
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded noiseFilterTime: " + std::to_string(_noiseFilterTime)) + '\n';

    _useGeometryFromCamera = visionMap["useGeometryFromCamera"].toBool();
    QString useGeomFromCameraStr = (_useGeometryFromCamera ? "true" : "false");
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded useGeometryFromCamera: " + useGeomFromCameraStr.toStdString()) + '\n';
}

void Constants::readSimActuatorConstants() {
    // Taking simActuator mapping in json
    QVariantMap actuatorMap = documentMap()["SimActuator"].toMap();

    // Filling vars
    _simActuatorAddress = actuatorMap["simActuatorAddress"].toString();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded simActuatorAddress: " + _simActuatorAddress.toStdString()) + '\n';

    _simActuatorPort = actuatorMap["simActuatorPort"].toUInt();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded simActuatorPort: " + std::to_string(_simActuatorPort)) + '\n';
}

void Constants::readRefereeConstants() {
    // Taking referee mapping in json
    QVariantMap refereeMap = documentMap()["Referee"].toMap();

    // Filling vars
    _refereeAddress = refereeMap["refereeAddress"].toString();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded refereeAddress: " + _refereeAddress.toStdString()) + '\n';

    _refereePort = refereeMap["refereePort"].toUInt();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded refereePort: " + std::to_string(_refereePort)) + '\n';

    _replacerAddress = refereeMap["replacerAddress"].toString();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded replacerAddress: " + _replacerAddress.toStdString()) + '\n';

    _replacerPort = refereeMap["replacerPort"].toUInt();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded replacerPort: " + std::to_string(_replacerPort)) + '\n';

}

void Constants::readTeamConstants() {
    // Taking team mapping in json
    QVariantMap teamMap = documentMap()["Team"].toMap();

    // Filling vars
    _teamColorName = teamMap["teamColor"].toString().toLower();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded teamColorName: " + _teamColorName.toStdString()) + '\n';

    QString teamSideAux = teamMap["teamSide"].toString().toLower();
    _teamSide = (teamSideAux == "left" ? FieldSide(Sides::SIDE::LEFT) : FieldSide(Sides::SIDE::RIGHT));
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded teamSide: " + teamSideAux.toStdString()) + '\n';

    _qtPlayers = teamMap["qtPlayers"].toUInt();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded qtPlayers: " + std::to_string(qtPlayers())) + '\n';

    _playbookFormation = teamMap["playbookFormation"].toString().toLower();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded playbook: " + _playbookFormation.toStdString()) + '\n';

    // No need to print the info below
    _predictionBaseCycles = teamMap["predictionBaseCycles"].toUInt();

    // Taking player mapping in json
    QVariantMap playerMap = teamMap["Player"].toMap();

    // Filling vars
    QList<QVariant> playerPIDList = playerMap["playerPID"].toList();
    _playerPID = std::tuple<float, float, float>(playerPIDList.at(0).toFloat(), playerPIDList.at(1).toFloat(), playerPIDList.at(2).toFloat());
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded playerPID: {"
                                                                  + std::to_string(playerPIDList.at(0).toFloat()) + ", "
                                                                  + std::to_string(playerPIDList.at(1).toFloat()) + ", "
                                                                  + std::to_string(playerPIDList.at(2).toFloat()) + "}\n");

    _playerBaseSpeed = playerMap["playerBaseSpeed"].toFloat();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded playerBaseSpeed: " + std::to_string(_playerBaseSpeed)) + '\n';

    _distToConsiderStuck = playerMap["distToConsiderStuck"].toFloat();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded distToConsiderStuck: " + std::to_string(_distToConsiderStuck)) + '\n';

    _timeToConsiderStuck = playerMap["timeToConsiderStuck"].toFloat();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded timeToConsiderStuck: " + std::to_string(_timeToConsiderStuck)) + '\n';

    _timeToWaitStuckMovement = playerMap["timeToWaitStuckMovement"].toFloat();
    std::cout << Text::purple("[CONSTANTS] ", true) << Text::bold("Loaded timeToWaitStuckMovement: " + std::to_string(_timeToWaitStuckMovement)) + '\n';
}

int Constants::threadFrequency() {
    return _threadFrequency;
}

QString Constants::visionAddress() {
    return _visionAddress;
}

quint16 Constants::visionPort() {
    return _visionPort;
}

void Constants::setManualVisionNetwork(QString visionNetwork) {
    QStringList arguments = visionNetwork.split(":");

    // Check if has sufficient arguments
    if(arguments.size() < 2) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold("Error while parsing manual vision network data.") + '\n';
        return;
    }

    // Parse data
    _visionAddress = arguments.at(0);
    _visionPort = arguments.at(1).toInt();

    // Debug
    std::cout << Text::blue("[MANUAL] ", true) + Text::bold("Parsed vision network: " + visionAddress().toStdString() + ":" + std::to_string(visionPort())) + '\n';
}

int Constants::lossFilterTime() {
    return _lossFilterTime;
}

int Constants::noiseFilterTime() {
    return _noiseFilterTime;
}

bool Constants::useGeometryFromCamera() {
    return _useGeometryFromCamera;
}

QString Constants::simActuatorAddress() {
    return _simActuatorAddress;
}

quint16 Constants::simActuatorPort() {
    return _simActuatorPort;
}

QString Constants::refereeAddress() {
    return _refereeAddress;
}

quint16 Constants::refereePort() {
    return _refereePort;
}

QString Constants::replacerAddress() {
    return _replacerAddress;
}

quint16 Constants::replacerPort() {
    return _replacerPort;
}

void Constants::setManualRefereeNetwork(QString refereeNetwork) {
    QStringList arguments = refereeNetwork.split(":");

    // Check if has sufficient arguments
    if(arguments.size() < 2) {
        std::cout << Text::red("[ERROR] ", true) + Text::bold("Error while parsing manual referee network data.") + '\n';
        return;
    }

    // Parse data
    _refereeAddress = arguments.at(0);
    _refereePort = arguments.at(1).toInt();

    // Debug
    std::cout << Text::blue("[MANUAL] ", true) + Text::bold("Parsed referee network: " + refereeAddress().toStdString() + ":" + std::to_string(refereePort())) + '\n';
}

QString Constants::teamColorName() {
    return _teamColorName;
}

Colors::Color Constants::teamColor() {
    return (_teamColorName == "blue" ? Colors::Color::BLUE : Colors::Color::YELLOW);
}

FieldSide Constants::teamSide() {
    _sideMutex.lock();
    FieldSide teamSide = _teamSide;
    _sideMutex.unlock();

    return teamSide;
}

int Constants::qtPlayers() {
    return _qtPlayers;
}

QString Constants::playbookFormation() {
    return _playbookFormation;
}

int Constants::predictionBaseCycles() {
    return _predictionBaseCycles;
}

std::tuple<float, float, float> Constants::playerPID() {
    return _playerPID;
}

float Constants::playerBaseSpeed() {
    return _playerBaseSpeed;
}

float Constants::distToConsiderStuck() {
    return _distToConsiderStuck;
}

float Constants::timeToConsiderStuck() {
    return _timeToConsiderStuck;
}

float Constants::timeToWaitStuckMovement() {
    return _timeToWaitStuckMovement;
}

void Constants::swapTeamSide() {
    _sideMutex.lock();
    _teamSide = FieldSide(_teamSide.oppositeSide());
    _sideMutex.unlock();
}

void Constants::setManualTeamColor(QString teamColorName) {
    if(teamColorName.toLower() == "blue" || teamColorName.toLower() == "yellow") {
        _teamColorName = teamColorName;
        std::cout << Text::blue("[MANUAL] ", true) + Text::bold("Parsed manual teamColor: " + teamColorName.toLower().toStdString()) + '\n';
    }
    else {
        std::cout << Text::red("[ERROR] ", true) + Text::bold("Failed to parse manual team color.") + '\n';
    }
}

void Constants::setManualTeamSide(QString teamSide) {
    if(teamSide.toLower() == "left" || teamSide.toLower() == "right") {
        if(teamSide.toLower() == "left") {
            _teamSide = FieldSide(Sides::LEFT);
        }
        else {
            _teamSide = FieldSide(Sides::RIGHT);
        }
        std::cout << Text::blue("[MANUAL] ", true) + Text::bold("Parsed manual teamSide: " + teamSide.toLower().toStdString()) + '\n';
    }
    else {
        std::cout << Text::red("[ERROR] ", true) + Text::bold("Failed to parse manual team side.") + '\n';
    }
}
