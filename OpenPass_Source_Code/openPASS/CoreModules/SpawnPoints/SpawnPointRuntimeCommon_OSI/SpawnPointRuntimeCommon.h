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
#include "Interfaces/spawnPointInterface.h"
#include "Common/SpawnPointDefinitions.h"
#include "Common/WorldAnalyzer.h"

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
    Agents Trigger() override;

private:
    /*!
     * \brief GetSpawnDetailsForLane Gets the spawn details for the specified
     *        lane; if there's spawn details queued for the lane, it uses
     *        those; otherwise, it generates new spawn details
     * \param laneId the laneId for which to get the spawn details
     * \return the spawn details for the lane, if successfully retrieved
     */
    std::optional<SpawnDetails> GetSpawnDetailsForLane(const LaneId laneId);

    /*!
     * \brief GenerateSpawnDetailsForLane Generates new spawn details for the
     *        specified lane
     * \param laneId the laneId for which to get the spawn details
     * \return the spawn details for the lane
     */
    SpawnDetails GenerateSpawnDetailsForLane(const LaneId laneId);

    /*!
     * \brief RollGapBetweenCars rolls for the requisite distance between the
     *        car to be spawned and the next car in the lane
     * \return the gap in seconds between this car and the next car in the lane
     */
    double RollGapBetweenCars() const;

    /*!
     * \brief LogError logs an error and throws
     * \param message the message for the error to log
     */
    [[noreturn]] void LogError(const std::string& message);

    /*!
     * \brief ShouldSpawnAgent determines if the spawn details are valid to be
     *        spawned on the lane
     * \param spawnDetails the spawn details to validate
     * \param laneId the lane for which to validate the spawn details
     * \return true if the agent should spawn; false otherwise
     */
    bool ShouldSpawnAgent(const SpawnDetails& spawnDetails,
                          const LaneId laneId) const;
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

    const SpawnPointDependencies dependencies;
    const SpawnPointRuntimeCommonParameters parameters;
    std::unordered_map<LaneId, SpawnDetails> queuedSpawnDetails;
    const WorldAnalyzer worldAnalyzer;
};
