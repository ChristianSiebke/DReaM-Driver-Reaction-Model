/********************************************************************************
 * Copyright (c) 2019-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

/** \addtogroup SpawnPoint
* @{
* \brief This SpawnerPreRunCommon populates the current World with Common Agents.
*
* \details This SpawnerPreRunCommon generates random Common Agents and places them in the World based on the specified spawning area of the configuration.
* This SpawnerPreRunCommon also checks that no cars are spawned in between Ego/Scenario agents.
*
* @} */

#pragma once

#include "SpawnerPreRunCommonDefinitions.h"
#include "agentBlueprint.h"
#include "common/WorldAnalyzer.h"
#include "common/spawnPointLibraryDefinitions.h"
#include "include/agentBlueprintProviderInterface.h"
#include "include/spawnPointInterface.h"
#include "include/worldInterface.h"

using namespace SpawnerPreRunCommonDefinitions;

/**
* \brief spawns agents for all scenarios
* \details This class implements the entire SpawnerPreRunCommon logic. The spawnpoint calls the agentsampler to fill out all probability related
* parameters of the blueprint. The spawnpoint then adds the spawning depended parameters to the
* agentblueprint. The agentblueprint is then forwarded to the AgentFactory for instantiation.
* \ingroup SpawnerPreRunCommon
*/
class SpawnerPreRunCommon : public SpawnPointInterface
{
private:
    static constexpr int NUMBER_OF_TRIALS_STOCHASTIC = 5;
    static constexpr std::string_view COMPONENTNAME = "SpawnerCommonPreRun";

public:
    SpawnerPreRunCommon(const SpawnPointDependencies* dependencies,
                           const CallbackInterface* callbacks);
    SpawnerPreRunCommon(const SpawnerPreRunCommon &) = delete;
    SpawnerPreRunCommon(SpawnerPreRunCommon &&) = delete;
    SpawnerPreRunCommon & operator=(const SpawnerPreRunCommon &) = delete;
    SpawnerPreRunCommon & operator=(SpawnerPreRunCommon &&) = delete;
    virtual ~SpawnerPreRunCommon() override = default;

    /*!
     * \brief Trigger creates the agents for the spawn points
     * \return the created agents
     */
    Agents Trigger(int time) override;

private:
    /**
     * @brief Generates agents in specified lane based on specified range.
     *
    /* @param[in]  laneStream               LaneStream to spawn in
     * @param[in]  validLaneSpawningRange   range to spawn
     * @return generated agent
     */
    Agents GenerateAgentsForRange(const std::unique_ptr<LaneStreamInterface>& laneStream,
                                  const Range& range);

    /**
     * @brief Get SpawnInfo for the next agent that should be spawned.
    /* @param[in]   laneStream          LaneStream to spawn in
     * @param[in]   range               Range of the lane in which new agent should be spawned.
     * @param[in]   gapInSeconds        Gap between new agent and other agents in the lane.
     * @param[in]   velocity            Velocity of the new agent.
     * @param[in]   agentFrontLength
     * @param[in]   agentRearLength
     * @return      SpawnInfo for new agent.
     */
    std::optional<SpawnInfo> GetNextSpawnCarInfo(const std::unique_ptr<LaneStreamInterface> &laneStream,
                                                      const Range& range,
                                                      const double gapInSeconds,
                                                      const double velocity,
                                                      const double agentFrontLength,
                                                      const double agentRearLength) const;

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


