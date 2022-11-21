/********************************************************************************
 * Copyright (c) 2020-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "switch.h"

Switch::Switch(){
    resultingControl = {0.0, 0.0, 0.0, {0.0, 0.0, 0.0, 0.0}};
    collisionState = false;
    collisionSteering = 0.0;
}

ControlData Switch::Perform(int index, ControlData driver, ControlData prio1, ControlData prio2, ControlData prio3) {
    /** @addtogroup switch_group2
     * Calculate the resulting control signals that are sent to the dynamics modules:
     *
     * After a collision, the driver control at the time of the collision
     * will be used for the remaining simulation.
     *
     * Without a collision, the active assistance system with the highest
     * priority will be used for the output signal.
     *
     * If any system has been active before, but not anymore, a special
     * control signal for fading out will be used.
    */

    resultingControl = ControlData{
            driver.steer,
            driver.throttle,
            driver.brakePedal,
            driver.brakeSuperpose};

    if (collisionState == false)
    {
        if (index == 0)
        {
            collisionState = true;
            collisionSteering = driver.steer;
        }
    }

    if (collisionState == true)
    {
        resultingControl = ControlData{collisionSteering, 0.0, 1.0, {0.0, 0.0, 0.0, 0.0}};
        //resultingControl = ControlData{0.0, 0.0, 1.0, {0.0, 0.0, 0.0, 0.0}};
    }
    else if (index == 1)
    {
        resultingControl = prio1;
    }
    else if (index == 2)
    {
        resultingControl = prio2;
    }
    else if (index == 3)
    {
        resultingControl = prio3;
    }
    else if (index == 100)
    {
        double fadingOutBrake = 0.7;
        double fadingOutSteer = (driver.steer<-0.1) ? -0.1 : (driver.steer>0.1) ? 0.1 : driver.steer;
        resultingControl = ControlData{fadingOutSteer, 0.0, fadingOutBrake, {0.0, 0.0, 0.0, 0.0}};
    }

    return resultingControl;
}
