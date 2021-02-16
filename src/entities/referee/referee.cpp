#include "referee.h"

#include <QNetworkDatagram>

Referee::Referee(Constants *constants, WorldMap *worldMap) : Entity(ENT_REFEREE) {
    // Setting constants
    _constants = constants;

    // Setting worldmap
    _worldMap = worldMap;

    // Taking network data
    _refereeAddress = getConstants()->refereeAddress();
    _refereePort = getConstants()->refereePort();
    _replacerAddress = getConstants()->replacerAddress();
    _replacerPort = getConstants()->replacerPort();

    // Set initial state
    _lastFoul = VSSRef::Foul::GAME_ON; // VSSRef::Foul::GAME_ON => start game WITHOUT an open referee
                                    // VSSRef::Foul::HALT => start game WITH an open referee
    _forTeam = VSSRef::Color::NONE;
    _atQuadrant = VSSRef::Quadrant::NO_QUADRANT;
}

Referee::~Referee() {
    _placementData.clear();
}

bool Referee::isGameOn() {
    return (_lastFoul == VSSRef::GAME_ON);
}

void Referee::initialization() {
    // Start clients
    startClients();
}

void Referee::loop() {
    while(_refereeClient->hasPendingDatagrams()) {
        QNetworkDatagram datagram;
        VSSRef::ref_to_team::VSSRef_Command command;

        // Receiving datagram
        datagram = _refereeClient->receiveDatagram();

        // Parse to protobuf
        if(command.ParseFromArray(datagram.data().data(), datagram.data().size()) == false) {
            std::cout << Text::cyan("[REFEREE]", true) + Text::bold("Failed to parse datagram into protobuf.") + '\n';
        }

        // Parse foul data
        _lastFoul = command.foul();
        _forTeam = command.teamcolor();
        _atQuadrant = command.foulquadrant();

        // Debug (test)
        std::cout << Text::cyan("[REFEREE] ", true) + Text::bold("Received foul '" + VSSRef::Foul_Name(_lastFoul) + "' for team: '" + VSSRef::Color_Name(_forTeam) + "' at quadrant: '" + VSSRef::Quadrant_Name(_atQuadrant) + "'") + '\n';

        // Avoid to emit signal in control fouls
        if(_lastFoul != VSSRef::Foul::HALT && _lastFoul != VSSRef::Foul::GAME_ON && _lastFoul != VSSRef::Foul::STOP) {
            emit sendFoul(_lastFoul, _forTeam, _atQuadrant);
        }
    }
}

void Referee::finalization() {
    // Close and delete clients
    closeAndDeleteClients();
}

void Referee::startClients() {
    // Connecting referee
    // Creating pointer
    _refereeClient = new QUdpSocket();

    // Binding
    if(_refereeClient->bind(QHostAddress(_refereeAddress), _refereePort, QUdpSocket::ShareAddress) == false) {
        std::cout << Text::cyan("[REFEREE]", true) + Text::red("Failed to bind client.", true) + '\n';
        exit(-1);
    }

    // Joining to multicast
    if(_refereeClient->joinMulticastGroup(QHostAddress(_refereeAddress)) == false) {
        std::cout << Text::cyan("[REFEREE]", true) + Text::red("Failed to join multicast.", true) + '\n';
        exit(-1);
    }

    // Debug referee network
    std::cout << Text::cyan("[REFEREE] ", true) + Text::bold(QString("Started with address '%1' and port '%2'.").arg(_refereeAddress).arg(_refereePort).toStdString()) + '\n';

    // Connecting replacer
    // Creating pointer
    _replacerClient = new QUdpSocket();

    // Connecting to host
    _replacerClient->connectToHost(_replacerAddress, _replacerPort, QIODevice::WriteOnly, QAbstractSocket::IPv4Protocol);

    // Debug replacer network
    std::cout << Text::cyan("[REPLACER] ", true) + Text::bold(QString("Started with address '%1' and port '%2'.").arg(_replacerAddress).arg(_replacerPort).toStdString()) + '\n';
}

void Referee::closeAndDeleteClients() {
    // Closing Referee socket
    if(_refereeClient->isOpen()) {
        _refereeClient->close();
    }
    delete _refereeClient;
    std::cout << Text::cyan("[REFEREE] ", true) + Text::bold("Module closed.") + '\n';

    // Closing Replacer socket
    if(_replacerClient->isOpen()) {
        _replacerClient->close();
    }
    delete _replacerClient;
    std::cout << Text::cyan("[REPLACER] ", true) + Text::bold("Module closed.") + '\n';
}

void Referee::placeRobots() {
    // Create command
    VSSRef::team_to_ref::VSSRef_Placement placementCommand;

    // Create frame
    VSSRef::Frame *placementFrame = new VSSRef::Frame();

    // Set frame team color
    placementFrame->set_teamcolor((getConstants()->teamColor() == Colors::BLUE) ? VSSRef::Color::BLUE : VSSRef::Color::YELLOW);

    // Fill with robots
    QList<quint8> placedPlayers = _placementData.keys();
    for(int i = 0; i < placedPlayers.size(); i++) {
        // Create robot
        VSSRef::Robot *robot = placementFrame->add_robots();

        // Take hash data
        Position desiredPosition = _placementData.value(placedPlayers.at(i)).first;
        Angle desiredOrientation = _placementData.value(placedPlayers.at(i)).second;

        // Fill data
        robot->set_robot_id(placedPlayers.at(i));
        robot->set_x(desiredPosition.x());
        robot->set_y(desiredPosition.y());
        robot->set_orientation(desiredOrientation.value() * (180 / 3.14)); // convert to deg
    }

    // Set frame to command
    placementCommand.set_allocated_world(placementFrame);

    // Parse command to buffer
    std::string buffer;
    placementCommand.SerializeToString(&buffer);

    // Send to socket
    if(_replacerClient->write(buffer.c_str(), static_cast<qint64>(buffer.length())) == -1) {
        std::cout << Text::cyan("[REPLACER]", true) + Text::red("Failed to send frame data.", true) + '\n';
    }

    // Clear hash
    _placementData.clear();
}

void Referee::receivePlacement(quint8 playerId, Position desiredPosition, Angle desiredOrientation) {
    // Save placement data into hash
    _placementData.insert(playerId, QPair<Position, Angle>(desiredPosition, desiredOrientation));

    // Check if qt of keys inserted (num of players placed) is equal to available players in field
    if(_placementData.keys().size() == getWorldMap()->getAvailablePlayers(getConstants()->teamColor()).size()) {
        // Place robots (everyone placed)
        placeRobots();
    }
}

Constants* Referee::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("Constants with nullptr value at Referee") + '\n';
    }
    else {
        return _constants;
    }

    return nullptr;
}

WorldMap* Referee::getWorldMap() {
    if(_worldMap == nullptr) {
        std::cout << Text::red("[ERROR] ", true) << Text::bold("WorldMap with nullptr value at Referee") + '\n';
    }
    else {
        return _worldMap;
    }

    return nullptr;
}
