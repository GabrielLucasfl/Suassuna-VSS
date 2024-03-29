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

#ifndef SKILL_H
#define SKILL_H

#include <src/entities/coach/basecoach.h>
#include <src/constants/constants.h>
#include <src/entities/coach/player/player.h>
#include <src/utils/utils.h>

class Skill
{
public:
    Skill();
    virtual ~Skill();

    // Skill name (for debug)
    virtual QString name() = 0;

    // Init skill control
    bool isInitialized();
    void initialize(Constants *constants);
    void setPlayer(Player *player);

    // Method to run in behavior
    void runSkill();

protected:
    // Player and constants getters
    Player *player();
    Constants *getConstants();

private:
    // Virtual implementation in inherited classes
    virtual void configure() = 0;
    virtual void run() = 0;

    // Player access
    Player *_player;

    // Constants
    Constants *_constants;

    // Init control
    bool _initialized;
};

#endif // SKILL_H
