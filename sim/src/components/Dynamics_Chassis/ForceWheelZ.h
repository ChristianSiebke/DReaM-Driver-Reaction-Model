/*********************************************************************
* Copyright (c) 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef FORCEWHEELZ_H
#define FORCEWHEELZ_H

#include "VehicleBasics.h"
#include <vector>

//! Wheel force perpendicular.
class ForceWheelZ
{
public:
    ForceWheelZ() = default;
    ForceWheelZ(const ForceWheelZ &) = delete;
    ForceWheelZ(ForceWheelZ &&) = delete;
    ForceWheelZ &operator=(const ForceWheelZ &) = delete;
    ForceWheelZ &operator=(ForceWheelZ &&) = delete;

    virtual ~ForceWheelZ() = default;

    // Calculate the perpendicular force on wheels against inertia
    bool CalForce(double fInertiaX, double fInertiaY, double pitchZ, double rollZ, VehicleBasics &carParam);

    double GetForce(int i) const { return forces[i]; }

private:
    double forces[NUMBER_WHEELS]      = {0.0, 0.0, 0.0, 0.0};
    double forcesPitch[NUMBER_WHEELS] = {0.0, 0.0, 0.0, 0.0};
    double forcesRoll[NUMBER_WHEELS]  = {0.0, 0.0, 0.0, 0.0};

    // Calculate the perpendicular force on front and rear wheels against the inertia force in X axis
    bool CalForceInPitch(double fInertiaX, double pitchAngle, VehicleBasics &carParam);

    // Calculate the perpendicular force on left and right wheels against the inertia force in Y axis
    bool CalForceInRoll(double fInertiaY, double rollAngle, VehicleBasics &carParam);
};

#endif //FORCEWHEELZ_H
