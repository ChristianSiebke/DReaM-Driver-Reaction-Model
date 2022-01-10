/********************************************************************************
 * Copyright (c) 2019-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <string>
#include <vector>
#include "common/commonTools.h"
#include "include/parameterInterface.h"
#include "include/callbackInterface.h"
#include "framework/sampler.h"

namespace SpawnPointDefinitions
{
constexpr char SPAWNPOINTS[] = {"SpawnPoints"};
constexpr char SPAWNZONES[] = {"SpawnZones"};
constexpr char ROADS[] = {"Roads"};
constexpr char LANES[] = {"Lanes"};
constexpr double DEFAULT_MINIMUM_SEPARATION_BUFFER = 5.0;

using RoadId = std::string;
using RoadIds = std::vector<RoadId>;
using LaneId = int;
using LaneIds = std::vector<LaneId>;
using SPosition = double;
using Range = std::pair<SPosition, SPosition>;
using Ranges = std::vector<Range>;
using VehicleRearAndFrontCoordinates = std::pair<SPosition, SPosition>;

//! Defines a AgentProfile for spawning and its associated spawn parameters
struct SpawningAgentProfile
{
    const std::string name;
    const openpass::parameter::StochasticDistribution velocity;
    const std::vector<double> homogeneities;
    const openpass::parameter::StochasticDistribution tGap;

    bool operator== (const SpawningAgentProfile& other) const
    {
        return this->name == other.name
                && this->velocity == other.velocity
                && this->homogeneities == other.homogeneities
                && this->tGap == other.tGap;
    }
};

using AgentProfiles = std::vector<std::pair<SpawningAgentProfile, double>>;

//! Defines possible AgentProfiles for a SpawnPoint
struct AgentProfileLaneMaps
{
    AgentProfiles leftLanes;
    AgentProfiles rightLanes;
};

#define SPAWNER_THROWIFFALSE(success,message) if (!(success)) { callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, message); throw std::runtime_error(message); }

//! Read the (fixed or stochastic) MinimumSeparationBuffer parameter or sets it to the default value if not defined
//!
//! \param parameter SpawnPointParameter
//! \return MinimumSeparationBuffer
static std::variant<double, openpass::parameter::StochasticDistribution> ExtractMinimumSpawnBuffer(const ParameterInterface &parameter)
{
    const auto& separationBufferStochastic = helper::map::query(parameter.GetParametersStochastic(), "MinimumSeparationBuffer");
    if (separationBufferStochastic.has_value())
    {
        return separationBufferStochastic.value();
    }
    const auto& separationBufferDouble = helper::map::query(parameter.GetParametersDouble(), "MinimumSeparationBuffer");
    return separationBufferDouble.value_or(DEFAULT_MINIMUM_SEPARATION_BUFFER);
}

//! Reads the possible AgentProfiles and their spawn parameters from the Spawner profile
//!
//! \param parameter    Spawner profile
//! \param callbacks    CallbackInterface for logging
//! \return AgentProfile
static AgentProfileLaneMaps ExtractAgentProfileLaneMaps(const ParameterInterface &parameter, const CallbackInterface* callbacks)
{
    using namespace helper;

    AgentProfileLaneMaps agentProfileLaneMaps;

    const auto& trafficGroupsList = map::query(parameter.GetParameterLists(),"TrafficGroups");

    for (const auto& trafficGroupParameter : trafficGroupsList.value())
    {
        const auto weightOfGroup = map::query(trafficGroupParameter->GetParametersDouble(), "Weight");
        const auto velocity = map::query(trafficGroupParameter->GetParametersStochastic(), "Velocity");
        const auto homogeneity = map::query(trafficGroupParameter->GetParametersDoubleVector(), "Homogeneity");
        const auto tGap = map::query(trafficGroupParameter->GetParametersStochastic(), "TGap");
        const auto rightLaneOnly = map::query(trafficGroupParameter->GetParametersBool(), "RightLaneOnly");

        const auto& agentProfilesList = map::query(trafficGroupParameter->GetParameterLists(),"AgentProfiles");

        SPAWNER_THROWIFFALSE(velocity.has_value(), "No velocity provided in TrafficGroup for SpawnerRuntimeCommon")
        SPAWNER_THROWIFFALSE(tGap.has_value(), "No t gap provided in TrafficGroup for SpawnerRuntimeCommon")
        SPAWNER_THROWIFFALSE(agentProfilesList.has_value(), "No AgentProfile provided in TrafficGroup for SpawnerRuntimeCommon")
        for (const auto& agentProfileParameter : agentProfilesList.value())
        {
            const auto weightOfProfile = map::query(agentProfileParameter->GetParametersDouble(), "Weight");
            const auto name = map::query(agentProfileParameter->GetParametersString(), "Name");
            SPAWNER_THROWIFFALSE(name.has_value(), "No AgentProfile name provided in TrafficGroup for SpawnerRuntimeCommon")
            agentProfileLaneMaps.rightLanes.push_back(std::make_pair(SpawningAgentProfile{name.value(), velocity.value(),
                                                                                 homogeneity.value_or(std::vector<double>{1.0}), tGap.value()},
                                                            weightOfGroup.value_or(1.0) * weightOfProfile.value_or(1.0)));
            if (!rightLaneOnly.value_or(false))
            {
                agentProfileLaneMaps.leftLanes.push_back(std::make_pair(SpawningAgentProfile{name.value(), velocity.value(),
                                                                                     homogeneity.value_or(std::vector<double>{1.0}), tGap.value()},
                                                                weightOfGroup.value_or(1.0) * weightOfProfile.value_or(1.0)));
            }
        }
    }

    return agentProfileLaneMaps;
}

static double GetStochasticOrPredefinedValue (std::variant<double, openpass::parameter::StochasticDistribution> value, StochasticsInterface* stochastics)
{
    if (std::holds_alternative<openpass::parameter::StochasticDistribution>(value))
    {
        return Sampler::RollForStochasticAttribute(std::get<openpass::parameter::StochasticDistribution>(value), stochastics);
    }
    else
    {
        return std::get<double>(value);
    }
}

} // namespace SpawnPointDefinitions
