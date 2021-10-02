#include "coordinator_vss.h"

Coordinator_VSS::Coordinator_VSS() {
}

QString Coordinator_VSS::name() {
    return "Coordinator_VSS";
}

void Coordinator_VSS::configure() {
    // Pb default => go to opposite goal and align
    addPlaybook(PLAYBOOK_DEFAULT, _playbook_default = new Playbook_Default());
    addPlaybook(PLAYBOOK_FLEX, _playbook_flex = new Playbook_Flex());
}

void Coordinator_VSS::run() {
    if (getConstants()->playbookFormation() == _playbook_default->name()) {
        setPlaybook(PLAYBOOK_DEFAULT);
    } else {
        setPlaybook(PLAYBOOK_FLEX);
    }
}
