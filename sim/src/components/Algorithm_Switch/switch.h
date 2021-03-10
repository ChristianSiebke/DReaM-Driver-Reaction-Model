#ifndef SWITCH_H
#define SWITCH_H

#include "controlData.h"

class Switch
{
public:
    Switch();

    ControlData Perform(int index, ControlData driver, ControlData prio1, ControlData prio2, ControlData prio3);
private:
    ControlData resultingControl;
    int collisionState;
    double collisionSteering;
};

#endif // SWITCH_H
