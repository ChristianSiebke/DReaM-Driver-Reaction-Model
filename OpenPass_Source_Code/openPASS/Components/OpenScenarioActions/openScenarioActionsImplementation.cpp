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
#include "Common/speedActionSignal.h"
#include "Interfaces/eventNetworkInterface.h"
#include "Interfaces/egoAgentInterface.h"
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
                                                                     PublisherInterface * const publisher,
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
        publisher,
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
    else if (localLinkId == 3)
    {
        if (speedActionEvent)
        {
            data = std::make_shared<SpeedActionSignal>(ComponentState::Acting, speedActionTargetSpeed, speedActionAcceleration);
        }
        else
        {
            data = std::make_shared<SpeedActionSignal>();
        }
    }
}

void OpenScenarioActionsImplementation::Trigger([[maybe_unused]]int time)
{
    const auto& agentId = GetAgent()->GetId();

    trajectoryEvent = nullptr;
    customLaneChangeEvent = nullptr;
    gazeFollowerEvent = nullptr;
    speedActionEvent = nullptr;

    const auto laneChangeEventList = GetEventNetwork()->GetActiveEventCategory(EventDefinitions::EventCategory::LaneChange);

    for (const auto &event : laneChangeEventList)
    {
        const auto& castedLaneChangeEvent = std::dynamic_pointer_cast<LaneChangeEvent>(event);
        if (castedLaneChangeEvent && castedLaneChangeEvent->agentId == agentId)
        {
            const auto& laneChange = castedLaneChangeEvent->laneChange;

            double deltaS;
            double deltaTime;
            if (laneChange.dynamicsType == openScenario::LaneChangeParameter::DynamicsType::Distance)
            {
                deltaS = laneChange.dynamicsTarget;
                deltaTime = deltaS / GetAgent()->GetVelocity();
            }
            else //LaneChange::DynamicsType::Time
            {
                deltaS = laneChange.dynamicsTarget * GetAgent()->GetVelocity();
                deltaTime = laneChange.dynamicsTarget;
            }

            const auto& roadId = GetAgent()->GetEgoAgent().GetRoadId();
            if (GetAgent()->GetObjectPosition().referencePoint.count(roadId) == 0)
            {
                throw std::runtime_error("Could not calculate LaneChange trajectory. Reference point is not on route.");
            }
            const auto currentLaneId = GetAgent()->GetObjectPosition().referencePoint.at(roadId).laneId;
            const auto currentS = GetAgent()->GetObjectPosition().referencePoint.at(roadId).roadPosition.s;
            int targetLaneId;
            if (laneChange.type == openScenario::LaneChangeParameter::Type::Absolute)
            {
                targetLaneId = laneChange.value;
            }
            else
            {
                const auto object = GetWorld()->GetAgentByName(laneChange.object);
                if (object->GetObjectPosition().referencePoint.count(roadId) == 0)
                {
                    throw std::runtime_error("Could not calculate LaneChange trajectory. Reference agent is not on same road as acting agent.");
                }
                targetLaneId = object->GetObjectPosition().referencePoint.at(roadId).laneId + laneChange.value;
            }
            double deltaT;
            if (targetLaneId > currentLaneId) //Change to left
            {
                deltaT = 0.5 * GetWorld()->GetLaneWidth(roadId, currentLaneId, currentS)
                         + 0.5 * GetWorld()->GetLaneWidth(roadId, targetLaneId, currentS);
                for (int laneId = currentLaneId + 1; laneId < targetLaneId; ++laneId)
                {
                    deltaT += GetWorld()->GetLaneWidth(roadId, laneId, currentS);
                }
            }
            else if(targetLaneId < currentLaneId)//Change to right
            {
                deltaT = - 0.5 * GetWorld()->GetLaneWidth(roadId, currentLaneId, currentS)
                         - 0.5 * GetWorld()->GetLaneWidth(roadId, targetLaneId, currentS);
                for (int laneId = currentLaneId - 1; laneId > targetLaneId; --laneId)
                {
                    deltaT -= GetWorld()->GetLaneWidth(roadId, laneId, currentS);
                }
            }
            else
            {
                continue;
            }

            GlobalRoadPosition startPosition{roadId,
                                             currentLaneId,
                                             currentS,
                                             GetAgent()->GetObjectPosition().referencePoint.at(roadId).roadPosition.t,
                                             GetAgent()->GetObjectPosition().referencePoint.at(roadId).roadPosition.hdg};
            auto trajectory = calculation.CalculateSinusiodalLaneChange(deltaS, deltaT, deltaTime, GetCycleTime() / 1000.0, startPosition, 0.0);
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

    const auto speedActionEventList = GetEventNetwork()->GetActiveEventCategory(EventDefinitions::EventCategory::SpeedAction);

    for (const auto &event : speedActionEventList)
    {
        const auto& speedActionEvent = std::dynamic_pointer_cast<SpeedActionEvent>(event);
        if (speedActionEvent && speedActionEvent->agentId == agentId)
        {
            const auto &speedAction = speedActionEvent->speedAction;

            // Determine target speed
            if (std::holds_alternative<openScenario::RelativeTargetSpeed>(speedAction.target))
            {
                const auto &relativeTargetSpeed = std::get<openScenario::RelativeTargetSpeed>(speedAction.target);
                auto referenceAgent = GetWorld()->GetAgentByName(relativeTargetSpeed.entityRef);

                if (!referenceAgent)
                {
                    throw std::runtime_error("Can't find entityref: " + relativeTargetSpeed.entityRef + " of SpeedAction.");
                }

                if (relativeTargetSpeed.speedTargetValueType == openScenario::SpeedTargetValueType::Delta)
                {
                    speedActionTargetSpeed = referenceAgent->GetVelocity() + relativeTargetSpeed.value;
                }
                else if (relativeTargetSpeed.speedTargetValueType == openScenario::SpeedTargetValueType::Factor)
                {
                    speedActionTargetSpeed = referenceAgent->GetVelocity() * relativeTargetSpeed.value;
                }
            }
            else if (std::holds_alternative<openScenario::AbsoluteTargetSpeed>(speedAction.target))
            {
                const auto &absoluteTargetSpeed = std::get<openScenario::AbsoluteTargetSpeed>(speedAction.target);
                speedActionTargetSpeed = absoluteTargetSpeed.value;
            }

            // Determine acceleration
            if (speedAction.transitionDynamics.shape == openScenario::Shape::Step)
            {
                double deltaVelocity = speedActionTargetSpeed - GetAgent()->GetVelocity();

                speedActionAcceleration = std::abs(deltaVelocity / (GetCycleTime() / 1000.0));
            }
            else if (speedAction.transitionDynamics.shape == openScenario::Shape::Linear)
            {
                speedActionAcceleration = speedAction.transitionDynamics.value;
            }
            else
            {
                throw std::runtime_error("Invalid TransitionsDyanimcs shape in SpeedAction.");
            }

            this->speedActionEvent = speedActionEvent;
        }
    }
}
