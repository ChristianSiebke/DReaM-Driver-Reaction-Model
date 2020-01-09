/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "eventDetectorDefinitions.h"
#include "commonTools.h"

namespace openScenario
{
// OpenScenario ByEntity Conditions

std::vector<const AgentInterface *> TimeToCollisionCondition::IsMet(WorldInterface* const world) const
{
    const auto referenceAgent = world->GetAgentByName(referenceEntityName);
    if (!referenceAgent)
    {
        throw std::runtime_error("Reference Agent does not exist for TimeToCollision Condition");
    }

    std::vector<const AgentInterface*> conditionMetAgents{};
    for (const auto triggeringAgent : GetTriggeringAgents(world))
    {
        double ttc;
        constexpr double timeStep = 100;

        ttc = TtcCalculations::CalculateObjectTTC(*triggeringAgent, *referenceAgent, targetTTC + timeStep / 1000.0, 0.0, 0.0, timeStep);

        switch(rule)
        {
            case Rule::LessThan:
                if (ttc < targetTTC - CommonHelper::EPSILON)
                {
                    conditionMetAgents.emplace_back(triggeringAgent);
                }
                break;
            case Rule::EqualTo:
                if (std::abs(targetTTC - ttc) < CommonHelper::EPSILON)
                {
                    conditionMetAgents.emplace_back(triggeringAgent);
                }
                break;
            case Rule::GreaterThan:
                if (ttc > targetTTC + CommonHelper::EPSILON)
                {
                    conditionMetAgents.emplace_back(triggeringAgent);
                }
                break;
        }
    }

    return conditionMetAgents;
}

std::vector<const AgentInterface*> RelativeSpeedCondition::IsMet(WorldInterface * const world) const
{
    const auto referenceAgent = world->GetAgentByName(referenceEntityName);
    if (!referenceAgent)
    {
        throw std::runtime_error("Reference Agent does not exist for RelativeSpeedCondition");
    }

    std::vector<const AgentInterface*> conditionMetAgents{};
    for (const auto triggeringAgent : GetTriggeringAgents(world))
    {
        const double relativeVelocityOfTriggeringAgent = triggeringAgent->GetVelocity() - referenceAgent->GetVelocity();

        switch (rule)
        {
            case Rule::LessThan:
                if (relativeVelocityOfTriggeringAgent < tolerance)
                {
                    conditionMetAgents.emplace_back(triggeringAgent);
                }
                break;
            case Rule::EqualTo:
                if (relativeVelocityOfTriggeringAgent == tolerance)
                {
                    conditionMetAgents.emplace_back(triggeringAgent);
                }
                break;
            case Rule::GreaterThan:
                if (relativeVelocityOfTriggeringAgent > tolerance)
                {
                    conditionMetAgents.emplace_back(triggeringAgent);
                }
                break;
        }
    }

    return conditionMetAgents;
}

std::vector<const AgentInterface*> ReachPositionRoadCondition::IsMet(WorldInterface * const world) const
{
    std::vector<const AgentInterface*> conditionMetAgents{};
    for (const auto agent : GetTriggeringAgents(world))
    {
        const auto& roadId = agent->GetRoadId(MeasurementPoint::Reference);

        if(roadId == targetRoadId)
        {
            const auto sCoordinate = agent->GetRoadPosition().s;

            if(std::abs(targetSCoordinate - sCoordinate) <= tolerance)
            {
                conditionMetAgents.emplace_back(agent);
            }
        }
    }

    return conditionMetAgents;
}

std::vector<const AgentInterface*> RelativeLaneCondition::IsMet(WorldInterface * const world) const
{
    const auto referenceAgent = world->GetAgentByName(referenceEntityName);
    if (!referenceAgent)
    {
        throw std::runtime_error("Reference Entity does not exist for RelativeLane Condition");
    }

    std::vector<const AgentInterface*> conditionMetAgents{};
    for (const auto agent : GetTriggeringAgents(world))
    {
        if (agent->GetRoadId(MeasurementPoint::Reference) == referenceAgent->GetRoadId(MeasurementPoint::Reference)
            && agent->GetMainLaneId(MeasurementPoint::Reference) == referenceAgent->GetMainLaneId(MeasurementPoint::Reference) + deltaLane)
        {
            const auto agentS = agent->GetRoadPosition().s;
            const auto referenceS = referenceAgent->GetRoadPosition().s + deltaS;

            if (std::abs(referenceS - agentS) <= tolerance)
            {
                conditionMetAgents.emplace_back(agent);
            }
        }
    }

    return conditionMetAgents;
}

// OpenScenario ByValue Conditions
bool SimulationTimeCondition::IsMet(const int value) const
{
    if (rule == Rule::LessThan)
    {
        return value < targetValue;
    }

    if (rule == Rule::EqualTo)
    {
        return value == targetValue;
    }
    if (rule == Rule::GreaterThan)
    {
        return value > targetValue;
    }

    return false;
}

int SimulationTimeCondition::GetTargetValue() const
{
    return targetValue;
}

// out-of-line virtual function declarations to prevent unnecessary vtable emissions
ByEntityCondition::~ByEntityCondition(){}
TimeToCollisionCondition::~TimeToCollisionCondition(){}
ReachPositionCondition::~ReachPositionCondition(){}
RelativeSpeedCondition::~RelativeSpeedCondition(){}
ReachPositionRoadCondition::~ReachPositionRoadCondition(){}
RelativeLaneCondition::~RelativeLaneCondition(){}
ByValueCondition::~ByValueCondition(){}
SimulationTimeCondition::~SimulationTimeCondition(){}

}; // openScenario
