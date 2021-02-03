#ifndef COORDINATOR_VSS_H
#define COORDINATOR_VSS_H

#include <src/entities/coach/coordinator/coordinator.h>

class Coordinator_VSS : public Coordinator
{
public:
    Coordinator_VSS();
    QString name();

private:
    // Coordinator inherited methods
    void configure();
    void run();

    // Playbook enums
    enum {
        PLAYBOOK_DEFAULT,
        PLAYBOOK_DEFAULT2
    };

    // Playbook pointers
    Playbook_Default *_playbook_default;
    Playbook_Default2 *_playbook_default2;

    // Timer
    Timer timer;

    // Management
    int actualState;
};

#endif // COORDINATOR_VSS_H
