/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
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
#include "Interfaces/parameterInterface.h"
#include "Common/SpawnPointDefinitions.h"
#include "Common/commonTools.h"

namespace SpawnPointPreRunCommonDefinitions
{
    using namespace SpawnPointDefinitions;

    struct SpawnArea
    {
        const RoadId roadId;
        const LaneIds laneIds;
        const SPosition sStart;
        const SPosition sEnd;

        bool operator== (const SpawnArea& other) const
        {
            return this->roadId == other.roadId
                    && this->laneIds == other.laneIds
                    && CommonHelper::DoubleEquality(this->sStart, other.sStart)
                    && CommonHelper::DoubleEquality(this->sEnd, other.sEnd);
        }
    };

    struct PreRunSpawnerParameters
    {
        const std::vector<SpawnArea> spawnAreas;
        const AgentProfileLaneMaps agentProfileLaneMaps;
    };
} // SpawnPointPreRunDefinitions
