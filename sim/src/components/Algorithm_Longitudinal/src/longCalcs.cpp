/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2016-2017 ITK Engineering GmbH
 *               2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** @file  longCalcs.cpp */
//-----------------------------------------------------------------------------

#include <cassert>
#include <array>
#include <limits>
#include <map>
#include <iostream>
#include "longCalcs.h"
#include "common/commonTools.h"
#include "components/common/vehicleProperties.h"

AlgorithmLongitudinalCalculations::AlgorithmLongitudinalCalculations(double xVel,
                                                                     double in_aVehicle,
                                                                     const VehicleModelParameters &vehicleModelParameters,
                                                                     std::function<void (CbkLogLevel, const char*, int, const std::string&)> Log) :
    Log(Log),
    velocity(xVel),
    accelerationWish(in_aVehicle),
    vehicleModelParameters(vehicleModelParameters)
{
}

double AlgorithmLongitudinalCalculations::GetBrakePedalPosition() const
{
    return brakePedalPosition;
}

double AlgorithmLongitudinalCalculations::GetAcceleratorPedalPosition() const
{
    return acceleratorPedalPosition;
}

double AlgorithmLongitudinalCalculations::GetEngineSpeed() const
{
    return engineSpeed;
}

int AlgorithmLongitudinalCalculations::GetGear() const
{
    return gear;
}

double AlgorithmLongitudinalCalculations::GetEngineTorqueAtGear(int gear, double acceleration)
{
    if (acceleration == 0 || gear == 0)
    {
        return 0;
    }

    if (gear > GetVehicleProperty(vehicle::properties::NumberOfGears) || gear < 0)
    {
        throw std::runtime_error("Gear in AlgorithmLongitudinal is invalid");
    }

    double wheelSetTorque = GetVehicleProperty(vehicle::properties::Mass) * 0.5 * vehicleModelParameters.rearAxle.wheelDiameter * acceleration;
    double engineTorqueAtGear = wheelSetTorque / (GetVehicleProperty(vehicle::properties::AxleRatio) * GetVehicleProperty(vehicle::properties::GearRatio+std::to_string(gear)));

    return engineTorqueAtGear;
}

double AlgorithmLongitudinalCalculations::GetVehicleProperty(const std::string& propertyName)
{
    const auto property = helper::map::query(vehicleModelParameters.properties, propertyName);
    THROWIFFALSE(property.has_value(), "Vehicle property \"" + propertyName + "\" was not set in the VehicleCatalog");
    return property.value();
}

double AlgorithmLongitudinalCalculations::GetEngineSpeedByVelocity(double xVel, int gear)
{
    return (xVel * GetVehicleProperty(vehicle::properties::AxleRatio) * GetVehicleProperty(vehicle::properties::GearRatio+std::to_string(gear)) * 60) / (vehicleModelParameters.rearAxle.wheelDiameter * M_PI); // an dynamic wheel radius rDyn must actually be used here
}

void AlgorithmLongitudinalCalculations::CalculateGearAndEngineSpeed()
{
    std::map<double, int> nEngineSet;
    std::map<double, std::tuple<int, double, double>> minDeltaAccWheelBased;

    const auto numberOfGears = GetVehicleProperty(vehicle::properties::NumberOfGears);
    for (int gear = 1; gear <= numberOfGears; ++gear)
    {
        double engineSpeed = GetEngineSpeedByVelocity(velocity, gear);
        double limitWheelAcc;
        double accDelta;

        if (accelerationWish >= 0)
        {
            double MMax = GetEngineTorqueMax(engineSpeed);
            limitWheelAcc = GetAccFromEngineTorque(MMax, gear);

            if(accelerationWish == 0)
                accDelta = 0;
            else
                accDelta = std::fabs(accelerationWish - limitWheelAcc);
        }
        else
        {
            double MMin = GetEngineTorqueMin(engineSpeed);
            limitWheelAcc = GetAccFromEngineTorque(MMin, gear);
            accDelta = std::fabs(accelerationWish - limitWheelAcc);
        }

        nEngineSet[engineSpeed] = gear;
        minDeltaAccWheelBased[accDelta] = {gear, engineSpeed, limitWheelAcc};

    }

    bool foundGear = false;

    for (const auto& [engineSpeedResult, gearResult] : nEngineSet)
    {
        if (isWithinEngineLimits(gearResult, engineSpeedResult, accelerationWish)) //&&isChangeOfGearPossibleNow
        {
              gear = gearResult;
              engineSpeed = engineSpeedResult;

              foundGear = true;
        }
        else
            if(foundGear) // leaving possible range
            {
                return;
            }
    }

    if(foundGear)
    {
        return;
    }

    // take lowest delta for gear and engineSpeed
    auto val = minDeltaAccWheelBased.begin()->second;

    // trim wish acceleration to possible value
    gear = std::get<0>(val);
    engineSpeed = std::get<1>(val);
    accelerationWish = std::min(accelerationWish, std::get<2>(val));
}


bool AlgorithmLongitudinalCalculations::isWithinEngineLimits(int gear, double engineSpeed, double acceleration)
{
    if(!isEngineSpeedWithinEngineLimits(engineSpeed))
    {
        return false;
    }

    double currentWishTorque = GetEngineTorqueAtGear(gear, acceleration);

    return isTorqueWithinEngineLimits(currentWishTorque, engineSpeed);
}

bool AlgorithmLongitudinalCalculations::isTorqueWithinEngineLimits(double torque, double engineSpeed)
{
    double currentMEngMax = GetEngineTorqueMax(engineSpeed);

    return torque <= currentMEngMax;
}

inline bool AlgorithmLongitudinalCalculations::isEngineSpeedWithinEngineLimits(double engineSpeed)
{
    return (engineSpeed >= GetVehicleProperty(vehicle::properties::MinimumEngineSpeed) && engineSpeed <= GetVehicleProperty(vehicle::properties::MaximumEngineSpeed));
}

double AlgorithmLongitudinalCalculations::GetEngineTorqueMax(double engineSpeed)
{
    const double maximumEngineTorque = GetVehicleProperty(vehicle::properties::MaximumEngineTorque);
    const double maximumEngineSpeed = GetVehicleProperty(vehicle::properties::MaximumEngineSpeed);
    const double minimumEngineSpeed = GetVehicleProperty(vehicle::properties::MinimumEngineSpeed);

    double torqueMax = maximumEngineTorque; // initial value at max
    double speed = engineSpeed;

    bool isLowerSection = engineSpeed < minimumEngineSpeed + 1000;
    bool isBeyondLowerSectionBorder = engineSpeed < minimumEngineSpeed;
    bool isUpperSection = engineSpeed > maximumEngineSpeed - 1000;
    bool isBeyondUpperSectionBorder = engineSpeed > maximumEngineSpeed;


    if (isLowerSection)
    {
        if (isBeyondLowerSectionBorder) // not within limits
        {
               speed = minimumEngineSpeed;
         }
        torqueMax = (1000 - (speed - minimumEngineSpeed)) * -0.1 + maximumEngineTorque;
    }
    else if (isUpperSection)
    {
        if (isBeyondUpperSectionBorder)
        {
            speed = maximumEngineSpeed;
        }
        torqueMax = (speed - maximumEngineSpeed + 1000) * -0.04 + maximumEngineTorque;
    }

    return torqueMax;
}

double AlgorithmLongitudinalCalculations::GetEngineTorqueMin(double engineSpeed)
{
    return GetEngineTorqueMax(engineSpeed) * -.1;
}

double AlgorithmLongitudinalCalculations::GetAccFromEngineTorque(double engineTorque, int chosenGear)
{
    double wheelSetTorque = engineTorque * (GetVehicleProperty(vehicle::properties::AxleRatio) * GetVehicleProperty(vehicle::properties::GearRatio+std::to_string(chosenGear)));
    double wheelSetForce = wheelSetTorque / (0.5 * vehicleModelParameters.rearAxle.wheelDiameter);
    return wheelSetForce / GetVehicleProperty(vehicle::properties::Mass);
}

void AlgorithmLongitudinalCalculations::CalculatePedalPositions()
{
    constexpr double oneG = 9.81;

    if (accelerationWish < 0) // speed shall be reduced with drag or brake
    {

        double engineTorque = GetEngineTorqueAtGear(gear, accelerationWish);
        double MDragMax = GetEngineTorqueMin(engineSpeed);

        if (engineTorque < MDragMax)
        { // brake

            // calculate acceleration of MDragMax and substract
            // this from in_aVehicle since brake and drag work in parallel while clutch is closed
            double accMDragMax = GetAccFromEngineTorque(MDragMax, gear);

            acceleratorPedalPosition = 0.0;
            brakePedalPosition = std::min(-(accelerationWish - accMDragMax) / oneG, 1.0);
            return;
        }
    }

    // cases of acceleration and drag => use engine here

    double MDragMax = GetEngineTorqueMin(engineSpeed);
    double MTorqueMax = GetEngineTorqueMax(engineSpeed);

    double wishTorque = GetEngineTorqueAtGear(gear, accelerationWish);

    acceleratorPedalPosition = std::min((wishTorque - MDragMax) / (MTorqueMax - MDragMax), 1.0);
    brakePedalPosition = 0.0;
}
