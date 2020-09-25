/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  AlgorithmAEBmplementation.cpp */
//-----------------------------------------------------------------------------

#include "autonomousEmergencyBraking.h"

#include <limits>
#include <memory>

#include "common/commonTools.h"
#include "common/eventTypes.h"
#include "common/sensorFusionQuery.h"
#include "boundingBoxCalculation.h"

AlgorithmAutonomousEmergencyBrakingImplementation::AlgorithmAutonomousEmergencyBrakingImplementation(
    std::string componentName,
    bool isInit,
    int priority,
    int offsetTime,
    int responseTime,
    int cycleTime,
    StochasticsInterface *stochastics,
    const ParameterInterface *parameters,
    PublisherInterface * const publisher,
    const CallbackInterface *callbacks,
    AgentInterface *agent) :
    AlgorithmInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        parameters,
        publisher,
        callbacks,
        agent)
{
    try
    {
        ParseParameters(parameters);
    }
    catch (...)
    {
        const std::string msg = COMPONENTNAME + " could not init parameters";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    try
    {
        if (GetPublisher() == nullptr)
        {
            throw std::runtime_error("");
        }
    }
    catch (...)
    {
        const std::string msg = COMPONENTNAME + " invalid publisher module setup";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
}

void AlgorithmAutonomousEmergencyBrakingImplementation::ParseParameters(const ParameterInterface *parameters)
{
    ttcBrake = parameters->GetParametersDouble().at("TTC");
    brakingAcceleration = parameters->GetParametersDouble().at("Acceleration");
    collisionDetectionLongitudinalBoundary = parameters->GetParametersDouble().at("CollisionDetectionLongitudinalBoundary");
    collisionDetectionLateralBoundary = parameters->GetParametersDouble().at("CollisionDetectionLateralBoundary");

    const auto &sensorList = parameters->GetParameterLists().at("SensorLinks");
    for (const auto &sensorLink : sensorList)
    {
        if (sensorLink->GetParametersString().at("InputId") == "Camera")
        {
            sensors.push_back(sensorLink->GetParametersInt().at("SensorId"));
        }
    }
}

void AlgorithmAutonomousEmergencyBrakingImplementation::UpdateInput(int localLinkId,
                                                                    const std::shared_ptr<SignalInterface const> &data,
                                                                    [[maybe_unused]] int time)
{
    std::stringstream log;
    log << COMPONENTNAME << " (component " << GetComponentName() << ", agent " << GetAgent()->GetId() << ", input data for local link " << localLinkId << ": ";
    LOG(CbkLogLevel::Debug, log.str());

    //from SensorFusion
    if (localLinkId == 0)
    {
        const std::shared_ptr<SensorDataSignal const> signal = std::dynamic_pointer_cast<SensorDataSignal const>(data);
        if (!signal)
        {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        auto sensorData = signal->sensorData;
        detectedMovingObjects = SensorFusionHelperFunctions::RetrieveMovingObjectsBySensorId(sensors, sensorData);
        detectedStationaryObjects = SensorFusionHelperFunctions::RetrieveStationaryObjectsBySensorId(sensors, sensorData);
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void AlgorithmAutonomousEmergencyBrakingImplementation::UpdateOutput(int localLinkId,
                                                                     std::shared_ptr<SignalInterface const> &data,
                                                                     [[maybe_unused]] int time)
{
    if (localLinkId == 0)
    {
        try
        {
            data = std::make_shared<AccelerationSignal const>(componentState, activeAcceleration);
        }
        catch (const std::bad_alloc &)
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

void AlgorithmAutonomousEmergencyBrakingImplementation::Trigger([[maybe_unused]] int time)
{
    const auto ttc = CalculateTTC();

    if (componentState == ComponentState::Disabled && ShouldBeActivated(ttc))
    {
        componentState = ComponentState::Acting;
        UpdateAcceleration();
    }
    else if (componentState == ComponentState::Acting && ShouldBeDeactivated(ttc))
    {
        componentState = ComponentState::Disabled;
        UpdateAcceleration();
    }
}

bool AlgorithmAutonomousEmergencyBrakingImplementation::ShouldBeActivated(const double ttc) const
{
    return ttc < ttcBrake;
}

bool AlgorithmAutonomousEmergencyBrakingImplementation::ShouldBeDeactivated(const double ttc) const
{
    return ttc > (ttcBrake * 1.5);
}

double AlgorithmAutonomousEmergencyBrakingImplementation::CalculateObjectTTC(const osi3::BaseMoving &baseMoving)
{
    TtcCalculations::TtcParameters own;
    own.length = GetAgent()->GetLength() + collisionDetectionLongitudinalBoundary;
    own.width = GetAgent()->GetWidth() + collisionDetectionLateralBoundary;
    own.frontLength = GetAgent()->GetDistanceReferencePointToLeadingEdge() + 0.5 * collisionDetectionLongitudinalBoundary;
    own.backLength = own.length - own.frontLength;
    own.position = {0.0, 0.0};
    own.velocityX = 0.0;
    own.velocityY = 0.0;
    own.accelerationX = 0.0;
    own.accelerationY = 0.0;
    own.yaw = 0.0;
    own.yawRate = GetAgent()->GetYawRate();
    own.yawAcceleration = 0.0; // GetAgent()->GetYawAcceleration() not implemented yet
    TtcCalculations::TtcParameters opponent;
    opponent.length = baseMoving.dimension().length() + collisionDetectionLongitudinalBoundary;
    opponent.width = baseMoving.dimension().width() + collisionDetectionLateralBoundary;
    opponent.frontLength = 0.5 * opponent.length;
    opponent.backLength = 0.5 * opponent.length;
    opponent.position = {baseMoving.position().x(), baseMoving.position().y()};
    opponent.velocityX = baseMoving.velocity().x();
    opponent.velocityY = baseMoving.velocity().y();
    opponent.accelerationX = baseMoving.acceleration().x();
    opponent.accelerationY = baseMoving.acceleration().y();
    opponent.yaw = baseMoving.orientation().yaw();
    opponent.yawRate = baseMoving.orientation_rate().yaw();
    opponent.yawAcceleration = baseMoving.orientation_acceleration().yaw();
    return TtcCalculations::CalculateObjectTTC(own, opponent, ttcBrake * 1.5, GetCycleTime());
}

double AlgorithmAutonomousEmergencyBrakingImplementation::CalculateObjectTTC(const osi3::BaseStationary &baseStationary)
{
    TtcCalculations::TtcParameters own;
    own.length = GetAgent()->GetLength() + collisionDetectionLongitudinalBoundary;
    own.width = GetAgent()->GetWidth() + collisionDetectionLateralBoundary;
    own.frontLength = GetAgent()->GetDistanceReferencePointToLeadingEdge() + 0.5 * collisionDetectionLongitudinalBoundary;
    own.backLength = own.length - own.frontLength;
    own.position = {0.0, 0.0};
    own.velocityX = 0.0;
    own.velocityY = 0.0;
    own.accelerationX = 0.0;
    own.accelerationY = 0.0;
    own.yaw = 0.0;
    own.yawRate = GetAgent()->GetYawRate();
    own.yawAcceleration = 0.0; // GetAgent()->GetYawAcceleration() not implemented yet

    TtcCalculations::TtcParameters opponent;
    opponent.length = baseStationary.dimension().length() + collisionDetectionLongitudinalBoundary;
    opponent.width = baseStationary.dimension().width() + collisionDetectionLateralBoundary;
    opponent.frontLength = 0.5 * opponent.length;
    opponent.backLength = 0.5 * opponent.length;
    opponent.position = {baseStationary.position().x(), baseStationary.position().y()};
    opponent.velocityX = -GetAgent()->GetVelocity();
    opponent.velocityY = 0.0;
    opponent.accelerationX = -GetAgent()->GetAcceleration();
    opponent.accelerationY = 0.0;
    opponent.yaw = baseStationary.orientation().yaw();
    opponent.yawRate = 0.0;
    opponent.yawAcceleration = 0.0;

    return TtcCalculations::CalculateObjectTTC(own, opponent, ttcBrake * 1.5, GetCycleTime());
}

double AlgorithmAutonomousEmergencyBrakingImplementation::CalculateTTC()
{
    double ttc = std::numeric_limits<double>::max();
    for (const auto &detectedObject : detectedMovingObjects)
    {
        double objectTtc = CalculateObjectTTC(detectedObject.base());
        if (objectTtc < ttc)
        {
            ttc = objectTtc;
        }
    }
    for (const auto &detectedObject : detectedStationaryObjects)
    {
        double objectTtc = CalculateObjectTTC(detectedObject.base());
        if (objectTtc < ttc)
        {
            ttc = objectTtc;
        }
    }

    return ttc;
}

void AlgorithmAutonomousEmergencyBrakingImplementation::SetAcceleration(double setValue)
{
    activeAcceleration = setValue;
    GetPublisher()->Publish(COMPONENTNAME, ComponentEvent({{"ComponentState", openpass::utils::to_string(componentState)}}));
}

void AlgorithmAutonomousEmergencyBrakingImplementation::UpdateAcceleration()
{
    if (componentState == ComponentState::Acting &&
        (activeAcceleration - brakingAcceleration) != 0.0)
    {
        SetAcceleration(brakingAcceleration);
    }
    else if (componentState == ComponentState::Disabled &&
             activeAcceleration != 0.0)
    {
        SetAcceleration(0.0);
    }
}
