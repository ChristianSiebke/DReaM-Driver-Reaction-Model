/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \addtogroup SpawnPoint
* @{
* \brief This SpawnPointPreRunCommon populates the current World with Common Agents.
*
* \details This SpawnPointPreRunCommon generates random Common Agents and places them in the World based on the specified spawning area of the configuration.
* This SpawnPointPreRunCommon also checks that no cars are spawned in between Ego/Scenario agents.
*
* @} */

#pragma once

#include "agentBlueprint.h"
#include "common/WorldAnalyzer.h"
#include "common/spawnPointLibraryDefinitions.h"
#include "include/spawnPointInterface.h"
#include "include/worldInterface.h"
#include "include/scenarioInterface.h"
#include "include/agentBlueprintProviderInterface.h"
#include "SpawnPointPreRunCommonDefinitions.h"

using namespace SpawnPointPreRunCommonDefinitions;

/**
* \brief spawns agents for all scenarios
* \details This class implements the entire SpawnPointPreRunCommon logic. The spawnpoint calls the agentsampler to fill out all probability related
* parameters of the blueprint. The spawnpoint then adds the spawning depended parameters to the
* agentblueprint. The agentblueprint is then forwarded to the AgentFactory for instantiation.
* \ingroup SpawnPointPreRunCommon
*/
class SpawnPointPreRunCommon : public SpawnPointInterface
{
private:
    static constexpr int NUMBER_OF_TRIALS_STOCHASTIC = 5;
    static constexpr std::string_view COMPONENTNAME = "SpawnPointCommonPreRun";

public:
    SpawnPointPreRunCommon(const SpawnPointDependencies* dependencies,
                           const CallbackInterface* callbacks);
    SpawnPointPreRunCommon(const SpawnPointPreRunCommon&) = delete;
    SpawnPointPreRunCommon(SpawnPointPreRunCommon&&) = delete;
    SpawnPointPreRunCommon& operator=(const SpawnPointPreRunCommon&) = delete;
    SpawnPointPreRunCommon& operator=(SpawnPointPreRunCommon&&) = delete;
    virtual ~SpawnPointPreRunCommon() override = default;

    /*!
     * \brief Trigger creates the agents for the spawn points
     * \return the created agents
     */
    Agents Trigger(int time) override;

private:
    /**
     * @brief Generates agents in specified lane based on specified range.
     *
     * @param[in]   roadId      Id of the road to spawn agents in.
     * @param[in]   validLaneSpawningRange       LaneSpawningRange containing laneId and Range.
     * @return
     */
    Agents GenerateAgentsForRange(const RoadId& roadId, const LaneSpawningRange& validLaneSpawningRange);

    /**
     * @brief Get SpawnInfo for the next agent that should be spawned.
     * @param[in]   roadId              Id of the road to spawn agents in.
     * @param[in]   laneId              Id of the lane in which new agent should be spawned.
     * @param[in]   range               Range of the lane in which new agent should be spawned.
     * @param[in]   gapInSeconds        Gap between new agent and other agents in the lane.
     * @param[in]   velocity            Velocity of the new agent.
     * @param[in]   agentFrontLength
     * @param[in]   agentRearLength
     * @param[in]   route
     * @return      SpawnInfo for new agent.
     */
    std::optional<SpawnInfo> GetNextSpawnCarInfo(const RoadId& roadId,
                                                 const LaneId& laneId,
                                                 const Range& range,
                                                 const double gapInSeconds,
                                                 const double velocity,
                                                 const double agentFrontLength,
                                                 const double agentRearLength,
                                                 const Route& route) const;
    /**
     * @brief Checks validity of spawning coordinates.
     * @param[in]   roadId                  Id of the road in which agent is to be spawned.
     * @param[in]   laneId                  Id of the lane in which agent is to be spawned.
     * @param[in]   sPosition               Position on the road in which agent is to be spawned.
     * @param[in]   offset
     * @param[in]   vehicleModelParameters  Contains information about the vehicle model.
     * @return      true if valid. false if not valid.
     */
    bool AreSpawningCoordinatesValid(const std::string& roadId,
                                     const int laneId,
                                     const double sPosition,
                                     const double offset,
                                     const VehicleModelParameters& vehicleModelParameters);
    /**
     * @brief Checks if offset for lane is valid.
     * @param[in]   roadId              Id of the road in which agent is to be spawned.
     * @param[in]   laneId              Id of the lane in which agent is to be spawned.
     * @param[in]   distanceFromStart   Distance from the start of the road.
     * @param[in]   offset
     * @param[in]   vehicleWidth        Width of the vehicle.
     * @return      true if valid. false if not valid.
     */
    bool IsOffsetValidForLane(const std::string& roadId,
                              const int laneId,
                              const double distanceFromStart,
                              const double offset,
                              const double vehicleWidth);
    /**
     * @brief NewAgentIntersectsWithExistingAgent
     * @param[in]   laneId                  Id of the lane in which agent is to be spawned.
     * @param[in]   sPosition               Position along the road where agent is to be spawned.
     * @param[in]   offset
     * @param[in]   vehicleModelParameters  Contains information about the vehicle model.
     * @return      returns true if new agent intersects with existing agent. returns false if not.
     */
    bool NewAgentIntersectsWithExistingAgent(const int laneId,
                                             const double sPosition,
                                             const double offset,
                                             const VehicleModelParameters& vehicleModelParameters) const;
    /**
     * @brief CalculateSpawnParameter
     * @param[in]   agentBlueprint      AgentBlueprint for new agent.
     * @param[in]   laneId              Id of lane in which new agent should be spawned.
     * @param[in]   spawnInfo           SpawnInfo of new agent.
     * @param[in]   route               Route
     * @return      return true if successful
     */
    bool CalculateSpawnParameter(AgentBlueprintInterface* agentBlueprint,
                                 const SpawnInfo& spawnInfo,
                                 const Route& route);

    /**
     * \brief Logs a message for error mode.
     *
     * \details Logs a message for error mode.
     *
     * @param[in]   message     Logged message.
     */
    [[noreturn]] void LogError(const std::string& message);

    SpawningAgentProfile SampleAgentProfile(bool rightLane);


    const SpawnPointDependencies dependencies;
    const PreRunSpawnerParameters parameters;
    const WorldAnalyzer worldAnalyzer;

    const LaneTypes supportedLaneTypes =
    {
        LaneType::Driving,
        LaneType::OnRamp,
        LaneType::OffRamp,
        LaneType::ConnectingRamp
    };

    static constexpr double NON_PLATOON_GAP_EXTENSION = 10.0;
};


