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

static std::vector<SpawnArea> ExtractSpawnAreas(const ParameterInterface &parameter, const CallbackInterface* callbacks)
{
    using namespace helper;

    std::vector<SpawnArea> spawnAreas;

    const auto& spawnPointList = map::query(parameter.GetParameterLists(), SPAWNPOINTS);
    SPAWNER_THROWIFFALSE(spawnPointList.has_value(), "No SpawnPoint provided for SpawnPointPreRunCommon");

    for (const auto& spawnPointParameter : spawnPointList.value())
    {
        const auto roadIdsElement = map::query(spawnPointParameter->GetParametersStringVector(), ROADS);
        const auto laneIdsElement = map::query(spawnPointParameter->GetParametersIntVector(), LANES);
        const auto sStartElement = map::query(spawnPointParameter->GetParametersDouble(), S_START);
        const auto sEndElement = map::query(spawnPointParameter->GetParametersDouble(), S_END);

        SPAWNER_THROWIFFALSE(roadIdsElement.has_value(), "No road ids provided in SceneryInformation for SpawnPointPreRunCommon");
        SPAWNER_THROWIFFALSE(laneIdsElement.has_value(), "No lane ids provided in SceneryInformation for SpawnPointPreRunCommon");
        SPAWNER_THROWIFFALSE(sStartElement.has_value(), "No S-Start provided in SceneryInformation for SpawnPointPreRunCommon");
        SPAWNER_THROWIFFALSE(sEndElement.has_value(), "No S-End provided in SceneryInformation for SpawnPointPreRunCommon");

        std::vector<int> sortedLaneIds(laneIdsElement.value());
        if (sortedLaneIds.front() < 0)
        {
            std::sort(sortedLaneIds.begin(), sortedLaneIds.end());
        }
        else
        {
            std::sort(sortedLaneIds.begin(), sortedLaneIds.end(), std::greater<int>{});
        }

        spawnAreas.emplace_back(SpawnArea{roadIdsElement.value(),
                                           sortedLaneIds,
                                           sStartElement.value(),
                                           sEndElement.value()});
    }

    return spawnAreas;
}

static PreRunSpawnerParameters ExtractSpawnPointParameters(const ParameterInterface &parameter, const CallbackInterface* callbacks)
{
    return {ExtractSpawnAreas(parameter, callbacks), ExtractAgentProfileLaneMaps(parameter, callbacks)};
}
