/********************************************************************************
 * Copyright (c) 2020-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef VEHICLEBASICS_H
#define VEHICLEBASICS_H

#define NUMBER_WHEELS 4
#define GRAVITY_ACC 9.81

#include <math.h>

class VehicleBasics
{
public:
    VehicleBasics() = default;

    VehicleBasics(double lLeft, double lRight, double lFront, double lRear, double hMC, double m):
        lenLeft(lLeft), lenRight(lRight), lenFront(lFront), lenRear(lRear), heightMC(hMC), mass(m)
    {
        CalculateRatio();
        CalculateWheelMass();
    }

//    VehicleBasics(const VehicleBasics &) = delete;
//    VehicleBasics(VehicleBasics &&) = delete;
//    VehicleBasics &operator=(const VehicleBasics &) = delete;
//    VehicleBasics &operator=(VehicleBasics &&) = delete;

    double lenLeft;
    double lenRight;
    double lenFront;
    double lenRear;

    double ratioX; // front length / rear length
    double ratioY;  // left length / right length

    double heightMC; // hight of mass center
    double mass;

    void CalculateRatio()
    {
        ratioX = (lenFront - mcOffsetX) / (lenRear + mcOffsetX);
        ratioY = (lenLeft - mcOffsetY) / (lenRight + mcOffsetY);
    }

    void CalculateWheelMass()
    {
        double massFront = mass / (1 + ratioX);
        double massRear = ratioX * massFront;

        // separate front mass to frontleft and frontright wheels
        wheelMass[0] = massFront / (1 + ratioY);  // frontleft
        wheelMass[1] = ratioY * wheelMass[0];     // frontright

        // separate rear mass to rearleft and rearright wheels
        wheelMass[2] = massRear / (1 + ratioY);   // rearleft
        wheelMass[3] = ratioY * wheelMass[2];     // rearright
    }

    void Deformation(double pitchAngle, double rollAngle)
    {
        if(pitchAngle >= 0) // pitching backward, and mcOffsetX will be negative
        {
            mcOffsetX = - sin(pitchAngle) * (heightMC + lenRear * tan(pitchAngle));
        }
        else // pitching foreward, and mcOffsetX will be positive
        {
            mcOffsetX = sin(-pitchAngle) * (heightMC + lenFront * tan(-pitchAngle));
        }

        if(rollAngle >= 0) // rolling towards right, and mcOffsetY will be negative
        {
            mcOffsetY = - sin(rollAngle) * (heightMC + lenRight* tan(rollAngle));
        }
        else // rolling towards left, and mcOffsetY will be positive
        {
            mcOffsetY = sin(-rollAngle) * (heightMC + lenLeft * tan(-rollAngle));
        }

        CalculateRatio();
        CalculateWheelMass();
    }

    double GetWheelMass(int i) { return wheelMass[i]; }

private:
    double mcOffsetX = 0.0;
    double mcOffsetY = 0.0;

    double wheelMass[NUMBER_WHEELS];
};

#endif //VEHICLEBASICS_H
