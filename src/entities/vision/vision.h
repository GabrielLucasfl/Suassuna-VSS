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

#ifndef VISION_H
#define VISION_H

#include <QUdpSocket>

#include <src/entities/entity.h>
#include <src/constants/constants.h>
#include <src/utils/types/color/color.h>
#include <src/utils/types/object/object.h>
#include <src/utils/types/ballobject/ballobject.h>
#include <include/packet.pb.h>

class Vision : public Entity
{
    Q_OBJECT
public:
    Vision(Constants *constants);
    ~Vision();

    QString name();

private:
    // Entity inherited methods
    void initialization();
    void loop();
    void finalization();

    // Constants
    Constants *_constants;
    Constants* getConstants();

    // Socket for receive vision data
    QUdpSocket *_visionClient;
    void bindAndConnect();

    // Network
    QString _visionAddress;
    quint16 _visionPort;

    // Objects control
    QMap<Colors::Color, QMap<quint8, Object*>*> _playerObjects;
    BallObject _ballObject;
    void updatePlayer(Colors::Color teamColor, quint8 playerId, fira_message::Robot player);
    void updateBall(fira_message::Ball ball);

    // Vision reached control
    bool *_blueControl;
    bool *_yellowControl;
    bool _ballControl;
    void clearControls();

signals:
    void updateVision();
    void sendPlayer(Colors::Color teamColor, quint8 playerId, Object playerObject);
    void sendBall(BallObject ballObject);
    void sendGeometryData(fira_message::Field geometData);
};

#endif // VISION_H
