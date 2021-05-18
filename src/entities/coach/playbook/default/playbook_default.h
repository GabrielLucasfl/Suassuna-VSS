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
    Role_Supporter *_rl_sup;
    Role_Attacker *_rl_atk;

    // Internal functions
    void switchPlayersIDs();
    void selectInitialIDs();
    bool isDefenderSituation();
    bool isBehindBallXcoord(Position pos);

    // Parameters
    quint8 _attackerID;
    quint8 _goalkeeperID;
    quint8 _lastID;

    // Initial
    bool _first;

    // Switch players parameters
    Timer _switchPlayersTimer;
    bool _switchedPlayers;
    Timer _atkStuckTimer;
    bool _atkStuck;
};

#endif // PLAYBOOK_DEFAULT_H
