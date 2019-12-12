/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  trajectoryFollowerCommonBase.cpp */
//-----------------------------------------------------------------------------

#include <memory>
#include <qglobal.h>

#include "trajectoryFollowerCommonBase.h"

#include "Interfaces/parameterInterface.h"
#include "Interfaces/observationInterface.h"
#include "Common/componentStateChangeEvent.h"

TrajectoryFollowerCommonBase::TrajectoryFollowerCommonBase(std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface* stochastics,
        WorldInterface* world,
        const ParameterInterface* parameters,
        const std::map<int, ObservationInterface*>* observations,
        const CallbackInterface* callbacks,
        AgentInterface* agent,
        SimulationSlave::EventNetworkInterface * const eventNetwork) :
    UnrestrictedEventModelInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        observations,
        callbacks,
        agent,
        eventNetwork),
    cycleTimeInSeconds{static_cast<double>(cycleTime) / 1000.0}
{
    dynamicsOutputSignal.positionX = 0;
    dynamicsOutputSignal.positionY = 0;
    dynamicsOutputSignal.yaw = 0;

    ParseParameters(parameters);
}

void TrajectoryFollowerCommonBase::ParseParameters(const ParameterInterface* parameters)
{
    try
    {
        auto boolParameters = parameters->GetParametersBool();
        enforceTrajectory = boolParameters.at("EnforceTrajectory");
        automaticDeactivation = boolParameters.at("AutomaticDeactivation");
    }
    catch (const std::out_of_range& error)
    {
        LOG(CbkLogLevel::Error, error.what());
        throw std::runtime_error(error.what());
    }
}

void TrajectoryFollowerCommonBase::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data,
        int time)
{
    Q_UNUSED(time);

    if (localLinkId == 0)
    {
        if (!enforceTrajectory)
        {
            const std::shared_ptr<ComponentStateSignalInterface const> stateSignal =
                std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
            if (stateSignal != nullptr && stateSignal->componentState == ComponentState::Acting)
            {
                const std::shared_ptr<LateralSignal const> signal = std::dynamic_pointer_cast<LateralSignal const>(data);
                if (!signal)
                {
                    ThrowInvalidSignalTypeError();
                }

                const bool isLateralSignalActing = signal->componentState == ComponentState::Acting;
                if (isLateralSignalActing)
                {
                    UpdateState(ComponentState::Disabled);
                }
            }
        }
    }
    else
        if (localLinkId == 1)
        {
            if (!enforceTrajectory)
            {
                const std::shared_ptr<ComponentStateSignalInterface const> stateSignal =
                    std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
                if (stateSignal != nullptr && stateSignal->componentState == ComponentState::Acting)
                {
                    const std::shared_ptr<AccelerationSignal const> signal = std::dynamic_pointer_cast<AccelerationSignal const>(data);
                    if (!signal)
                    {
                        ThrowInvalidSignalTypeError();
                    }

                    inputAccelerationActive = true;
                    inputAcceleration = signal->acceleration;
                }
                else
                {
                    inputAcceleration = 0;
                }
            }
        }
        else
        {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Error, msg);
            throw std::runtime_error(msg);
        }
}

void TrajectoryFollowerCommonBase::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time)
{
    Q_UNUSED(time);

    if (localLinkId == 0)
    {
        try
        {
            dynamicsOutputSignal.componentState = componentState;
            data = std::make_shared<DynamicsSignal const>(dynamicsOutputSignal);
        }
        catch (const std::bad_alloc&)
        {
            ThrowCouldNotInstantiateSignalError();
        }
    }
    else if (localLinkId == 83)
    {
        HandleCompCtrlSignalOutput(data);
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void TrajectoryFollowerCommonBase::Trigger(int time)
{
    const auto stateChangeEventList = GetEventNetwork()->GetActiveEventCategory(EventDefinitions::EventCategory::ComponentStateChange);

    for (const auto &stateChangeEvent : stateChangeEventList)
    {
        const auto &componentChangeEvent = std::dynamic_pointer_cast<ComponentChangeEvent>(stateChangeEvent);

        if (componentChangeEvent->GetComponentName() == COMPONENTNAME
            && std::find(componentChangeEvent->actingAgents.begin(), componentChangeEvent->actingAgents.end(), GetAgent()->GetId()) != componentChangeEvent->actingAgents.end())
        {
            UpdateState(componentChangeEvent->GetGoalState());
        }
    }

    CalculateNextTimestep(time);
}

void TrajectoryFollowerCommonBase::HandleCompCtrlSignalOutput(std::shared_ptr<SignalInterface const>& data)
{
    try
    {
        data = std::make_shared<AgentCompToCompCtrlSignal>(ComponentType::TrajectoryFollower,
                                                           GetComponentName(),
                                                           componentState);
    }
    catch (const std::bad_alloc&)
    {
        ThrowCouldNotInstantiateSignalError();
    }
}

ComponentState TrajectoryFollowerCommonBase::GetState() const
{
    return componentState;
}

void TrajectoryFollowerCommonBase::UpdateState(const ComponentState newState)
{
    // only update state if the newstate differs from the current state
    if (newState != componentState)
    {
        if (newState == ComponentState::Acting)
        {
            if (canBeActivated)
            {
                componentState = newState;
                std::shared_ptr<VehicleComponentEvent> event = std::make_shared<VehicleComponentEvent>(currentTime,
                        GetComponentName(),
                        "TrajectoryFollower_Activate",
                        EventDefinitions::EventType::TrajectoryFollowerActivated,
                        GetAgent()->GetId());

                GetObservations()->at(0)->InsertEvent(event);
            }
        }
        else if (newState == ComponentState::Disabled)
        {
            canBeActivated = false;
            componentState = newState;
            std::shared_ptr<VehicleComponentEvent> event = std::make_shared<VehicleComponentEvent>(currentTime,
                    GetComponentName(),
                    "TrajectoryFollower_Deactivate",
                    EventDefinitions::EventType::TrajectoryFollowerDeactivated,
                    GetAgent()->GetId());

            GetObservations()->at(0)->InsertEvent(event);
        }
        else
        {
            throw std::runtime_error("TrajectoryFollower cannot handle new component state");
        }
    }
}

[[ noreturn ]] void TrajectoryFollowerCommonBase::ThrowCouldNotInstantiateSignalError()
{
    const std::string msg = COMPONENTNAME + " could not instantiate signal";
    LOG(CbkLogLevel::Debug, msg);
    throw std::runtime_error(msg);
}

[[ noreturn ]] void TrajectoryFollowerCommonBase::ThrowInvalidSignalTypeError()
{
    const std::string msg = COMPONENTNAME + " invalid signaltype";
    LOG(CbkLogLevel::Debug, msg);
    throw std::runtime_error(msg);
}

void TrajectoryFollowerCommonBase::HandleEndOfTrajectory()
{
    dynamicsOutputSignal.velocity = 0;
    dynamicsOutputSignal.acceleration = 0;
    dynamicsOutputSignal.travelDistance = 0;
    dynamicsOutputSignal.yawRate = 0;

    if(automaticDeactivation)
    {
        UpdateState(ComponentState::Disabled);
    }
}
