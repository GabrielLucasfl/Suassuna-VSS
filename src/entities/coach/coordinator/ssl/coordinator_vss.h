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
        PLAYBOOK_FLEX
    };

    // Playbook pointers
    Playbook_Default *_playbook_default;
    Playbook_Flex *_playbook_flex;
};

#endif // COORDINATOR_VSS_H
