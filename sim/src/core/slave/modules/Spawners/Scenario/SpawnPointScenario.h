/*******************************************************************************
* Copyright (c) 2017, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "common/SpawnPointDefinitions.h"
#include "common/spawnPointLibraryDefinitions.h"
#include "include/spawnPointInterface.h"
#include "include/worldInterface.h"
#include "include/scenarioInterface.h"
#include "include/agentBlueprintProviderInterface.h"

struct STCoordinates
{
    double s;
    double t;
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
    static constexpr std::string_view COMPONENTNAME = "SpawnPointScenario";

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
    Agents Trigger(int time) override;

private:
    /*!
    * \brief Validates if there are already existing agents at a LanePosition
    *
    * @param[in]        stCoordinates           stCoordinates of position being validates
    * @param[in]        lanePosition            lanePosition containing road and lane id
    * @param[in]        vehicleModelParameters  vehicleModelParameters of the new agent
    * @return           true, if new LanePosition does not overlap with existing agents
    */
    bool ValidateOverlappingAgents(const STCoordinates& stCoordinates,
                                   const openScenario::LanePosition& lanePosition,
                                   const VehicleModelParameters& vehicleModelParameters);

    /*!
    * \brief Checks if the offset is valid for a specific lane.
    *
    * \details  Checks if the vehicle is more than 50% inside the lane and is not placed on an invalid lane
    *
    * @param[in]        stCoordinates   stCoordinates of position being validates
    * @param[in]        lanePosition    lanePosition containing road and lane id
    * @param[in]        vehicleWidth    width of the new agent
    * @return        true if valid. false if not valid.
    */
    bool ValidateSTCoordinatesOnLane(const STCoordinates& stCoordinate,
                                     const openScenario::LanePosition& lanePosition,
                                     const double vehicleWidth);

    /*!
     * \brief This method defines the S and T coordinate for a new agent, which is spawned based on a LanePosition
     *
     * \details If the LanePosition is based on stochastics the SpawnPoint will try to place up to 5 different samples.
     *  If no valid S and T coordinates can be found, an error is thrown.
     *
     * \param[in]       lanePosition            Id of the referenced openDrive road
     * \param[in]       vehicleModelParameters  Parameter of the vehicle
     * \return       valid S and T coordinates of a new agent
     */
    STCoordinates GetSTCoordinates(const openScenario::LanePosition& lanePosition,
                                   const VehicleModelParameters& vehicleModelParameters);

    /*!
    * \brief Creates new SpawnParameter for the AgentBlueprint.
    *
    * \param[in]     spawnInfo              Containing spawn information of the scenario file
     * \param[in]    vehicleModelParameters Parameter of the vehicle
    * \return       Valid SpawnParameter
    */
    SpawnParameter CalculateSpawnParameter(const SpawnInfo& spawnInfo,
                                           const VehicleModelParameters& vehicleModelParameters);

    /*!
     * \brief Samples a value based on stochastics
     * \param[in]   attribute
     * \return value
     */
    double CalculateAttributeValue(const openScenario::StochasticAttribute &attribute);

    /*!
     * \brief Converts the imported route from OpenScenario into a graph
     *
     * \param[in]   roads   route from OpenScenario
     *
     * \return  graph of road network from agent's perspective with target node as given by OpenScenario, nullopt if no road in OpenScenario defined
     */
    Route GetPredefinedRoute(const std::vector<RouteElement>& roads);

    /*!
     * \brief Samples a random route for the agent starting at the given position
     *
     * \param spawnParameter parameters containing spawn position
     *
     * \return sampled route
     */
    Route GetRandomRoute(const SpawnParameter& spawnParameter);

    /*!
    * \brief Logs a error message.
    *
    * \details Logs a error message.
    *
    * @param[in]     message    Logged message.
    */
    [[noreturn]] void LogError(const std::string& message);

    const SpawnPointDependencies dependencies;
};
