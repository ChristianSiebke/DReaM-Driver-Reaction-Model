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

#include "Interfaces/agentBlueprintInterface.h"
#include "Interfaces/worldInterface.h"

#include "SpawnPointDefinitions.h"

using namespace SpawnPointDefinitions;

enum class Direction
{
    FORWARD = 0,
    BACKWARD
};

class WorldAnalyzer
{
public:
    WorldAnalyzer(WorldInterface * const world):
        world{world},
        loggingCallback([](const std::string&) {})
    {}
    WorldAnalyzer(WorldInterface * const world,
                  std::function<void(const std::string&)> loggingCallback):
        world{world},
        loggingCallback(loggingCallback)
    {}

    WorldAnalyzer(const WorldAnalyzer&) = delete;
    WorldAnalyzer(WorldAnalyzer&&) = delete;
    WorldAnalyzer& operator=(const WorldAnalyzer&) = delete;
    WorldAnalyzer& operator=(WorldAnalyzer&&) = delete;
    ~WorldAnalyzer() = default;

    std::optional<ValidLaneSpawningRanges> GetValidLaneSpawningRanges(const Route& routeForRoadId,
                                                                      const RoadId& roadId,
                                                                      const LaneId laneId,
                                                                      const SPosition sStart,
                                                                      const SPosition sEnd) const;

    std::optional<double> GetNextSpawnPosition(const Route& routeForRoadId,
                                               const RoadId& roadId,
                                               const LaneId laneId,
                                               const Range& bounds,
                                               const double agentFrontLength,
                                               const double agentRearLength,
                                               const double intendedVelocity,
                                               const double gapInSeconds,
                                               const Direction direction) const;

    double CalculateSpawnVelocityToPreventCrashing(const Route& routeForRoadId,
                                                   const RoadId& roadId,
                                                   const LaneId laneId,
                                                   const double intendedSpawnPosition,
                                                   const double agentFrontLength,
                                                   const double agentRearLength,
                                                   const double intendedVelocity) const;

    bool ValidMinimumSpawningDistanceToObjectInFront(const RoadId& roadId,
                                                     const LaneId laneId,
                                                     const SPosition sPosition,
                                                     const VehicleModelParameters& vehicleModelParameters) const;

    bool AreSpawningCoordinatesValid(const RoadId& roadId,
                                     const LaneId laneId,
                                     const SPosition sPosition,
                                     const double offset,
                                     const VehicleModelParameters& vehicleModelParameters) const;

    bool SpawnWillCauseCrash(const Route& route,
                             const RoadId& roadId,
                             const LaneId laneId,
                             const SPosition sPosition,
                             const double agentFrontLength,
                             const double agentRearLength,
                             const double velocity,
                             const Direction direction) const;

private:
    static std::optional<ValidLaneSpawningRanges> GetValidSpawningInformationForRange(const double sStart,
                                                                                      const double sEnd,
                                                                                      const double firstScenarioAgentSPosition,
                                                                                      const double lastScenarioAgentSPosition);

    bool IsOffsetValidForLane(const RoadId& roadId,
                              const LaneId laneId,
                              const SPosition distanceFromStart,
                              const double offset,
                              const double vehicleWidth) const;

    bool NewAgentIntersectsWithExistingAgent(const RoadId& roadId,
                                             const LaneId laneId,
                                             const SPosition sPosition,
                                             const double offset,
                                             const VehicleModelParameters& vehicleModelParameters) const;

    WorldInterface * const world;
    const std::function<void(const std::string&)> loggingCallback;
};
