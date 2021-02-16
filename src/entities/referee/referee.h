#ifndef REFEREE_H
#define REFEREE_H

#include <QUdpSocket>

#include <src/utils/types/position/position.h>
#include <src/utils/types/angle/angle.h>
#include <include/vssref_placement.pb.h>
#include <include/vssref_command.pb.h>
#include <src/constants/constants.h>
#include <src/entities/entity.h>
#include <src/entities/world/worldmap.h>

class Referee : public Entity
{
    Q_OBJECT
public:
    Referee(Constants *constants, WorldMap *worldMap);
    ~Referee();

    QString name();
    // Getters
    bool isGameOn();

private:
    // Entity inherited methods
    void initialization();
    void loop();
    void finalization();

    // WorldMap
    WorldMap *_worldMap;
    WorldMap* getWorldMap();

    // Network
    QString _refereeAddress;
    quint16 _refereePort;
    QString _replacerAddress;
    quint16 _replacerPort;

    // Foul data
    VSSRef::Foul _lastFoul;
    VSSRef::Color _forTeam;
    VSSRef::Quadrant _atQuadrant;

    // Team placement data
    QHash<quint8, QPair<Position, Angle>> _placementData;
    void placeRobots();

    // Sockets
    QUdpSocket *_refereeClient;
    QUdpSocket *_replacerClient;
    void startClients();
    void closeAndDeleteClients();

    // Constants
    Constants *_constants;
    Constants* getConstants();

signals:
    void sendFoul(VSSRef::Foul foul, VSSRef::Color forTeam, VSSRef::Quadrant atQuadrant);

public slots:
    void receivePlacement(quint8 playerId, Position desiredPosition, Angle desiredOrientation);
};

#endif // REFEREE_H
