/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  LimiterAccelerationVehicleComponentsImplementation.cpp
//! @brief This file contains the implementation of the header file
//-----------------------------------------------------------------------------

#include "common/opMath.h"
#include <qglobal.h>

#include "include/worldInterface.h"
#include "components/common/vehicleProperties.h"

#include "limiterAccVehCompImpl.h"

void LimiterAccelerationVehicleComponentsImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if (localLinkId == 0) {
        const std::shared_ptr<ComponentStateSignalInterface const> stateSignal = std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
        if (stateSignal->componentState == ComponentState::Acting)
        {
            const std::shared_ptr<AccelerationSignal const> signal = std::dynamic_pointer_cast<AccelerationSignal const>(data);
            if (!signal)
            {
                const std::string msg = GetComponentName() + " invalid signaltype";
                LOG(CbkLogLevel::Debug, msg);
                throw std::runtime_error(msg);
            }

            incomingAcceleration = signal->acceleration;
        }
        componentState = stateSignal->componentState;
    }
    else if (localLinkId == 100)
    {
        // from ParametersAgent
        const std::shared_ptr<ParametersVehicleSignal const> signal = std::dynamic_pointer_cast<ParametersVehicleSignal const>(data);
        if (!signal)
        {
            const std::string msg = GetComponentName() + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
        vehicleModelParameters = signal->vehicleParameters;

        PrepareReferences();
    }
    else
    {
        const std::string msg = GetComponentName() + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void LimiterAccelerationVehicleComponentsImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if(localLinkId == 0)
    {
        try
        {
            data = std::make_shared<AccelerationSignal const>(componentState, outgoingAcceleration);
        }
        catch(const std::bad_alloc&)
        {
            const std::string msg = GetComponentName() + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }

    else
    {
        const std::string msg = GetComponentName() + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void LimiterAccelerationVehicleComponentsImplementation::Trigger(int time)
{
    Q_UNUSED(time);

    const double &accelerationLimit = CalculateAccelerationLimit();
    const double &decelerationLimit = CalculateDecelerationLimit();

    outgoingAcceleration = std::max(std::min(incomingAcceleration, accelerationLimit), decelerationLimit);
}

double LimiterAccelerationVehicleComponentsImplementation::GetVehicleProperty(const std::string& propertyName)
{
    const auto property = helper::map::query(vehicleModelParameters.properties, propertyName);
    THROWIFFALSE(property.has_value(), "Vehicle property \"" + propertyName + "\" was not set in the VehicleCatalog");
    return property.value();
}

double LimiterAccelerationVehicleComponentsImplementation::InterpolateEngineTorqueBasedOnSpeed(const double &engineSpeed)
{
    if(engineSpeedReferences.size() != engineTorqueReferences.size() || engineSpeedReferences.empty())
    {
        throw std::runtime_error("Interpolation requires same size for vectors and at least one element.");
    }

    if(engineSpeed <= engineSpeedReferences.front())
    {
        return engineTorqueReferences.front();
    }

    if(engineSpeed >= engineSpeedReferences.back())
    {
        return engineTorqueReferences.back();
    }

    for(size_t i = 1; i < engineSpeedReferences.size(); i++)
    {
        if(engineSpeedReferences.at(i) >= engineSpeed)
        {
            const double differenceBetweenEngineSpeedReferencePoints = engineSpeedReferences.at(i) - engineSpeedReferences.at(i-1);
            const double percentage = (engineSpeed - engineSpeedReferences.at(i-1)) / differenceBetweenEngineSpeedReferencePoints;

            const double differenceBetweenEngineTorqueReferencePoints = engineTorqueReferences.at(i) - engineTorqueReferences.at(i-1);

            const double interpolatedTorque = engineTorqueReferences.at(i-1) + (differenceBetweenEngineTorqueReferencePoints * percentage);

            return interpolatedTorque;
        }
    }

    throw std::runtime_error("Could not interpolate torque.");
}

std::vector<double> LimiterAccelerationVehicleComponentsImplementation::PrepareEngineTorqueVectorBasedOnGearRatios()
{
    std::vector<double> engineTorquesBasedOnGearRatios {};

    if(GetVehicleProperty(vehicle::properties::NumberOfGears) < 1)
    {
        throw std::runtime_error("At least on gear is required!");
    }

    for(size_t i = 1; i <= GetVehicleProperty(vehicle::properties::NumberOfGears); i++)
    {
        const double engineSpeedBasedOnGear = CalculateEngineSpeedBasedOnGear(GetAgent()->GetVelocity(), i);

        if(engineSpeedBasedOnGear > GetVehicleProperty(vehicle::properties::MaximumEngineSpeed) || engineSpeedBasedOnGear < GetVehicleProperty(vehicle::properties::MinimumEngineSpeed))
        {
            continue;
        }

        const double interpolatedEngineTorque = InterpolateEngineTorqueBasedOnSpeed(engineSpeedBasedOnGear);

        double engineTorqueBasedOnGearRatio = interpolatedEngineTorque * GetVehicleProperty(vehicle::properties::GearRatio+std::to_string(i));
        engineTorquesBasedOnGearRatios.push_back(engineTorqueBasedOnGearRatio);
    }

    return engineTorquesBasedOnGearRatios;
}

double LimiterAccelerationVehicleComponentsImplementation::CalculateEngineSpeedBasedOnGear(const double &currentVelocity, const size_t &gear)
{
    const double engineSpeed = currentVelocity * GetVehicleProperty(vehicle::properties::AxleRatio) * GetVehicleProperty(vehicle::properties::GearRatio+std::to_string(gear)) / (M_PI * vehicleModelParameters.rearAxle.wheelDiameter) * 60.0;
    return engineSpeed;
}

void LimiterAccelerationVehicleComponentsImplementation::PrepareReferences()
{
    const double& maxEngineTorque = GetVehicleProperty(vehicle::properties::MaximumEngineTorque);
    const double& maxEngineSpeed = GetVehicleProperty(vehicle::properties::MaximumEngineSpeed);
    const double& minEngineSpeed = GetVehicleProperty(vehicle::properties::MinimumEngineSpeed);

    engineTorqueReferences = {0.5 * maxEngineTorque,
                              1.0 * maxEngineTorque,
                              1.0 * maxEngineTorque,
                              1.0 * maxEngineTorque,
                              1.0 * maxEngineTorque / maxEngineSpeed * 5000.0};

    engineSpeedReferences = {minEngineSpeed,
                             1350,
                             4600,
                             5000,
                             maxEngineSpeed};
}

double LimiterAccelerationVehicleComponentsImplementation::CalculateAccelerationLimit()
{
    const double currentVelocity = GetAgent()->GetVelocity();

    const auto &engineTorquesBasedOnGearRatios = PrepareEngineTorqueVectorBasedOnGearRatios();

    if(engineTorquesBasedOnGearRatios.empty())
    {
        return oneG;
    }

    const double &engineTorqueBasedOnVelocity = *(std::max_element(engineTorquesBasedOnGearRatios.begin(), engineTorquesBasedOnGearRatios.end()));

    const double forceAtWheel = engineTorqueBasedOnVelocity * GetVehicleProperty(vehicle::properties::AxleRatio) / (0.5 * vehicleModelParameters.rearAxle.wheelDiameter);
    const double forceRoll = GetVehicleProperty(vehicle::properties::Mass) * oneG * rollFrictionCoefficient;
    const double forceAir = (airResistance / 2) * GetVehicleProperty(vehicle::properties::FrontSurface) * GetVehicleProperty(vehicle::properties::AirDragCoefficient) * std::pow(currentVelocity, 2);

    const double accelerationLimit = (forceAtWheel - forceRoll - forceAir) / GetVehicleProperty(vehicle::properties::Mass);

    return accelerationLimit;
}

double LimiterAccelerationVehicleComponentsImplementation::CalculateDecelerationLimit()
{
    const double decelerationLimit = GetWorld()->GetFriction() * GetVehicleProperty(vehicle::properties::FrictionCoefficient) * (-oneG);

    return decelerationLimit;
}
