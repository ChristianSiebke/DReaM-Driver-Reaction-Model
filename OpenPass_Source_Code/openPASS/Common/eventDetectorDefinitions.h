/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*               2020 HLRS, University of Stuttgart.
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
#include "opExport.h"

namespace openScenario
{

enum class Rule
{
    LessThan = 0,
    EqualTo,
    GreaterThan
};


// OpenScenario ByEntity Conditions
class OPENPASSCOMMONEXPORT ByEntityCondition
{
public:
    ByEntityCondition(const std::vector<std::string> &triggeringEntityNames):
        triggeringEntityNames(triggeringEntityNames)
    {}
    ByEntityCondition(const ByEntityCondition&) = default;
    virtual ~ByEntityCondition();

    ByEntityCondition() = delete;
    ByEntityCondition(ByEntityCondition&&) = delete;
    ByEntityCondition& operator=(const ByEntityCondition&) = delete;
    ByEntityCondition& operator=(ByEntityCondition&&) = delete;

    std::vector<const AgentInterface *> GetTriggeringAgents(WorldInterface* const world) const
    {
        std::vector<const AgentInterface *> triggeringAgents {};

        if(triggeringEntityNames.empty())
        {
            const auto &agentMap = world->GetAgents();
            std::transform(agentMap.cbegin(),
                           agentMap.cend(),
                           std::back_inserter(triggeringAgents),
                           [] (const auto &agentPair) -> const AgentInterface*
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

class OPENPASSCOMMONEXPORT TimeToCollisionCondition : public ByEntityCondition
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
    ~TimeToCollisionCondition();

    std::vector<const AgentInterface*> IsMet(WorldInterface * const world) const;

    TimeToCollisionCondition() = delete;
    TimeToCollisionCondition(TimeToCollisionCondition&&) = delete;
    TimeToCollisionCondition& operator=(const TimeToCollisionCondition&) = delete;
    TimeToCollisionCondition& operator=(TimeToCollisionCondition&&) = delete;

private:
    const std::string referenceEntityName;
    const double targetTTC;
    const Rule rule;
};

class OPENPASSCOMMONEXPORT ReachPositionCondition :  public ByEntityCondition
{
public:
    ReachPositionCondition(const std::vector<std::string>& triggeringEntityNames,
                           const double tolerance):
        ByEntityCondition(triggeringEntityNames),
        tolerance(tolerance)
    {}
    ReachPositionCondition(const ReachPositionCondition&) = default;
    ~ReachPositionCondition();

    ReachPositionCondition() = delete;
    ReachPositionCondition(ReachPositionCondition&&) = delete;
    ReachPositionCondition& operator=(const ReachPositionCondition&) = delete;
    ReachPositionCondition& operator=(ReachPositionCondition&&) = delete;
protected:
    const double tolerance{};
};

class OPENPASSCOMMONEXPORT RelativeSpeedCondition : public ByEntityCondition
{
public:
    RelativeSpeedCondition(const std::vector<std::string> &triggeringEntityNames,
                           const std::string &referenceEntityName,
                           const double tolerance,
                           const Rule rule):
        ByEntityCondition(triggeringEntityNames),
        referenceEntityName(referenceEntityName),
        tolerance(tolerance),
        rule(rule)
    {}
    RelativeSpeedCondition(const RelativeSpeedCondition&) = default;
    ~RelativeSpeedCondition();

    std::vector<const AgentInterface*> IsMet(WorldInterface * const world) const;

    RelativeSpeedCondition() = delete;
    RelativeSpeedCondition(RelativeSpeedCondition&&) = delete;
    RelativeSpeedCondition& operator=(const RelativeSpeedCondition&) = delete;
    RelativeSpeedCondition& operator=(RelativeSpeedCondition&&) = delete;

private:
    const std::string referenceEntityName{};
    const double tolerance{};
    const Rule rule{};
};

class OPENPASSCOMMONEXPORT ReachPositionRoadCondition : public ReachPositionCondition
{
public:
    ReachPositionRoadCondition(const std::vector<std::string> &triggeringEntityNames,
                               const double tolerance,
                               const double targetSCoordinate,
                               const std::string& targetRoadId):
        ReachPositionCondition(triggeringEntityNames,
                               tolerance),
        targetSCoordinate(targetSCoordinate),
        targetRoadId(targetRoadId)
    {
        if (targetSCoordinate < 0)
        {
            throw std::runtime_error("Reach Position Target S Coordinate must be greater than or equal to 0");
        }
        else if (tolerance < 0)
        {
            throw std::runtime_error("Reach Position Tolerance must be greater than or equal to 0");
        }
    }
    ReachPositionRoadCondition(const ReachPositionRoadCondition&) = default;
    ~ReachPositionRoadCondition();

    std::vector<const AgentInterface*> IsMet(WorldInterface * const world) const;

    ReachPositionRoadCondition() = delete;
    ReachPositionRoadCondition(ReachPositionRoadCondition&&) = delete;
    ReachPositionRoadCondition& operator=(const ReachPositionRoadCondition&) = delete;
    ReachPositionRoadCondition& operator=(ReachPositionRoadCondition&&) = delete;

private:
    const double targetSCoordinate;
    const std::string targetRoadId;
};

class OPENPASSCOMMONEXPORT RelativeLaneCondition: public ReachPositionCondition
{
public:
    RelativeLaneCondition(const std::vector<std::string> &triggeringEntities,
                          const std::string &referenceEntityName,
                          const int deltaLane,
                          const double deltaS,
                          const double tolerance):
        ReachPositionCondition(triggeringEntities,
                               tolerance),
        referenceEntityName(referenceEntityName),
        deltaLane(deltaLane),
        deltaS(deltaS)
    {
        if (tolerance < 0)
        {
            throw std::runtime_error("RelativeLaneCondition tolerance must be greater than or equal to 0");
        }
    }
    RelativeLaneCondition(const RelativeLaneCondition&) = default;
    ~RelativeLaneCondition();

    std::vector<const AgentInterface*> IsMet(WorldInterface * const world) const;

    RelativeLaneCondition() = delete;
    RelativeLaneCondition(RelativeLaneCondition&&) = delete;
    RelativeLaneCondition& operator=(const RelativeLaneCondition&) = delete;
    RelativeLaneCondition& operator=(RelativeLaneCondition&&) = delete;

private:
    const std::string referenceEntityName;
    const int deltaLane;
    const double deltaS;
};

// OpenScenario ByValue Conditions
class OPENPASSCOMMONEXPORT ByValueCondition
{
public:
    ByValueCondition(const Rule rule):
        rule(rule)
    {}
    ByValueCondition(const ByValueCondition&) = default;
    virtual ~ByValueCondition();

    ByValueCondition(ByValueCondition&&) = delete;
    ByValueCondition& operator=(const ByValueCondition&) = delete;
    ByValueCondition& operator=(ByValueCondition&&) = delete;
protected:
    const Rule rule;
};

class OPENPASSCOMMONEXPORT SimulationTimeCondition : public ByValueCondition
{
public:
    SimulationTimeCondition(const Rule rule,
                            const double targetValueInSeconds):
        ByValueCondition(rule),
        targetValue(static_cast<int>(targetValueInSeconds * 1000.0))
    {}
    SimulationTimeCondition(const SimulationTimeCondition&) = default;
    ~SimulationTimeCondition();

    bool IsMet(const int value) const;
    int GetTargetValue() const;

    SimulationTimeCondition() = delete;
    SimulationTimeCondition(SimulationTimeCondition&&) = delete;
    SimulationTimeCondition& operator=(const SimulationTimeCondition&) = delete;
    SimulationTimeCondition& operator=(SimulationTimeCondition&&) = delete;

private:
    const int targetValue;
};

using Condition = std::variant<ReachPositionRoadCondition,
                               RelativeLaneCondition,
                               RelativeSpeedCondition,
                               SimulationTimeCondition,
                               TimeToCollisionCondition>;
using ConditionCollection = std::vector<Condition>;

struct ConditionalEventDetectorInformation
{
    ActorInformation actorInformation{};
    int numberOfExecutions{};
    std::string sequenceName{};
    ConditionCollection conditions{};
};

} // openScenario
