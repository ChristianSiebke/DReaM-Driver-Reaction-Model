/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "ForceWheelZ.h"

// Calculate the perpendicular force on wheels against inertia
//   - the pitchZ is positive if pitching backward, otherwise negative
//   - the rollZ is positive if rolling right, otherwise negative
bool ForceWheelZ::CalForce(double fInertiaX, double fInertiaY, double pitchZ, double rollZ, VehicleBasics &carParam)
{
    double pitchAngle = atan2(pitchZ, (carParam.lenFront + carParam.lenRear)); // the pitch angle is positive if pitching backward, otherwise negative
    double rollAngle = atan2(rollZ, (carParam.lenLeft + carParam.lenRight));   // the roll angle is positive if rolling right, otherwise negative
    carParam.Deformation(pitchAngle, rollAngle);

    if(!CalForceInPitch(fInertiaX, pitchAngle, carParam))
        return false;

    if(!CalForceInRoll(fInertiaY, rollAngle, carParam))
        return false;

    for(int i=0; i<NUMBER_WHEELS; i++)
    {
        forces[i] = forcesPitch[i] + forcesRoll[i];
    }
    return true;
}

// Calculate the perpendicular force on front and rear wheels against the inertia force in X axis
//   - the pitch angle is positive if pitching backward, otherwise negative
//   - the roll angle is positive if rolling right, otherwise negative
bool ForceWheelZ::CalForceInPitch(double fInertiaX, double pitchAngle, VehicleBasics &carParam)
{

    double wheelBaseLen = carParam.lenFront + carParam.lenRear;
    double frontAngle = atan2(carParam.heightMC, carParam.lenFront) - pitchAngle;
    double rearAngle  = atan2(carParam.heightMC, carParam.lenRear)  + pitchAngle;

    // Calculate the perpendicular force on front wheels
    double dRear2MC = sqrt((carParam.lenRear * carParam.lenRear) + (carParam.heightMC * carParam.heightMC));
    double fZFront = dRear2MC * fInertiaX * sin(rearAngle) / wheelBaseLen;

    // Calculate the perpendicular force on rear wheels
    double dFront2MC = sqrt((carParam.lenFront * carParam.lenFront) + (carParam.heightMC * carParam.heightMC));
    double fZRear = dFront2MC * (-fInertiaX) * sin(frontAngle) / wheelBaseLen;


    // separate front force to frontleft and frontright wheels
    forcesPitch[0] = fZFront / (1 + carParam.ratioY);  // frontleft
    forcesPitch[1] = carParam.ratioY * forcesPitch[0];     // frontright

    // separate rear force to rearleft and rearright wheels
    forcesPitch[2] = fZRear / (1 + carParam.ratioY);     // rearleft
    forcesPitch[3] = carParam.ratioY * forcesPitch[2];     // rearright

    return true;
}

// Calculate the perpendicular force on left and right wheels against the inertia force in Y axis
//   - the pitch angle is positive if pitching backward, otherwise negative
//   - the roll angle is positive if rolling right, otherwise negative
bool ForceWheelZ::CalForceInRoll(double fInertiaY, double rollAngle, VehicleBasics &carParam)
{

    double wheelBaseWid = carParam.lenLeft + carParam.lenRight;
    double leftAngle  = atan2(carParam.heightMC, carParam.lenLeft)  - rollAngle;
    double rightAngle = atan2(carParam.heightMC, carParam.lenRight) + rollAngle;

    // Calculate the perpendicular force on front wheels against the longitudinal inertia
    double dRight2MC = sqrt((carParam.lenRight * carParam.lenRight) + (carParam.heightMC * carParam.heightMC));
    double fZLeft = dRight2MC * fInertiaY * sin(rightAngle) / wheelBaseWid;

    // Calculate the perpendicular force on rear wheels against the longitudinal inertia
    double dLeft2MC = sqrt((carParam.lenLeft * carParam.lenLeft) + (carParam.heightMC * carParam.heightMC));
    double fZRight = dLeft2MC * (-fInertiaY) * sin(leftAngle) / wheelBaseWid;

    // separate left force to frontleft and rearleft wheels
    forcesRoll[0] = fZLeft / (1 + carParam.ratioX);   // frontleft
    forcesRoll[2] = carParam.ratioX * forcesRoll[0];     // rearleft

    // separate right force to frontright and rearright wheels
    forcesRoll[1] = fZRight / (1 + carParam.ratioX);  // frontright
    forcesRoll[3] = carParam.ratioX * forcesRoll[1];     // rearright

    return true;
}
