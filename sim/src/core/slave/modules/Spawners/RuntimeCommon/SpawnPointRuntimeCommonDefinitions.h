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

    struct SpawnPointRuntimeCommonParameters
    {
        const std::vector<SpawnPosition> spawnPositions;
        const SpawnPointDefinitions::AgentProfileLaneMaps agentProfileLaneMaps;
    };
}
