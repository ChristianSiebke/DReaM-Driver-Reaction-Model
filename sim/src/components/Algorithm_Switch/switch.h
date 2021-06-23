/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

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
