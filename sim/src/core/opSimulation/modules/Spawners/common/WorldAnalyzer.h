/********************************************************************************
 * Copyright (c) 2019-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "SpawnerDefinitions.h"
#include "include/agentBlueprintInterface.h"
#include "include/stochasticsInterface.h"
#include "include/worldInterface.h"

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
    //! \param laneStream           LaneStream to spawn in
    //! \param sStart               start of the search range
    //! \param sEnd                 end of the search range
    //! \param supportedLaneTypes   Container of all valid lanetypes
    //! \return valid spawning ranges
    Ranges GetValidLaneSpawningRanges(const std::unique_ptr<LaneStreamInterface>& laneStream,
                                      const SPosition sStart,
                                      const SPosition sEnd,
                                      const LaneTypes& supportedLaneTypes) const;

    //! Calculates the next s coordinate to spawn an agent on
    //!
    //! \param laneStream               LaneStream to spawn in
    //! \param bounds                   s range to spawn in
    //! \param agentFrontLength         distance from reference point to front of agent
    //! \param agentRearLength          distance from reference point to rear of agent
    //! \param intendedVelocity         spawning velocity
    //! \param gapInSeconds             desired TGap between spawned agent and next agent
    //! \param minimumSeparationBuffer  minimum distance between agents
    //! \return s position or nullopt if this would be outside the range
    std::optional<double> GetNextSpawnPosition(const std::unique_ptr<LaneStreamInterface> &laneStream,
                                               const Range& bounds,
                                               const double agentFrontLength,
                                               const double agentRearLength,
                                               const double intendedVelocity,
                                               const double gapInSeconds,
                                               const double minimumSeparationBuffer) const;

    //! Calculates the adjusted spawn distance such that the minimum ttc to the end of lane is not violated
    //!
    //! \param laneId                   lane to spawn
    //! \param sCoordinate              s coordinate on road
    //! \param intendedSpawnPosition    initially calcuted spawn distance on stream
    //! \param intendedVelocity         velocity of the agent
    //! \param route                    route of the agent
    //! \param supportedLaneTypes   Container of all valid lanetypes
    //! \return adjusted spawn distance
    double CalculateAdjustedSpawnDistanceToEndOfLane(const LaneId laneId,
                                                     const double sCoordinate,
                                                     const double intendedSpawnPosition,
                                                     const double intendedVelocity,
                                                     const Route& route,
                                                     const LaneTypes& supportedLaneTypes) const;

    //! Adjust spawning velocity so that the spawned agent won't immediately crash.
    //!
    //! \param roadId                   Id of the road
    //! \param laneId                   Id of the lane
    //! \param intendedSpawnPosition    s coordinate
    //! \param agentFrontLength         distance from reference point to front of agent
    //! \param agentRearLength          distance from reference point to rear of agent
    //! \param intendedVelocity         spawning velocity
    //! \param route                    initial route of the agent
    //! \return possibly adjusted velocity
    double CalculateSpawnVelocityToPreventCrashing(const RoadId& roadId,
                                                   const LaneId laneId,
                                                   const double intendedSpawnPosition,
                                                   const double agentFrontLength,
                                                   const double agentRearLength,
                                                   const double intendedVelocity,
                                                   const Route& route) const;


    bool TrafficJamAtSpwanPoint(const RoadId &roadId, const LaneId laneId, const double intendedSpawnPosition, const double agentFrontLength, const double agentRearLength, const double intendedVelocity, const Route &route) const;


    //! Adjust spawning velocity so that the spawned agent won't immediately crash.
    //!
    //! \param laneStream               LaneStream to spawn in
    //! \param intendedSpawnPosition    s coordinate
    //! \param agentFrontLength         distance from reference point to front of agent
    //! \param agentRearLength          distance from reference point to rear of agent
    //! \param intendedVelocity         spawning velocity
    //! \return possibly adjusted velocity
    double CalculateSpawnVelocityToPreventCrashing(const std::unique_ptr<LaneStreamInterface> &laneStream,
                                                   const double intendedSpawnPosition,
                                                   const double agentFrontLength,
                                                   const double agentRearLength,
                                                   const double intendedVelocity) const;

    //! Check wether the minimum distance the next object is satisfied
    //!
    //! \param laneId                   Id of the lane
    //! \param sPosition                s coordinate of own agent
    //! \param route                    initial route of the agent
    //! \param vehicleModelParameters   parameters of the vehicle model
    //! \param minimumSeparationBuffer  minimum distance between agents
    //! \return true if minimum distance is met
    bool ValidMinimumSpawningDistanceToObjectInFront(const LaneId laneId,
                                                     const SPosition sPosition,
                                                     const Route& route,
                                                     const VehicleModelParameters& vehicleModelParameters,
                                                     const double minimumSeparationBuffer) const;

    //! Check if it is allowed the spawn an agent at the given coordinate
    //!
    //! \param roadId                   Id of the road
    //! \param laneId                   Id of the lane
    //! \param sPosition                s coordinate of own agent
    //! \param offset                   lateral position in lane
    //! \param minimumSeparationBuffer  minimum distance between agents
    //! \param route                    initial route of the agent
    //! \param vehicleModelParameters   parameters of the vehicle model
    //! \true if spawning coordinates are valid
    bool AreSpawningCoordinatesValid(const RoadId& roadId,
                                     const LaneId laneId,
                                     const SPosition sPosition,
                                     const double offset,
                                     const double minimumSeparationBuffer,
                                     const Route &route,
                                     const VehicleModelParameters& vehicleModelParameters) const;

    //! Check wether spawning an agent with given parameters will result in a crash
    //!
    //! \param laneStream               LaneStream to spawn in
    //! \param sPosition                s coordinate
    //! \param agentFrontLength         distance from reference point to front of agent
    //! \param agentRearLength          distance from reference point to rear of agent
    //! \param velocity                 spawning velocity
    //! \return true if spawning would cause a crash
    bool SpawnWillCauseCrash(const std::unique_ptr<LaneStreamInterface> &laneStream,
                             const SPosition sPosition,
                             const double agentFrontLength,
                             const double agentRearLength,
                             const double velocity,
                             const Direction direction) const;

    //! Returns the number of lanes to the right of the given lane at the given s coordinate
    //!
    //! \param roadId       Id of the road
    //! \param laneId       Id of the lane
    //! \param sPosition    s coordinate
    //! \return number of lanes to the right
    size_t GetRightLaneCount(const RoadId& roadId, const LaneId& laneId, const double sPosition) const;

    //! Returns the number of lanes to the right the given lane at the given s coordinate
    //!
    //! \param laneStream   LaneStream to spawn in
    //! \param sPosition    s coordinate
    //! \return number of lanes to the right
    size_t GetRightLaneCount(const std::unique_ptr<LaneStreamInterface> &laneStream, const double sPosition) const;

    //! Checks if a roadId exists in direction of a laneId
    //!
    //! \param roadId       Id of the road
    //! \param laneId       Id of the lane
    //! \param distanceOnLane   s Coordinate along road
    //! \param validLaneTypes   Container of all valid lane types
    //!
    //! \return true, if roadId exists in laneId direction
    bool ValidateRoadIdInDirection(const RoadId& roadId,
                                   const LaneId laneId,
                                   const double distanceOnLane,
                                   const LaneTypes& validLaneTypes) const;

    //! Randomly generates a route based on a starting roadId
    //!
    //! \param roadId       Id of the road
    //! \param laneId       Id of the lane
    //! \param stochastics  Stochastics module which randomizes the route
    //! \return route
    Route SampleRoute(const RoadId& roadId,
                      const LaneId laneId,
                      StochasticsInterface* stochastics) const;

private:
    static Ranges GetValidSpawningInformationForRange(const double sStart,
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
