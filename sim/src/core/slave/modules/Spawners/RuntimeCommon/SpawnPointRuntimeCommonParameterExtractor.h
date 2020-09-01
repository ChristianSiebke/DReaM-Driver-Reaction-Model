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

#include "SpawnPointRuntimeCommonDefinitions.h"

#include "common/commonTools.h"
#include "include/parameterInterface.h"
#include "common/SpawnPointDefinitions.h"
#include "common/log.h"

using namespace SpawnPointDefinitions;

namespace SpawnPointRuntimeCommonParameterExtractor
{
    using namespace SpawnPointRuntimeCommonDefinitions;

    constexpr char SCOORDINATE[] = {"SCoordinate"};

    static std::vector<SpawnPosition> ExtractSpawnPoints(const ParameterInterface &parameter)
    {
        using namespace helper;

        std::vector<SpawnPosition> spawnpoints;

        const auto& spawnPointList = map::query(parameter.GetParameterLists(),SPAWNPOINTS);
        ThrowIfFalse(spawnPointList.has_value(), "No SpawnPoint provided for SpawnPointRuntimeCommon");

        for (const auto& spawnPointParameter : spawnPointList.value())
        {
            const auto roadIdElement = map::query(spawnPointParameter->GetParametersString(), ROAD);
            const auto laneIdsElement = map::query(spawnPointParameter->GetParametersIntVector(), LANES);
            const auto sCoordinateElement = map::query(spawnPointParameter->GetParametersDouble(), SCOORDINATE);

            ThrowIfFalse(roadIdElement.has_value(), "No road id provided in SceneryInformation for SpawnPointRuntimeCommon");
            ThrowIfFalse(laneIdsElement.has_value(), "No lane id provided in SceneryInformation for SpawnPointRuntimeCommon");
            ThrowIfFalse(sCoordinateElement.has_value(), "No s coordinate provided in SceneryInformation for SpawnPointRuntimeCommon");

            std::vector<int> sortedLaneIds(laneIdsElement.value());
            if (sortedLaneIds.front() < 0)
            {
                std::sort(sortedLaneIds.begin(), sortedLaneIds.end());
            }
            else
            {
                std::sort(sortedLaneIds.begin(), sortedLaneIds.end(), std::greater<int>{});
            }

            for (const auto& laneId : sortedLaneIds)
            {
                spawnpoints.emplace_back(SpawnPosition{roadIdElement.value(), laneId, sCoordinateElement.value()});
            }
        }

        return spawnpoints;
    }

    /*!
     * \brief ExtractSpawnPointParameters extracts the parameters for the
     *        spawn point from the provided parameterInterface
     * \param parameterInterface the parameterInterface from which to extract
     * \param sampler the sampler with which to sample random values
     * \return the parameters for the spawn point
     */
    static SpawnPointRuntimeCommonParameters ExtractSpawnPointParameters(const ParameterInterface& parameter)
    {
        return {ExtractSpawnPoints(parameter), SpawnPointDefinitions::ExtractAgentProfileLaneMaps(parameter)};
    }
};
