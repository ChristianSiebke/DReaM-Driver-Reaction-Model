/*******************************************************************************
* Copyright (c) 2017, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Common/SpawnPointDefinitions.h"
#include "Interfaces/spawnPointInterface.h"
#include "Interfaces/worldInterface.h"
#include "Interfaces/samplerInterface.h"
#include "Interfaces/scenarioInterface.h"
#include "Interfaces/agentBlueprintProviderInterface.h"

struct LaneParameters
{
    int laneId{0};
    double distanceToLaneStart{0};
    double offset{0};
};
/**
* \brief spawns agents for all scenarios
* \details This class implements a SpawnPointInterface which is used by the framework to fill out
* AgentBlueprints. The spawnpoint calls the agentsampler to fill out all probability related
* parameters of the blueprint. The spawnpoint then adds the spawning depended parameters to the
* agentblueprint. The agentblueprint is then returned to the framework.
* \ingroup SpawnPointScenario
*/
class SpawnPointScenario : public SpawnPointInterface
{
private:
    static constexpr int NUMBER_OF_TRIALS_STOCHASTIC = 5;
    static constexpr std::string_view COMPONENTNAME = "SpawnPointScenario_OSI";

public:
    SpawnPointScenario(const SpawnPointDependencies* dependencies,
               const CallbackInterface* callbacks);
    SpawnPointScenario(const SpawnPointScenario&) = delete;
    SpawnPointScenario(SpawnPointScenario&&) = delete;
    SpawnPointScenario& operator=(const SpawnPointScenario&) = delete;
    SpawnPointScenario& operator=(SpawnPointScenario&&) = delete;
    ~SpawnPointScenario() override = default;

    /*!
     * \brief Trigger creates the agents for the spawn points
     * \return the created agents
     */
    Agents Trigger() override;

    /*!
    * \brief Calculates the SpawnInfo of an agent.
    *
    * \details Calculates the SpawnInfo of an agent. Depending on the SpawnPoint state different methods are used.
    *
    *
    * @param[in]     laneId            The LaneId on which the agent will be spawned
    * @param[in]     agentBlueprint    AgentBlueprint contains all parameters of an agent.
    * @param[in]     entity            ScenarioEntity contains spawn info for the scenario agent.
    * @return        true if successful. false if not successful.
    */
    SpawnInfo GetNextSpawnCarInfo(const int laneId,
                                  AgentBlueprintInterface& agentBlueprint,
                                  const ScenarioEntity &entity);

    /*!
    * \brief Sets the predefined SpawnParameters in the AgentBlueprint.
    *
    * @param[in]     agentBlueprint             AgentBlueprint containing all information of an agent which is about to be spawned.
    * @param[in]     scenarioEntity             ScenarioEntity contains spawn info for the scenario agent which is about to be spawned.
    */
    void SetPredefinedParameter(AgentBlueprintInterface& agentBluePrint, const ScenarioEntity& scenarioEntity) const;

    /*!
    * \brief Updates the SpawnParameter of an AgentBlueprint.
    *
    * \details Triggers functions to update the SpawnInfo.
    * Then updates the SpawnParameter of an AgentBlueprint.
    *
    * @param[in|out]     agentBlueprint         AgentBlueprint which is being updated.
    * @param[in]         spawnInfo              SpawnInfo for the agent.
    * @return            true if successful. false if not.
    */
    bool CalculateSpawnParameter(AgentBlueprintInterface& agentBlueprint,
                                 const SpawnInfo& spawnInfo);

    /*!
    * \brief Checks if the offset is valid for a specific lane. Only for unit testin !
    *
    * \details  Checks if the vehicle is more than 50% inside the lane and is not placed on an invalid lane
    *
    * @param[in]     roadId   id of the referenced openDrive road
    * @param[in]     laneId   id of the lane
    * @param[in]     distanceFromStart    s-coordinate
    * @param[in]     offset    vehicle offset inside the lane
    * @param[in]     vehicleWidth    width  of the vehicle
    * @return        true if valid. false if not valid.
    */
    bool IsOffsetValidForLane(std::string roadId, int laneId, double distanceFromStart, double offset, double vehicleWidth);





private:
    /*!
    * \brief Calculates the SpawnInfo for a scenario agent.
    *
    * \details Calculates the SpawnInfo for the scenario agent if possible.
    *
    *
    * @param[in]     laneId             Id of the lane where the scenario agent is supposed to be.
    * @param[in]     egoVelocity        Velocity of the scenario agent.
    * @param[in]     rearCarLength      Length of the scenario agent from the reference point to the rear.
    * @return        SpawnInfo of the scenario agent.
    */
    SpawnInfo GenerateSpawnInfo(SpawnInfo spawnInfo,
                                     int laneId,
                                     double egoVelocity,
                                     double rearCarLength);

    /*!
    * \brief Checks whether a new agent intersects with an existing agent
    *
    * @param[in]        spawnInfo information of new Agent's intended spawning parameters
    * @param[in]        agentBlueprint the intended blueprint with which to spawn the new Agent
    * @return           true if if new agent intersects with existing agent
    */
    bool NewAgentIntersectsWithExistingAgent(const SpawnInfo& spawnInfo,
                                             const AgentBlueprintInterface& agentBlueprint);

    /*!
     * \brief TryGetSpawnDistance
     *
     * @param[in]   roadId        Id of the referenced openDrive road
     * \param[in]   laneId        Lane ID of interest (e.g. -1)
     * \param[in]   sAttribute    potentially stochastic attribute for the spawn distance
     * \return              true if (rolled) spawn distance is on lane of interest
     */
    bool TryGetSpawnDistance(std::string roadId, int laneId, SpawnAttribute sAttribute);

    /*!
     * \brief CalculateAttributeValue
     * \param[in]   attribute
     */
    void CalculateAttributeValue(SpawnAttribute& attribute);

    /*!
    * \brief Respawns an agent.
    *
    * \details Respawns an agent. This is only activated if the manipulator for this event exists.
    *
    *
    * @param[in]     LaneParameters                         LaneParameters of the lane where the common agent is supposed to be.
    * @param[in]     gapInSeconds                   Gap between agents in seconds.
    * @param[in]     egoVelocity                    Velocity of the common agent.
    * @param[in]     frontCarRearLength Length of the agent in front from the reference point to the front.
    * @param[in]     distanceReferencePointToLeadingEdge    Length of the common agent from the reference point to the front.
    * @return        SpawnInfo of the common agent.
    */
    SpawnInfo GetNextSpawnCarInfoRespawn(LaneParameters laneParameters,
                                         double gapInSeconds,
                                         double egoVelocity,
                                         double frontCarRearLength,
                                         double distanceReferencePointToLeadingEdge);

    /*!
    * \brief Helper of the SetPredefinedParameter method.
    *
    * \details Sets the predefined SpawnParameters in the AgentBlueprint.
    *
    *
    * @param[in]     agentBlueprint             AgentBlueprint containing all information of an agent which is about to be spawned.
    * @param[in]     agentTypeGroupName         Name of the agent type group the agent belongs to.
    * @param[in]     scenarioEntity             The entity information from the scenario file
    */
    void SetPredefinedParameterHelper(AgentBlueprintInterface& agentBlueprint,
                                      AgentCategory agentCategory,
                                      const ScenarioEntity& scenarioEntity);

    /*!
    * \brief Checks wether a distance is valid for spawning on a lane.
    *
    * \details Checks wether a distance is valid for spawning on a lane.
    * This method considers wether the lane exists at this distance.
    * Then it checks wether another agent is already at this position.
    * Then it tests wether the distance is to close to the end of the road.
    *
    * @param[in]     spawnInfo      The information pertaining to the intended spawn coordinates of the new Agent
    * @param[in]     agentBlueprint The AgentBlueprint intended to define the new Agent
    * @return        true if valid. false if not valid.
    */
    bool AreSpawningCoordinatesValid(const SpawnInfo& spawnInfo,
                                     const AgentBlueprintInterface& agentBlueprint);

    /*!
    * \brief Updates the LaneParameters to contain the next scenery Laneparameters.
    *
    * \details Looks up the next scenery LaneParameters specified in the scenario file.
    *
    *
    * @param[out]       laneParameters    LaneParameters of an scenery agent.
    * @return           true if successful. false if not.
    */
    LaneParameters GenerateLaneParameters(SpawnInfo spawnInfo);

    /*!
    * \brief Logs a message for warning mode.
    *
    * \details Logs a message for warning mode.
    *
    * @param[in]     message    Logged message.
    */
    void LogWarning(const std::string& message);

    /*!
    * \brief Logs a error message.
    *
    * \details Logs a error message.
    *
    * @param[in]     message    Logged message.
    */
    void LogError(const std::string& message);

    const SpawnPointDependencies dependencies;
};
