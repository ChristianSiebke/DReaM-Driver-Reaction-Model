#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Interfaces/parameterInterface.h"

class AgentBlueprint;

namespace SpawnPointRuntimeCommonDefinitions
{
    using RoadId = std::string;
    using LaneId = int;
    using LaneIds = std::vector<LaneId>;
    using SPosition = double;
    using Range = std::pair<SPosition, SPosition>;
    using ValidLaneSpawningRanges = std::vector<Range>;
    using VehicleRearAndFrontCoordinates = std::pair<SPosition, SPosition>;
    using CarsPerSecond = double;
    using PlatoonRate = double;
    using TrafficVelocityDistribution = openpass::parameter::NormalDistribution;
    using StringProbabilities = std::unordered_map<std::string, double>;
    using GapInSeconds = double;
    using SpawnDetails = std::pair<GapInSeconds, AgentBlueprint>;

    struct SpawnPointRuntimeCommonParameters
    {
        SpawnPointRuntimeCommonParameters(const RoadId& roadId,
                                          const LaneIds& laneIds,
                                          const SPosition spawnLocation,
                                          const CarsPerSecond carsPerSecond,
                                          const PlatoonRate platoonRate,
                                          const TrafficVelocityDistribution& trafficVelocityDistribution,
                                          const StringProbabilities& agentProfiles):
            roadId{roadId},
            laneIds{laneIds},
            spawnLocation{spawnLocation},
            carsPerSecond{carsPerSecond},
            platoonRate{platoonRate},
            trafficVelocityDistribution{trafficVelocityDistribution},
            agentProfiles{agentProfiles}
        {}

        const RoadId roadId;
        const LaneIds laneIds;
        const SPosition spawnLocation;
        const CarsPerSecond carsPerSecond;
        const PlatoonRate platoonRate;
        const TrafficVelocityDistribution trafficVelocityDistribution;
        const StringProbabilities agentProfiles;
    };
}
