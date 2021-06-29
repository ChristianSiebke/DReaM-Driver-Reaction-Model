/*********************************************************************
* Copyright (c) 2020, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "WheelOscillation.h"

WheelOscillation::WheelOscillation(int wId, double k, double q): id(wId), coeffSpring(k), coeffDamp(q) {}

void WheelOscillation::Init(int wId, double time_step, double k, double q)
{
    id = wId;
    timeStep = time_step;
    coeffSpring = k;
    coeffDamp = q;
}

void WheelOscillation::Perform(double forceZ, double mass)
{
    /*
     *  forceZ + counter_force = m * Az
     *  forceZ + fSpring + fDamp = m * Az
     *  forceZ - K * Z - Q * Vz= m * Az
     *     Z = prevZ + prevVz * dt + 0.5 * Az * dt^2
     *     Vz = prevVz + Az * dt
     *  forceZ - K * (prevZ + prevVz * dt + 0.5 * Az * dt^2) - Q * (prevVz + Az * dt) = m * Az
     *  forceZ - K * prevZ - K * prevVz * dt - Q * prevVz = (m + 0.5 * K * dt^2 + Q * dt ) * Az
    */

    curAz = (forceZ - coeffSpring * curZ - coeffDamp * curVz) / (mass);
    curVz = prevVz + curAz * timeStep;
    curZ += prevVz * timeStep + 0.5 * curAz * timeStep * timeStep;

    curZ = (curZ > maxZ) ? maxZ : curZ;
    curZ = (curZ < minZ) ? minZ : curZ;

    // prepare for the next iteration
    prevVz = curVz;
}
