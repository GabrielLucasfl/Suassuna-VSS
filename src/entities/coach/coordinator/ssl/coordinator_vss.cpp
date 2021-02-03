#include "coordinator_vss.h"

Coordinator_VSS::Coordinator_VSS() {
    timer.start();
    actualState = PLAYBOOK_DEFAULT;
}

QString Coordinator_VSS::name() {
    return "Coordinator_VSS";
}

void Coordinator_VSS::configure() {
    // Pb default => go to opposite goal and align
    addPlaybook(PLAYBOOK_DEFAULT, _playbook_default = new Playbook_Default());

    // Pb default2 => go to allie goal and align
    addPlaybook(PLAYBOOK_DEFAULT2, _playbook_default2 = new Playbook_Default2());
}

void Coordinator_VSS::run() {
    timer.stop();
    // Each 6 seconds, swap playbooks
    if(timer.getSeconds() >= 5.0) {
        timer.start();

        if(actualState == PLAYBOOK_DEFAULT) {
            actualState = PLAYBOOK_DEFAULT2;
        }
        else {
            actualState = PLAYBOOK_DEFAULT;
        }

        setPlaybook(actualState);
    }
}
