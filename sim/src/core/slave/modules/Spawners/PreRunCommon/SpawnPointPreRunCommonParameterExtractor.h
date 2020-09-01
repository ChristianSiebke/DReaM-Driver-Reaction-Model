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

#include "include/parameterInterface.h"
#include "common/commonTools.h"
#include "common/SpawnPointDefinitions.h"
#include "SpawnPointPreRunCommonDefinitions.h"

using namespace SpawnPointPreRunCommonDefinitions;

constexpr char S_START[] = {"SStart"};
constexpr char S_END[] = {"SEnd"};

static std::vector<SpawnArea> ExtractSpawnAreas(const ParameterInterface &parameter)
{
    using namespace helper;

    std::vector<SpawnArea> spawnAreas;

    const auto& spawnPointList = map::query(parameter.GetParameterLists(), SPAWNPOINTS);
    ThrowIfFalse(spawnPointList.has_value(), "No SpawnPoint provided for SpawnPointPreRunCommon");

    for (const auto& spawnPointParameter : spawnPointList.value())
    {
        const auto roadIdElement = map::query(spawnPointParameter->GetParametersString(), ROAD);
        const auto laneIdsElement = map::query(spawnPointParameter->GetParametersIntVector(), LANES);
        const auto sStartElement = map::query(spawnPointParameter->GetParametersDouble(), S_START);
        const auto sEndElement = map::query(spawnPointParameter->GetParametersDouble(), S_END);

        ThrowIfFalse(roadIdElement.has_value(), "No road id provided in SceneryInformation for SpawnPointRuntimeCommon");
        ThrowIfFalse(laneIdsElement.has_value(), "No lane id provided in SceneryInformation for SpawnPointRuntimeCommon");
        ThrowIfFalse(sStartElement.has_value(), "No S-Start provided in SceneryInformation for SpawnPointRuntimeCommon");
        ThrowIfFalse(sEndElement.has_value(), "No S-End provided in SceneryInformation for SpawnPointRuntimeCommon");

        std::vector<int> sortedLaneIds(laneIdsElement.value());
        if (sortedLaneIds.front() < 0)
        {
            std::sort(sortedLaneIds.begin(), sortedLaneIds.end());
        }
        else
        {
            std::sort(sortedLaneIds.begin(), sortedLaneIds.end(), std::greater<int>{});
        }

        spawnAreas.emplace_back(SpawnArea{roadIdElement.value(),
                                           sortedLaneIds,
                                           sStartElement.value(),
                                           sEndElement.value()});
    }

    return spawnAreas;
}

static PreRunSpawnerParameters ExtractSpawnPointParameters(const ParameterInterface &parameter)
{
    return {ExtractSpawnAreas(parameter), ExtractAgentProfileLaneMaps(parameter)};
}
