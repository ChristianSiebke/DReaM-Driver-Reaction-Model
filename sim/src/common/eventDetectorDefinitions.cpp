/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/


#include <vector>

#include "include/egoAgentInterface.h"
#include "common/commonTools.h"
#include "common/eventDetectorDefinitions.h"

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

    for (auto triggeringAgent : GetTriggeringAgents(world))
    {
        auto longPos = triggeringAgent->GetEgoAgent().GetDistanceToObject(referenceAgent);
        auto deltaS = freeSpace ? longPos.netDistance : longPos.referencePoint;

        if (deltaS.has_value())
        {
            double timeHeadway = deltaS.value() / triggeringAgent->GetVelocity(VelocityScope::Longitudinal);

            if (CheckEquation(timeHeadway, targetTHW, rule))
            {
                conditionMetAgents.emplace_back(triggeringAgent);
            }
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

std::vector<const AgentInterface*> ReachPositionCondition::IsMet(WorldInterface * const world) const
{
    std::vector<const AgentInterface*> conditionMetAgents{};

    if (std::holds_alternative<openScenario::RoadPosition>(position))
    {
        const auto roadPosition = std::get<openScenario::RoadPosition>(position);

        for (const auto agent : GetTriggeringAgents(world))
        {
            const auto& roadIds = agent->GetRoads(MeasurementPoint::Reference);
            if (std::find(roadIds.cbegin(), roadIds.cend(), roadPosition.roadId) != roadIds.end())
            {
                const auto sCoordinate = agent->GetObjectPosition().referencePoint.at(roadPosition.roadId).roadPosition.s;

                if (std::abs(roadPosition.s - sCoordinate) <= tolerance)
                {
                   conditionMetAgents.emplace_back(agent);
                }
            }
        }
    }
    else if (std::holds_alternative<openScenario::RelativeLanePosition>(position))
    {
        const auto relativeLanePosition = std::get<openScenario::RelativeLanePosition>(position);
        const auto referenceAgent = world->GetAgentByName(relativeLanePosition.entityRef);
        if (!referenceAgent)
        {
            throw std::runtime_error("Reference Entity '" + relativeLanePosition.entityRef + "' does not exist for RelativeLane Condition");
        }

        for (const auto agent : GetTriggeringAgents(world))
        {
            for (const auto& roadId : agent->GetRoads(MeasurementPoint::Reference))
            {
                const auto& referenceAgentRoads = referenceAgent->GetRoads(MeasurementPoint::Reference);
                if (std::find(referenceAgentRoads.cbegin(), referenceAgentRoads.cend(), roadId) != referenceAgentRoads.cend()
                        && agent->GetObjectPosition().referencePoint.at(roadId).laneId == referenceAgent->GetObjectPosition().referencePoint.at(roadId).laneId + relativeLanePosition.dLane)
                {
                    const auto agentS = agent->GetObjectPosition().referencePoint.at(roadId).roadPosition.s;
                    const auto referenceS = referenceAgent->GetObjectPosition().referencePoint.at(roadId).roadPosition.s + relativeLanePosition.ds;
                    if (std::abs(referenceS - agentS) <= tolerance)
                    {
                    conditionMetAgents.emplace_back(agent);
                    }
                }
            }
        }
    }
    else
    {
        throw std::runtime_error("Position type not supported by ReachPositionCondition.");
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
ByValueCondition::~ByValueCondition(){}
SimulationTimeCondition::~SimulationTimeCondition(){}

}; // openScenario
