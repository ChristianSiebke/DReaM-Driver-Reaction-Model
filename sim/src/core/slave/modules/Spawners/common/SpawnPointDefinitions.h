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

#include <string>
#include <vector>
#include "include/parameterInterface.h"
#include "common/commonTools.h"
#include "common/log.h"

namespace SpawnPointDefinitions
{
constexpr char SPAWNPOINTS[] = {"SpawnPoints"};
constexpr char ROADS[] = {"Roads"};
constexpr char LANES[] = {"Lanes"};

using RoadId = std::string;
using RoadIds = std::vector<RoadId>;
using LaneId = int;
using LaneIds = std::vector<LaneId>;
using SPosition = double;
using Range = std::pair<SPosition, SPosition>;
using ValidLaneSpawningRanges = std::vector<Range>;
using VehicleRearAndFrontCoordinates = std::pair<SPosition, SPosition>;

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

struct AgentProfileLaneMaps
{
    AgentProfiles leftLanes;
    AgentProfiles rightLanes;
};

static AgentProfileLaneMaps ExtractAgentProfileLaneMaps(const ParameterInterface &parameter)
{
    using namespace helper;

    AgentProfileLaneMaps agentProfileLaneMaps;

    const auto& trafficGroupsList = map::query(parameter.GetParameterLists(),"TrafficGroups");
    ThrowIfFalse(trafficGroupsList.has_value(), "No TrafficGroups provided for SpawnPointRuntimeCommon");

    for (const auto& trafficGroupParameter : trafficGroupsList.value())
    {
        const auto weightOfGroup = map::query(trafficGroupParameter->GetParametersDouble(), "Weight");
        const auto velocity = map::query(trafficGroupParameter->GetParametersStochastic(), "Velocity");
        const auto homogeneity = map::query(trafficGroupParameter->GetParametersDoubleVector(), "Homogeneity");
        const auto tGap = map::query(trafficGroupParameter->GetParametersStochastic(), "TGap");
        const auto rightLaneOnly = map::query(trafficGroupParameter->GetParametersBool(), "RightLaneOnly");

        const auto& agentProfilesList = map::query(trafficGroupParameter->GetParameterLists(),"AgentProfiles");

        ThrowIfFalse(velocity.has_value(), "No velocity provided in TrafficGroup for SpawnPointRuntimeCommon");
        ThrowIfFalse(tGap.has_value(), "No t gap provided in TrafficGroup for SpawnPointRuntimeCommon");
        ThrowIfFalse(agentProfilesList.has_value(), "No AgentProfile provided in TrafficGroup for SpawnPointRuntimeCommon");
        for (const auto& agentProfileParameter : agentProfilesList.value())
        {
            const auto weightOfProfile = map::query(agentProfileParameter->GetParametersDouble(), "Weight");
            const auto name = map::query(agentProfileParameter->GetParametersString(), "Name");
            ThrowIfFalse(name.has_value(), "No AgentProfile name provided in TrafficGroup for SpawnPointRuntimeCommon");
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

} // namespace SpawnPointDefinitions
