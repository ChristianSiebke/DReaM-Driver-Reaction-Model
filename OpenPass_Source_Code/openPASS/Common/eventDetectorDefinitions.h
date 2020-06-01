/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <optional>
#include <string>
#include <variant>
#include "Common/openScenarioDefinitions.h"
#include "Interfaces/agentInterface.h"
#include "Interfaces/worldInterface.h"

namespace openScenario
{

namespace ConditionEquality
{
//! Maximum difference between two values to be consired equal for OpenSCENARIO rule equal_to
static constexpr double EPSILON = 1e-12;
}

enum class Rule
{
    LessThan = 0,
    EqualTo,
    GreaterThan
};


// OpenScenario ByEntity Conditions
class ByEntityCondition
{
public:
    ByEntityCondition(const std::vector<std::string> &triggeringEntityNames):
        triggeringEntityNames(triggeringEntityNames)
    {}
    ByEntityCondition(const ByEntityCondition&) = default;
    virtual ~ByEntityCondition();

    std::vector<AgentInterface *> GetTriggeringAgents(WorldInterface* const world) const
    {
        std::vector<AgentInterface *> triggeringAgents {};

        if(triggeringEntityNames.empty())
        {
            const auto &agentMap = world->GetAgents();
            std::transform(agentMap.cbegin(),
                           agentMap.cend(),
                           std::back_inserter(triggeringAgents),
                           [] (const auto &agentPair) -> AgentInterface*
                           {
                               return agentPair.second;
                           });
        }
        else
        {
            for (const auto& triggeringEntityName : triggeringEntityNames)
            {
                const auto triggeringAgent = world->GetAgentByName(triggeringEntityName);

                if(triggeringAgent != nullptr)
                {
                    triggeringAgents.emplace_back(triggeringAgent);
                }
            }
        }

        return triggeringAgents;
    }

private:
    const std::vector<std::string> triggeringEntityNames;
};

class TimeToCollisionCondition : public ByEntityCondition
{
public:
    TimeToCollisionCondition(const std::vector<std::string>& triggeringEntityNames,
                             const std::string& referenceEntityName,
                             const double targetTTC,
                             const Rule rule):
        ByEntityCondition(triggeringEntityNames),
        referenceEntityName(referenceEntityName),
        targetTTC(targetTTC),
        rule(rule)
    {}
    TimeToCollisionCondition(const TimeToCollisionCondition&) = default;
    virtual ~TimeToCollisionCondition();

    std::vector<const AgentInterface*> IsMet(WorldInterface * const world) const;

private:
    const std::string referenceEntityName;
    const double targetTTC;
    const Rule rule;
};

class TimeHeadwayCondition : public ByEntityCondition
{
public:
    TimeHeadwayCondition(const std::vector<std::string>& triggeringEntityNames,
                         const std::string& referenceEntityName,
                         const double targetTHW,
                         const bool freeSpace,
                         const Rule rule):
        ByEntityCondition(triggeringEntityNames),
        referenceEntityName(referenceEntityName),
        targetTHW(targetTHW),
        freeSpace(freeSpace),
        rule(rule)
    {}
    TimeHeadwayCondition(const TimeHeadwayCondition&) = default;
    virtual ~TimeHeadwayCondition();

    std::vector<const AgentInterface*> IsMet(WorldInterface * const world) const;

private:
    const std::string referenceEntityName;
    const double targetTHW;
    const bool freeSpace;
    const Rule rule;
};

class ReachPositionCondition : public ByEntityCondition
{
public:
    ReachPositionCondition(const std::vector<std::string>& triggeringEntityNames,
                           const double tolerance,
                           const openScenario::Position position):
        ByEntityCondition(triggeringEntityNames),
        tolerance(tolerance),
        position(position)
    {
        if (tolerance < 0)
        {
            throw std::runtime_error("Reach Position Tolerance must be greater than or equal to 0");
        }

        if (std::holds_alternative<openScenario::RoadPosition>(position))
        {
            const auto roadPosition = std::get<openScenario::RoadPosition>(position);

            if (roadPosition.s < 0)
            {
                throw std::runtime_error("Reach Position Target S Coordinate must be greater than or equal to 0");
            }

        }
    }
    ReachPositionCondition(const ReachPositionCondition&) = default;
    virtual ~ReachPositionCondition();

    std::vector<const AgentInterface*> IsMet(WorldInterface * const world) const;

private:
    const double tolerance{};
    const openScenario::Position position{};
};

class RelativeSpeedCondition : public ByEntityCondition
{
public:
    RelativeSpeedCondition(const std::vector<std::string> &triggeringEntityNames,
                           const std::string &referenceEntityName,
                           const double value,
                           const Rule rule):
        ByEntityCondition(triggeringEntityNames),
        referenceEntityName(referenceEntityName),
        value(value),
        rule(rule)
    {}
    RelativeSpeedCondition(const RelativeSpeedCondition&) = default;
    virtual ~RelativeSpeedCondition();

    std::vector<const AgentInterface*> IsMet(WorldInterface * const world) const;

private:
    const std::string referenceEntityName{};
    const double value{};
    const Rule rule{};
};

// OpenScenario ByValue Conditions
class ByValueCondition
{
public:
    ByValueCondition(const Rule rule):
        rule(rule)
    {}
    ByValueCondition(const ByValueCondition&) = default;
    virtual ~ByValueCondition();
protected:
    const Rule rule;
};

class SimulationTimeCondition : public ByValueCondition
{
public:
    SimulationTimeCondition(const Rule rule,
                            const double targetValueInSeconds):
        ByValueCondition(rule),
        targetValue(static_cast<int>(targetValueInSeconds * 1000.0))
    {}
    SimulationTimeCondition(const SimulationTimeCondition&) = default;
    virtual ~SimulationTimeCondition();

    bool IsMet(const int value) const;
    int GetTargetValue() const;

private:
    const int targetValue;
};

using Condition = std::variant<ReachPositionCondition,
                               RelativeSpeedCondition,
                               SimulationTimeCondition,
                               TimeToCollisionCondition,
                               TimeHeadwayCondition>;
using ConditionGroup = std::vector<Condition>;

///
/// \brief Event specific information collected from an openSCENARIO story
///
struct ConditionalEventDetectorInformation
{
    ActorInformation actorInformation{};
    int numberOfExecutions{};             ///< Specifies number of executions. Use -1 for "unrestricted"
    std::string eventName{};
    ConditionGroup conditions{};
};

} // openScenario
