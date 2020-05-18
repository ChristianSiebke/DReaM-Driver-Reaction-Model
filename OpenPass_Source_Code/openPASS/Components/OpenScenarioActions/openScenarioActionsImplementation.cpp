/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ComponentControllerImplementation.cpp
//! @brief This file contains the implementation of the header file
//-----------------------------------------------------------------------------

#include "openScenarioActionsImplementation.h"
#include "Common/trajectorySignal.h"
#include "Common/customLaneChangeSignal.h"
#include "Common/gazeFollowerSignal.h"
#include "Interfaces/eventNetworkInterface.h"
#include "Common/eventTypes.h"

OpenScenarioActionsImplementation::OpenScenarioActionsImplementation(std::string componentName,
                                                                     bool isInit,
                                                                     int priority,
                                                                     int offsetTime,
                                                                     int responseTime,
                                                                     int cycleTime,
                                                                     StochasticsInterface *stochastics,
                                                                     WorldInterface *world,
                                                                     const ParameterInterface *parameters,
                                                                     const std::map<int, ObservationInterface *> *observations,
                                                                     const CallbackInterface *callbacks,
                                                                     AgentInterface *agent,
                                                                     SimulationSlave::EventNetworkInterface* const eventNetwork) :
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
    calculation(world)
{
}

void OpenScenarioActionsImplementation::UpdateInput([[maybe_unused]]int localLinkId, [[maybe_unused]]const std::shared_ptr<SignalInterface const>& data, [[maybe_unused]]int time)
{
}

void OpenScenarioActionsImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, [[maybe_unused]]int time)
{
    if (localLinkId == 0)
    {
        if (trajectoryEvent)
        {
            data = std::make_shared<TrajectorySignal>(ComponentState::Acting, trajectoryEvent->trajectory);
        }
        else
        {
            data = std::make_shared<TrajectorySignal>();
        }
    }
    else if (localLinkId == 1)
    {
        if (customLaneChangeEvent)
        {
            data = std::make_shared<CustomLaneChangeSignal>(ComponentState::Acting, customLaneChangeEvent->deltaLaneId);
        }
        else
        {
            data = std::make_shared<CustomLaneChangeSignal>();
        }
    }
    else if (localLinkId == 2)
    {
        if (gazeFollowerEvent)
        {
            data = std::make_shared<GazeFollowerSignal>(ComponentState::Acting, gazeFollowerEvent->gazeActivityState, gazeFollowerEvent->gazeFileName);
        }
        else
        {
            data = std::make_shared<GazeFollowerSignal>();
        }
    }
}

void OpenScenarioActionsImplementation::Trigger([[maybe_unused]]int time)
{
    const auto& agentId = GetAgent()->GetId();

    trajectoryEvent = nullptr;
    customLaneChangeEvent = nullptr;
    gazeFollowerEvent = nullptr;

    const auto laneChangeEventList = GetEventNetwork()->GetActiveEventCategory(EventDefinitions::EventCategory::LaneChange);

    for (const auto &event : laneChangeEventList)
    {
        const auto& castedLaneChangeEvent = std::dynamic_pointer_cast<LaneChangeEvent>(event);
        if (castedLaneChangeEvent && castedLaneChangeEvent->agentId == agentId)
        {
            const auto& laneChange = castedLaneChangeEvent->laneChange;

            double deltaS;
            double deltaTime;
            if (laneChange.dynamicsType == LaneChangeParameter::DynamicsType::Distance)
            {
                deltaS = laneChange.dynamicsTarget;
                deltaTime = deltaS / GetAgent()->GetVelocity();
            }
            else //LaneChange::DynamicsType::Time
            {
                deltaS = laneChange.dynamicsTarget * GetAgent()->GetVelocity();
                deltaTime = laneChange.dynamicsTarget;
            }

            const auto& roadId = GetAgent()->GetRoadId(MeasurementPoint::Reference);
            const auto& currentLaneId = GetAgent()->GetMainLaneId(MeasurementPoint::Reference);
            const auto& currentS = GetAgent()->GetDistanceToStartOfRoad(MeasurementPoint::Reference);
            int targetLaneId;
            if (laneChange.type == LaneChangeParameter::Type::Absolute)
            {
                targetLaneId = laneChange.value;
            }
            else
            {
                const auto object = GetWorld()->GetAgentByName(laneChange.object);
                targetLaneId = object->GetMainLaneId(MeasurementPoint::Reference) + laneChange.value;
            }
            double deltaT;
            if (targetLaneId > currentLaneId) //Change to left
            {
                deltaT = 0.5 * GetWorld()->GetLaneWidth(Route{roadId}, roadId, currentLaneId, currentS)
                         + 0.5 * GetWorld()->GetLaneWidth(Route{roadId}, roadId, targetLaneId, currentS);
                for (int laneId = currentLaneId + 1; laneId < targetLaneId; ++laneId)
                {
                    deltaT += GetWorld()->GetLaneWidth(Route{roadId}, roadId, laneId, currentS);
                }
            }
            else if(targetLaneId < currentLaneId)//Change to right
            {
                deltaT = - 0.5 * GetWorld()->GetLaneWidth(Route{roadId}, roadId, currentLaneId, currentS)
                         - 0.5 * GetWorld()->GetLaneWidth(Route{roadId}, roadId, targetLaneId, currentS);
                for (int laneId = currentLaneId - 1; laneId > targetLaneId; --laneId)
                {
                    deltaT -= GetWorld()->GetLaneWidth(Route{roadId}, roadId, laneId, currentS);
                }
            }
            else
            {
                continue;
            }

            GlobalRoadPosition startPosition{roadId,
                                             currentLaneId,
                                             currentS,
                                             GetAgent()->GetPositionLateral(),
                                             GetAgent()->GetRelativeYaw()}; //TODO: For new routing branch use new method GetObjectPosition
            auto trajectory = calculation.CalculateSinusiodalLaneChange(deltaS, deltaT, deltaTime, GetCycleTime() / 1000.0, startPosition, time / 1000.0);
            this->trajectoryEvent = std::make_shared<TrajectoryEvent>(time, "OscAction", castedLaneChangeEvent->GetName(), GetAgent()->GetId(), trajectory);
        }
    }

    const auto trajectoryEventList = GetEventNetwork()->GetActiveEventCategory(EventDefinitions::EventCategory::SetTrajectory);

    for (const auto &event : trajectoryEventList)
    {
        const auto& castedtrajectoryEvent = std::dynamic_pointer_cast<TrajectoryEvent>(event);
        if (castedtrajectoryEvent && castedtrajectoryEvent->agentId == agentId)
        {
            this->trajectoryEvent = castedtrajectoryEvent;
        }
    }

    const auto customLaneChangeEventList = GetEventNetwork()->GetActiveEventCategory(EventDefinitions::EventCategory::CustomLaneChange);

    for (const auto &event : customLaneChangeEventList)
    {
        const auto& castedCustomLaneChangeEvent = std::dynamic_pointer_cast<CustomLaneChangeEvent>(event);
        if (castedCustomLaneChangeEvent && castedCustomLaneChangeEvent->agentId == agentId)
        {
            this->customLaneChangeEvent = castedCustomLaneChangeEvent;
        }
    }
    
    const auto gazeFollowerEventList = GetEventNetwork()->GetActiveEventCategory(EventDefinitions::EventCategory::SetGazeFollower);

    for (const auto &event : gazeFollowerEventList)
    {
        const auto& castedGazeFollowerEvent = std::dynamic_pointer_cast<GazeFollowerEvent>(event);
        if (castedGazeFollowerEvent && castedGazeFollowerEvent->agentId == agentId)
        {
            this->gazeFollowerEvent = castedGazeFollowerEvent;
        }
    }
}
