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

//! This class provides utility functions for spawners to query information about the world
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

    //! Returns the ranges where the spawner can spawn common ranges.
    //! It excludes s positions where a scenario agent is standing as well as the range
    //! inbetween two scenario agents.
    //!
    //! \param roadId       Id of the road
    //! \param laneId       Id of the lane
    //! \param sStart       start of the search range
    //! \param sEnd         end of the search range
    //! \return valid spawning ranges
    std::optional<ValidLaneSpawningRanges> GetValidLaneSpawningRanges(const RoadId& roadId,
                                                                      const LaneId laneId,
                                                                      const SPosition sStart,
                                                                      const SPosition sEnd) const;

    //! Calculates the next s coordinate to spawn an agents on
    //!
    //! \param roadId               Id of the road
    //! \param laneId               Id of the lane
    //! \param bounds               s range to spawn in
    //! \param agentFrontLength     distance from reference point to front of agent
    //! \param agentRearLength      distance from reference point to rear of agent
    //! \param intendedVelocity     spawning velocity
    //! \param gapInSeconds         desired TGap between spawned agent and next agent
    //! \param direction            wether to spawn from front to end of the road or from end to front
    //! \return s position
    std::optional<double> GetNextSpawnPosition(const RoadId& roadId,
                                               const LaneId laneId,
                                               const Range& bounds,
                                               const double agentFrontLength,
                                               const double agentRearLength,
                                               const double intendedVelocity,
                                               const double gapInSeconds,
                                               const Direction direction) const;


    //! Adjust spawning velocity so that the spawned agent won't immediately crash.
    //!
    //! \param roadId                   Id of the road
    //! \param laneId                   Id of the lane
    //! \param intendedSpawnPosition    s coordinate
    //! \param agentFrontLength         distance from reference point to front of agent
    //! \param agentRearLength          distance from reference point to rear of agent
    //! \param intendedVelocity         spawning velocity
    //! \return possibly adjusted velocity
    double CalculateSpawnVelocityToPreventCrashing(const RoadId& roadId,
                                                   const LaneId laneId,
                                                   const double intendedSpawnPosition,
                                                   const double agentFrontLength,
                                                   const double agentRearLength,
                                                   const double intendedVelocity) const;

    //! Check wether the minimum distance the next object is satisfied
    bool ValidMinimumSpawningDistanceToObjectInFront(const RoadId& roadId,
                                                     const LaneId laneId,
                                                     const SPosition sPosition,
                                                     const VehicleModelParameters& vehicleModelParameters) const;

    //! Check if it is allowed the spawn an agent at the given coordinate
    bool AreSpawningCoordinatesValid(const RoadId& roadId,
                                     const LaneId laneId,
                                     const SPosition sPosition,
                                     const double offset,
                                     const VehicleModelParameters& vehicleModelParameters) const;

    //! Check wether spawning an agent with given parameters will result in a crash
    bool SpawnWillCauseCrash(const RoadId& roadId,
                             const LaneId laneId,
                             const SPosition sPosition,
                             const double agentFrontLength,
                             const double agentRearLength,
                             const double velocity,
                             const Direction direction) const;

    //! Returns the number of lanes to the right the given lane at the given s coordinate
    //!
    //! \param roadId       Id of the road
    //! \param laneId       Id of the lane
    //! \param sPosition    s coordinate
    //! \return number of lanes to the right
    size_t GetRightLaneCount(const RoadId& roadId, const LaneId& laneId, const double sPosition) const;

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
