/*******************************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech GmbH
*               2018, 2019 AMFD GmbH
* Copyright (c) 2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
//-----------------------------------------------------------------------------
//! @file  dynamics_regularDrivingImplementation.cpp
//! @brief This file contains the implementation header file
//-----------------------------------------------------------------------------

#include "regularDriving.h"

#include <algorithm>
#include <cassert>
#include <memory>

#include <qglobal.h>

#include "common/accelerationSignal.h"
#include "common/commonTools.h"
#include "common/globalDefinitions.h"
#include "common/longitudinalSignal.h"
#include "common/opMath.h"
#include "common/parametersVehicleSignal.h"
#include "common/steeringSignal.h"
#include "common/rollSignal.h"
#include "components/common/vehicleProperties.h"
#include "include/worldInterface.h"

void DynamicsRegularDrivingImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if (localLinkId == 0)
    {
        const std::shared_ptr<ComponentStateSignalInterface const> stateSignal = std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
        if(stateSignal->componentState == ComponentState::Acting)
        {
            const std::shared_ptr<LongitudinalSignal const> signal = std::dynamic_pointer_cast<LongitudinalSignal const>(data);
            if (!signal)
            {
                const std::string msg = COMPONENTNAME + " invalid signaltype";
                LOG(CbkLogLevel::Debug, msg);
                throw std::runtime_error(msg);
            }
            in_accPedalPos = signal->accPedalPos;
            in_brakePedalPos = signal->brakePedalPos;
            ApplyPedalPositionLimits();
            in_gear = signal->gear;
            ApplyGearLimit();
        }
    }
    else if (localLinkId == 1)
    {
        const std::shared_ptr<ComponentStateSignalInterface const> stateSignal = std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
        if(stateSignal->componentState == ComponentState::Acting)
        {
            const std::shared_ptr<SteeringSignal const> signal = std::dynamic_pointer_cast<SteeringSignal const>(data);
            if (!signal)
            {
                const std::string msg = COMPONENTNAME + " invalid signaltype";
                LOG(CbkLogLevel::Debug, msg);
                throw std::runtime_error(msg);
            }
            in_steeringWheelAngle = signal->steeringWheelAngle;
        }
    }
    else if (localLinkId == 2)
    {
        const std::shared_ptr<ComponentStateSignalInterface const> stateSignal = std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
        if(stateSignal->componentState == ComponentState::Acting)
        {
            const std::shared_ptr<RollSignal const> signal = std::dynamic_pointer_cast<RollSignal const>(data);
            if (!signal)
            {
                const std::string msg = COMPONENTNAME + " invalid signaltype";
                LOG(CbkLogLevel::Debug, msg);
                throw std::runtime_error(msg);
            }
            dynamicsSignal.roll = signal->rollAngle;
        }
    }
    else if (localLinkId == 100)
    {
        // from ParametersAgent
        const std::shared_ptr<ParametersVehicleSignal const> signal = std::dynamic_pointer_cast<ParametersVehicleSignal const>(data);
        if (!signal)
        {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        vehicleModelParameters = signal->vehicleParameters;
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void DynamicsRegularDrivingImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if(localLinkId == 0)
    {
        try {
            data = std::make_shared<DynamicsSignal const>(dynamicsSignal);
        }
        catch(const std::bad_alloc&)
        {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void DynamicsRegularDrivingImplementation::ApplyGearLimit()
{
    in_gear = std::min(std::max(in_gear, 1), static_cast<int>(GetVehicleProperty(vehicle::properties::NumberOfGears)));
}

void DynamicsRegularDrivingImplementation::ApplyPedalPositionLimits()
{
    in_accPedalPos = std::min(std::max(in_accPedalPos, 0.0), 1.0);
    in_brakePedalPos = std::min(std::max(in_brakePedalPos, 0.0), 1.0);
}

double DynamicsRegularDrivingImplementation::GetEngineSpeedByVelocity(double xVel, int gear)
{
    return (xVel * GetVehicleProperty(vehicle::properties::AxleRatio) * GetVehicleProperty(vehicle::properties::GearRatio+std::to_string(gear)) * 60.) /
            (vehicleModelParameters.rearAxle.wheelDiameter * M_PI);  // an dynamic wheel radius rDyn must actually be used here
}

double DynamicsRegularDrivingImplementation::GetEngineMomentMax(double engineSpeed)
{
    const double maximumEngineTorque = GetVehicleProperty(vehicle::properties::MaximumEngineTorque);
    const double maximumEngineSpeed = GetVehicleProperty(vehicle::properties::MaximumEngineSpeed);
    const double minimumEngineSpeed = GetVehicleProperty(vehicle::properties::MinimumEngineSpeed);

    double torqueMax = maximumEngineTorque; // initial value at max
    double speed = engineSpeed;

    bool isLowerSection = engineSpeed < minimumEngineSpeed + 1000.;
    bool isBeyondLowerSectionBorder = engineSpeed < minimumEngineSpeed;
    bool isUpperSection = engineSpeed > maximumEngineSpeed - 1000.;
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

double DynamicsRegularDrivingImplementation::GetAccelerationFromRollingResistance()
{
    double rollingResistanceCoeff = .0125;  // Dummy value, get via vehicle Parameters (vehicleModelParameters.rollingDragCoefficient)
    double accDueToRollingResistance = -rollingResistanceCoeff * _oneG;
    return accDueToRollingResistance;
}

double DynamicsRegularDrivingImplementation::GetAccelerationFromAirResistance(double velocity)
{
    double forceAirResistance = -.5 * _rho * GetVehicleProperty(vehicle::properties::AirDragCoefficient) *
            GetVehicleProperty(vehicle::properties::FrontSurface) * velocity * velocity;
    double accDueToAirResistance = forceAirResistance / GetVehicleProperty(vehicle::properties::Mass);
    return accDueToAirResistance;
}

double DynamicsRegularDrivingImplementation::GetEngineMomentMin(double engineSpeed)
{
    return GetEngineMomentMax(engineSpeed) * -.1;
}

double DynamicsRegularDrivingImplementation::GetFrictionCoefficient()
{
    return GetWorld()->GetFriction() * GetVehicleProperty(vehicle::properties::FrictionCoefficient);
}

double DynamicsRegularDrivingImplementation::GetVehicleProperty(const std::string& propertyName)
{
    const auto property = helper::map::query(vehicleModelParameters.properties, propertyName);
    THROWIFFALSE(property.has_value(), "Vehicle property \"" + propertyName + "\" was not set in the VehicleCatalog");
    return property.value();
}

double DynamicsRegularDrivingImplementation::GetEngineMoment(double gasPedalPos, int gear)
{
    double xVel = GetAgent()->GetVelocity();

    double engineSpeedAtGear = GetEngineSpeedByVelocity(xVel, gear);

    GetAgent()->SetEngineSpeed(engineSpeedAtGear);

    double max = GetEngineMomentMax(engineSpeedAtGear);
    double maxAccAtGear = GetAccFromEngineMoment(xVel, max, gear, GetCycleTime());

    GetAgent()->SetMaxAcceleration(maxAccAtGear*GetFrictionCoefficient());

    double min = GetEngineMomentMin(engineSpeedAtGear);

    return (std::fabs(min) + max) * gasPedalPos + min;
}

double DynamicsRegularDrivingImplementation::GetAccFromEngineMoment(double xVel, double engineMoment, int chosenGear, int cycleTime)
{
    Q_UNUSED(xVel);
    Q_UNUSED(cycleTime);

    double wheelSetMoment = engineMoment * (GetVehicleProperty(vehicle::properties::AxleRatio) * GetVehicleProperty(vehicle::properties::GearRatio+std::to_string(chosenGear)));
    double wheelSetForce = wheelSetMoment / (0.5 * vehicleModelParameters.rearAxle.wheelDiameter);

    double vehicleSetForce = wheelSetForce;
    double acc = vehicleSetForce / GetVehicleProperty(vehicle::properties::Mass);

    return acc;
}

double DynamicsRegularDrivingImplementation::GetAccVehicle(double accPedalPos, double brakePedalPos, int gear, int time)
{
    double resultAcc = 0;

    double xVel = GetAgent()->GetVelocity();

    if (brakePedalPos > 0.)  // Brake
    {
        double accelerationDueToPedal {brakePedalPos * _oneG * -1.};
        double engineSpeed {GetEngineSpeedByVelocity(xVel, gear)};
        double engineDrag {GetEngineMomentMin(engineSpeed)};
        double accelerationDueToDrag {GetAccFromEngineMoment(xVel, engineDrag, gear, GetCycleTime())};
        if (accelerationDueToPedal > 0. || accelerationDueToDrag > 0.)
        {
            throw std::runtime_error("DynamicsRegularDrivingImplementation - Wrong sign for acceleration!");
        }

        resultAcc = accelerationDueToPedal + accelerationDueToDrag;

        double maxDecel = GetAgent()->GetMaxDeceleration();
        resultAcc = std::fmax(maxDecel, resultAcc);
    }
    else  // Gas
    {
        double engineMoment = GetEngineMoment(accPedalPos, gear);
        GetPublisher()->Publish("EngineMoment", engineMoment);
        resultAcc = GetAccFromEngineMoment(xVel, engineMoment, gear, GetCycleTime());
    }

    const double accelerationDueToAirResistance = GetAccelerationFromAirResistance(xVel);
    const double accelerationDueToRollingResistance = GetAccelerationFromRollingResistance();

    return resultAcc+accelerationDueToAirResistance + accelerationDueToRollingResistance;
}

void DynamicsRegularDrivingImplementation::Trigger(int time)
{
    Q_UNUSED(time);

    const auto agent = GetAgent();

    //Lateral behavior
    double maxDecel = _oneG * GetFrictionCoefficient() * -1;
    agent->SetMaxDeceleration(maxDecel);

    double v;
    double yawAngle = agent->GetYaw();

    double accVehicle = GetAccVehicle(in_accPedalPos, in_brakePedalPos, in_gear, time);

    v = agent->GetVelocity() + accVehicle*GetCycleTime()/1000.;

    if(v < VLowerLimit)
    {
        accVehicle = 0.0;
        v = VLowerLimit;
    }

    // change of path coordinate since last time step [m]
    double ds = v * GetCycleTime() * 0.001;
    // change of inertial x-position due to ds and yaw [m]
    double dx = ds * std::cos(yawAngle);
    // change of inertial y-position due to ds and yaw [m]
    double dy = ds * std::sin(yawAngle);
    // new inertial x-position [m]
    double x = agent->GetPositionX() + dx;
    // new inertial y-position [m]
    double y = agent->GetPositionY() + dy;

    dynamicsSignal.acceleration = accVehicle;
    dynamicsSignal.velocity = v;
    dynamicsSignal.positionX = x;
    dynamicsSignal.positionY = y;
    dynamicsSignal.travelDistance = ds;

    // convert steering wheel angle to steering angle of front wheels [radian]
    const auto steering_angle = std::clamp(-vehicleModelParameters.frontAxle.maxSteering, in_steeringWheelAngle / GetVehicleProperty(vehicle::properties::SteeringRatio), vehicleModelParameters.frontAxle.maxSteering);
    dynamicsSignal.steeringWheelAngle = steering_angle * GetVehicleProperty(vehicle::properties::SteeringRatio) ;
    GetPublisher()->Publish("SteeringAngle", steering_angle);
    const double wheelbase = vehicleModelParameters.frontAxle.positionX - vehicleModelParameters.rearAxle.positionX;
    // calculate curvature (Ackermann model; reference point of yawing = rear axle!) [radian]
    double steeringCurvature = std::tan(steering_angle) / wheelbase;
    // change of yaw angle due to ds and curvature [radian]
    double dpsi = std::atan(steeringCurvature*ds);
    dynamicsSignal.yawRate = dpsi / (GetCycleTime() * 0.001);
    dynamicsSignal.centripetalAcceleration = dynamicsSignal.yawRate * v;
    // new yaw angle in current time step [radian]
    double psi = agent->GetYaw() + dpsi;
    dynamicsSignal.yaw = psi;
}
