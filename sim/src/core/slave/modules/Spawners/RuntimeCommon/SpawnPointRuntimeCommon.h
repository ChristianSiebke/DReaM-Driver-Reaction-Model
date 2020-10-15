/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/**
 * \addtogroup SpawnPoint
 * @{
 * \brief The SpawnPointRuntimeCommon keeps the World populated with Common
 *        Agents by spawning new Agents according to the provided Traffic
 *        Parameters.
 *
 * \details The SpawnPointRuntimeCommon continuously checks a specified road
 *          location for a valid spawn opportunity and spawns Common Agents
 *          when such an opportunity is found.
 *          This keeps traffic ever-flowing within the world.
 * @}
 */

#pragma once

#include "SpawnPointRuntimeCommonDefinitions.h"

#include "agentBlueprint.h"
#include "include/spawnPointInterface.h"
#include "common/SpawnPointDefinitions.h"
#include "common/spawnPointLibraryDefinitions.h"
#include "common/WorldAnalyzer.h"

using namespace SpawnPointRuntimeCommonDefinitions;

class SpawnPointRuntimeCommon : public SpawnPointInterface
{
private:
    static constexpr std::string_view COMPONENTNAME = "SpawnPointRuntimeCommon";
    static constexpr double NON_PLATOON_GAP_EXTENSION = 10.0;
public:
    SpawnPointRuntimeCommon(const SpawnPointDependencies * dependencies,
                            const CallbackInterface * const callbacks);
    SpawnPointRuntimeCommon(const SpawnPointRuntimeCommon&) = delete;
    SpawnPointRuntimeCommon(SpawnPointRuntimeCommon&&) = delete;
    SpawnPointRuntimeCommon& operator=(const SpawnPointRuntimeCommon&) = delete;
    SpawnPointRuntimeCommon& operator=(SpawnPointRuntimeCommon&&) = delete;
    ~SpawnPointRuntimeCommon() override = default;

    /*!
     * \brief Trigger creates the agents for the spawn points
     * \return the created agents
     */
    Agents Trigger(int time) override;

private:

    /*!
     * \brief GenerateSpawnDetailsForLane Generates new spawn details for the
     *        specified lane
     * \param laneId the laneId for which to get the spawn details
     * \return the spawn details for the lane
     */
    SpawnDetails GenerateSpawnDetailsForLane(const SpawnPosition sceneryInformation, int time);

    /*!
     * \brief LogError logs an error and throws
     * \param message the message for the error to log
     */
    [[noreturn]] void LogError(const std::string& message);

    void AdjustVelocityForCrash(SpawnDetails& spawnDetails, const SpawnPosition& sceneryInformation) const;

    bool AreSpawningCoordinatesValid(const SpawnDetails& spawnDetails, const SpawnPosition& sceneryInformation) const;
    /**
     * @brief CalculateSpawnParameter
     * @param[in]   agentBlueprint      AgentBlueprint for new agent.
     * @param[in]   laneId              Id of lane in which new agent should be spawned.
     * @param[in]   spawnInfo           SpawnInfo of new agent.
     */
    void CalculateSpawnParameter(AgentBlueprintInterface* agentBlueprint,
                                 const RoadId& roadId,
                                 const LaneId laneId,
                                 const SPosition sPosition,
                                 const double velocity) const;

    SpawningAgentProfile SampleAgentProfile(bool rightLane);

    const SpawnPointDependencies dependencies;
    const WorldAnalyzer worldAnalyzer;
    const SpawnPointRuntimeCommonParameters parameters;

    std::vector<SpawnDetails> queuedSpawnDetails;
};
