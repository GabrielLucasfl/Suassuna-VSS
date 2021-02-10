#include "coordinator_vss.h"

Coordinator_VSS::Coordinator_VSS() {
}

QString Coordinator_VSS::name() {
    return "Coordinator_VSS";
}

void Coordinator_VSS::configure() {
    // Pb default => go to opposite goal and align
    addPlaybook(PLAYBOOK_DEFAULT, _playbook_default = new Playbook_Default());
}

void Coordinator_VSS::run() {
    setPlaybook(PLAYBOOK_DEFAULT);
}
