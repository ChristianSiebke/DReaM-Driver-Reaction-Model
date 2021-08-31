/*******************************************************************************
* Copyright (c) 2019, 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "include/parameterInterface.h"
#include "agentBlueprint.h"
#include "common/SpawnPointDefinitions.h"

namespace SpawnPointRuntimeCommonDefinitions
{
    using RoadId = std::string;
    using LaneId = int;
    using LaneIds = std::vector<LaneId>;
    using SPosition = double;

    struct SpawnDetails
    {
        int spawnTime;
        AgentBlueprint agentBlueprint;
    };

    struct SpawnPosition
    {
        const RoadId roadId;
        const LaneId laneId;
        const SPosition sPosition;

        bool operator== (const SpawnPosition& other) const
        {
            return this->roadId == other.roadId
                    && this->laneId == other.laneId
                    && this->sPosition == other.sPosition;
        }
    };

    //! Parameters of the RuntimeCommonSpawner
    struct SpawnPointRuntimeCommonParameters
    {
        const std::variant<double, openpass::parameter::StochasticDistribution> minimumSeparationBuffer; //!< Minimum distance between two agents
        const std::vector<SpawnPosition> spawnPositions; //!< Areas to spawn in
        const SpawnPointDefinitions::AgentProfileLaneMaps agentProfileLaneMaps; //!< AgentProfiles to spawn, separated for left lane and other lanes
    };
}
