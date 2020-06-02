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

namespace SpawnPointPreRunCommonDefinitions
{
    using namespace SpawnPointDefinitions;
    struct SpawnPointParameters
    {
        SpawnPointParameters(const RoadId& roadId,
                             const LaneIds& laneIds,
                             const SPosition sStart,
                             const SPosition sEnd,
                             const double carsPerSecond,
                             const double platoonRate,
                             const openpass::parameter::NormalDistribution& trafficVelocityDistribution,
                             const StringProbabilities& agentProfiles):
            roadId{roadId},
            laneIds{laneIds},
            sStart{sStart},
            sEnd{sEnd},
            carsPerSecond{carsPerSecond},
            platoonRate{platoonRate},
            trafficVelocityDistribution{trafficVelocityDistribution},
            agentProfiles{agentProfiles}
        {}

        const RoadId roadId;
        const LaneIds laneIds;
        const SPosition sStart;
        const SPosition sEnd;
        const double carsPerSecond;
        const double platoonRate;
        const openpass::parameter::NormalDistribution trafficVelocityDistribution;
        const StringProbabilities agentProfiles;
    };
} // SpawnPointPreRunDefinitions
