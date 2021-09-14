#ifndef PLAYBOOK_FLEX_H
#define PLAYBOOK_FLEX_H

#include <src/entities/coach/playbook/playbook.h>
#include <src/entities/coach/role/roles.h>
#include <src/utils/types/playerstate/playerstate.h>

class Playbook_Flex : public Playbook
{
public:
    Playbook_Flex();
    QString name();

private:
    QList<float> punctuations;
    void definePunctuation();
    // Playbook inherited methods
    void configure(int numPlayers);
    void run(int numPlayers);

    // Role pointer vector
    //Role_Default *_rl_default;
    Role_Goalkeeper *_role_gk;
    QList<Role_Defender*> _roles_def;
    //Role_TMP_DEF *_rl_tmp;
    QList<Role_Supporter*> _roles_sup;
    QList<Role_Attacker*> _roles_att;

    // Internal functions
    void switchPlayersIDs();
    void selectInitialIDs();
    void thirdPlayerState();
    bool isBehindBallXcoord(Position pos);
    bool isBallInsideDefenderEllipse(float ellipseA, float ellipseB);
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
    Timer _switchPlayersTimer;
    Timer _replaceSecRoleTimer;
    bool _switchedPlayers;
    bool _replacedSecRole;
    QHash<quint8, PlayerState*> playersState;
};


#endif // PLAYBOOK_FLEX_H
