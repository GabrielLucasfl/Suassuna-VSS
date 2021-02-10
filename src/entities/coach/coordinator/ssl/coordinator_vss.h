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
        PLAYBOOK_DEFAULT
    };

    // Playbook pointers
    Playbook_Default *_playbook_default;
};

#endif // COORDINATOR_VSS_H
