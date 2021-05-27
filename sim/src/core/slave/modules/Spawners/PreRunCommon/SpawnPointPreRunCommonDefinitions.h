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

    //! Defines an area in which agents should be spawned
    struct SpawnArea
    {
        std::unique_ptr<LaneStreamInterface> laneStream; //!< laneStream to spawn on
        const SPosition sStart; //! start position on the stream
        const SPosition sEnd; //! end position on the stream

        SpawnArea(std::unique_ptr<LaneStreamInterface> laneStream,
                  const SPosition sStart,
                  const SPosition sEnd) :
            sStart(sStart),
            sEnd(sEnd)
        {
            this->laneStream = std::move(laneStream);
        }
    };

    //!Parameters of the PreRunCommonSpawner
    struct PreRunSpawnerParameters
    {
        const std::variant<double, openpass::parameter::StochasticDistribution> minimumSeparationBuffer; //!< Minimum distance between two agents
        const std::vector<SpawnArea> spawnAreas; //!< Areas to spawn in
        const AgentProfileLaneMaps agentProfileLaneMaps; //!< AgentProfiles to spawn, separated for left lane and other lanes
    };

    //! position and parameters of an agent to spawn
    struct SpawnInfo
    {
        SpawnParameter spawnParameter;
        std::string roadId;
        int laneId;
        double s;
        double streamPosition;
    };
} // SpawnPointPreRunDefinitions
