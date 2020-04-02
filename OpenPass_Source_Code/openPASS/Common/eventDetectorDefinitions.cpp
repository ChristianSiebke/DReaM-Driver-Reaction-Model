/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "commonTools.h"
#include "eventDetectorDefinitions.h"

namespace openScenario
{
// OpenScenario ByEntity Conditions
bool CheckEquation (double lhs, double rhs, Rule rule)
{
    switch (rule)
    {
        case Rule::LessThan:
            return lhs < rhs;
        case Rule::EqualTo:
            return std::abs(lhs - rhs) < ConditionEquality::EPSILON;
        case Rule::GreaterThan:
            return lhs > rhs;
    }
    throw std::invalid_argument( "invalid rule" );
}

std::vector<const AgentInterface *> TimeToCollisionCondition::IsMet(WorldInterface* const world) const
{
    const auto referenceAgent = world->GetAgentByName(referenceEntityName);
    if (!referenceAgent)
    {
        throw std::runtime_error("Could not find reference agent for condition 'TimeToCollision'");
    }

    std::vector<const AgentInterface*> conditionMetAgents{};
    for (const auto triggeringAgent : GetTriggeringAgents(world))
    {
        double ttc;
        constexpr double timeStep = 100;

        ttc = TtcCalculations::CalculateObjectTTC(*triggeringAgent, *referenceAgent, targetTTC + timeStep / 1000.0, 0.0, 0.0, timeStep);

        if (CheckEquation(ttc, targetTTC, rule))
        {
            conditionMetAgents.emplace_back(triggeringAgent);
        }
    }

    return conditionMetAgents;
}

std::vector<const AgentInterface *> TimeHeadwayCondition::IsMet(WorldInterface* const world) const
{
    const auto referenceAgent = world->GetAgentByName(referenceEntityName);
    if (!referenceAgent)
    {
        throw std::runtime_error("Could not find reference agent for condition 'TimeHeadway'");
    }

    std::vector<const AgentInterface*> conditionMetAgents{};
    for (const auto triggeringAgent : GetTriggeringAgents(world))
    {
        double deltaS{std::numeric_limits<double>::infinity()};
        if (freeSpace)
        {
            deltaS = triggeringAgent->GetDistanceToObject(referenceAgent);
        }
        else
        {
            const auto roadId = triggeringAgent->GetRoadId(MeasurementPoint::Reference);
            if (referenceAgent->GetRoadId(MeasurementPoint::Reference) != roadId)
            {
                break;
            }
            double sOwn = triggeringAgent->GetDistanceToStartOfRoad(MeasurementPoint::Reference, roadId);
            double sOther = referenceAgent->GetDistanceToStartOfRoad(MeasurementPoint::Reference, roadId);
            deltaS = sOther - sOwn;
        }
        double timeHeadway = deltaS / triggeringAgent->GetVelocity(VelocityScope::Longitudinal);

        if (CheckEquation(timeHeadway, targetTHW, rule))
        {
            conditionMetAgents.emplace_back(triggeringAgent);
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

        if (CheckEquation(relativeVelocityOfTriggeringAgent, value, rule))
        {
            conditionMetAgents.emplace_back(triggeringAgent);
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
        throw std::runtime_error("Reference Entity '" + referenceEntityName + "' does not exist for RelativeLane Condition");
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
    return CheckEquation(value, targetValue, rule);
}

int SimulationTimeCondition::GetTargetValue() const
{
    return targetValue;
}

// out-of-line virtual function declarations to prevent unnecessary vtable emissions
ByEntityCondition::~ByEntityCondition(){}
TimeToCollisionCondition::~TimeToCollisionCondition(){}
TimeHeadwayCondition::~TimeHeadwayCondition(){}
ReachPositionCondition::~ReachPositionCondition(){}
RelativeSpeedCondition::~RelativeSpeedCondition(){}
ReachPositionRoadCondition::~ReachPositionRoadCondition(){}
RelativeLaneCondition::~RelativeLaneCondition(){}
ByValueCondition::~ByValueCondition(){}
SimulationTimeCondition::~SimulationTimeCondition(){}

}; // openScenario
