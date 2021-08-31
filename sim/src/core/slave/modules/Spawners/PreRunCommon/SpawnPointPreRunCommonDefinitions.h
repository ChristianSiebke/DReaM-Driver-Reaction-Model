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
#include "common/SpawnPointDefinitions.h"
#include "common/commonTools.h"

namespace SpawnPointPreRunCommonDefinitions
{
    using namespace SpawnPointDefinitions;

    struct SpawnArea
    {
        const RoadIds roadIds;
        const LaneIds laneIds;
        const SPosition sStart;
        const SPosition sEnd;

        bool operator== (const SpawnArea& other) const
        {
            return this->roadIds == other.roadIds
                    && this->laneIds == other.laneIds
                    && CommonHelper::DoubleEquality(this->sStart, other.sStart)
                    && CommonHelper::DoubleEquality(this->sEnd, other.sEnd);
        }
    };

    //!Parameters of the PreRunCommonSpawner
    struct PreRunSpawnerParameters
    {
        const std::variant<double, openpass::parameter::StochasticDistribution> minimumSeparationBuffer; //!< Minimum distance between two agents
        const std::vector<SpawnArea> spawnAreas; //!< Areas to spawn in
        const AgentProfileLaneMaps agentProfileLaneMaps; //!< AgentProfiles to spawn, separated for left lane and other lanes
    };
} // SpawnPointPreRunDefinitions
