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

#ifndef PLAYBOOK_DEFAULT_H
#define PLAYBOOK_DEFAULT_H

#include <src/entities/coach/playbook/playbook.h>
#include <src/entities/coach/role/roles.h>
#include <src/utils/types/playerstate/playerstate.h>

class Playbook_Default : public Playbook
{
public:
    Playbook_Default();
    QString name();

private:
    // Playbook inherited methods
    void configure(int numPlayers);
    void run(int numPlayers);

    // Role pointer vector
    Role_Default *_rl_default;
    Role_Goalkeeper *_rl_gk;
    Role_Defender *_rl_df;
    //Role_TMP_DEF *_rl_tmp;
    Role_Supporter *_rl_sup;
    Role_Attacker *_rl_atk;

    // Internal functions
    void switchPlayersIDs();
    void selectInitialIDs();
    void thirdPlayerState();
    bool isBehindBallXcoord(Position pos);
    bool isBallInsideDefenderEllipse(std::pair<float, float> ellipseParameters);
    void setDefenderEllipse(Position center, std::pair<float, float> ellipseParameters);
    void updatePlayerStuck(quint8 id);
    float minDistPlayerObstacle(quint8 id);

    // Parameters
    quint8 _attackerID;
    quint8 _goalkeeperID;
    quint8 _lastID;
    bool _defenderState;

    // Initial
    bool _first = true;

    // Switch players/roles parameters
    QHash<quint8, PlayerState*> playersState;
    Timer _switchPlayersTimer;
    Timer _replaceSecRoleTimer;
    bool _switchedPlayers;
    bool _replacedSecRole;
};

#endif // PLAYBOOK_DEFAULT_H
