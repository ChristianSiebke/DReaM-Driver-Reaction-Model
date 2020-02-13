/*******************************************************************************
* Copyright (c) 2017, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "SpawnPointPreRunCommon.h"

#include "Interfaces/agentInterface.h"
#include "Interfaces/worldInterface.h"
#include "SpawnPointPreRunCommonParameterExtractor.h"
#include "CoreFramework/OpenPassSlave/framework/agentFactory.h"

// TODO JMT may we assume that all specified lanes are driving lanes or should we check each lane before spawning?

SpawnPointPreRunCommon::SpawnPointPreRunCommon(const SpawnPointDependencies* dependencies,
                       const CallbackInterface* callbacks):
    SpawnPointInterface(dependencies->world, callbacks),
    dependencies(*dependencies),
    parameters(SpawnPointParameterExtractor::ExtractSpawnPointParameters(*(dependencies->parameters.value()), dependencies->sampler)),
    worldAnalyzer(dependencies->world)
{}

SpawnPointInterface::Agents SpawnPointPreRunCommon::Trigger()
{
    if(parameters.carsPerSecond <= 0.0)
    {
        std::stringstream log;
        log.str(std::string());
        log << COMPONENTNAME << ": Unable to spawn agents (carsPerSecond 0 or negative: " << parameters.carsPerSecond << ")";
        LOG(CbkLogLevel::Warning, log.str());
        return {};
    }

    SpawnPointInterface::Agents newAgents;

    const Route routeForRoadId{parameters.roadId};
    for (const auto laneId : parameters.laneIds)
    {
        const auto validLaneSpawningRanges = worldAnalyzer.GetValidLaneSpawningRanges(routeForRoadId,
                                                                                       parameters.roadId,
                                                                                       laneId,
                                                                                       parameters.sStart,
                                                                                       parameters.sEnd);
        if (validLaneSpawningRanges)
        {
            for (const auto& spawningRange : *validLaneSpawningRanges)
            {
                const auto generatedAgents = GenerateAgentsForRange(laneId,
                                                                    spawningRange);
                newAgents.insert(std::cend(newAgents),
                                 std::cbegin(generatedAgents),
                                 std::cend(generatedAgents));
            }
        }
    }

    return newAgents;
}

SpawnPointInterface::Agents SpawnPointPreRunCommon::GenerateAgentsForRange(const LaneId laneId,
                                                               const Range& range)
{
    SpawnPointInterface::Agents agents;
    bool generating = true;

    while (generating)
    {
        const auto agentProfileName = dependencies.sampler->SampleStringProbability(parameters.agentProfiles);

        try
        {
            auto agentBlueprint = dependencies.agentBlueprintProvider->SampleAgent(agentProfileName);

            agentBlueprint.SetAgentProfileName(agentProfileName);
            agentBlueprint.SetAgentCategory(AgentCategory::Common);

            const auto agentLength = agentBlueprint.GetVehicleModelParameters().length;
            const auto agentFrontLength = agentBlueprint.GetVehicleModelParameters().distanceReferencePointToLeadingEdge;
            const auto agentRearLength = agentLength - agentFrontLength;
            const auto gapInSeconds = RollGapBetweenCars();

            const auto velocity = dependencies.sampler->RollForStochasticAttribute(parameters.trafficVelocityDistribution.mean,
                                                                                   parameters.trafficVelocityDistribution.standardDeviation,
                                                                                   parameters.trafficVelocityDistribution.min,
                                                                                   parameters.trafficVelocityDistribution.max);

            const auto spawnInfo = GetNextSpawnCarInfo(laneId, range, gapInSeconds, velocity, agentFrontLength, agentRearLength);
            if (!spawnInfo)
            {
                generating = false;
                break;
            }
            if (!worldAnalyzer.AreSpawningCoordinatesValid(parameters.roadId,
                                                           laneId,
                                                           std::get<openScenario::LanePosition>(spawnInfo->position).s,
                                                           0 /* offset */,
                                                           agentBlueprint.GetVehicleModelParameters()))
            {
                generating = false;
                break;
            }

            if (!CalculateSpawnParameter(&agentBlueprint, *spawnInfo))
            {
                generating = false;
                break;
            }

            SimulationSlave::Agent* newAgent = dependencies.agentFactory->AddAgent(&agentBlueprint);

            if(newAgent != nullptr)
            {
                agents.emplace_back(newAgent);
            }
            else
            {
                generating = false;
                break;
            }
        }
        catch (const std::runtime_error& error)
        {
            LogError(error.what());
        }
    }
    return agents;
}

double SpawnPointPreRunCommon::RollGapBetweenCars() const
{
    double gapInSeconds = dependencies.sampler->RollGapBetweenCars(parameters.carsPerSecond);

    if (!dependencies.sampler->RollFor(parameters.platoonRate))
    {
        gapInSeconds += dependencies.sampler->RollGapExtension(NON_PLATOON_GAP_EXTENSION);
    }

    return gapInSeconds;
}

std::optional<SpawnInfo> SpawnPointPreRunCommon::GetNextSpawnCarInfo(const LaneId laneId,
                                                         const Range& range,
                                                         const double gapInSeconds,
                                                         const double velocity,
                                                         const double agentFrontLength,
                                                         const double agentRearLength) const
{
    std::optional<SpawnInfo> optionalSpawnInfo;

    const auto routeForRoadId = Route{parameters.roadId};
    const auto spawnDistance = worldAnalyzer.GetNextSpawnPosition(routeForRoadId,
                                                                  parameters.roadId,
                                                                  laneId,
                                                                  range,
                                                                  agentFrontLength,
                                                                  agentRearLength,
                                                                  velocity,
                                                                  gapInSeconds,
                                                                  Direction::FORWARD);

    if (!spawnDistance)
    {
        return optionalSpawnInfo;
    }

    const auto adjustedVelocity = worldAnalyzer.CalculateSpawnVelocityToPreventCrashing(routeForRoadId,
                                                                                        parameters.roadId,
                                                                                        laneId,
                                                                                        *spawnDistance,
                                                                                        agentFrontLength,
                                                                                        agentRearLength,
                                                                                        velocity);

    openScenario::LanePosition lanePosition;
    lanePosition.roadId = parameters.roadId;
    lanePosition.laneId = laneId;
    lanePosition.offset = 0.0;
    lanePosition.s = spawnDistance.value();

    SpawnInfo spawnInfo;
    spawnInfo.position = lanePosition;
    spawnInfo.velocity = adjustedVelocity;
    spawnInfo.acceleration = 0.0;

    optionalSpawnInfo = spawnInfo;

    return optionalSpawnInfo;
}



bool SpawnPointPreRunCommon::CalculateSpawnParameter(AgentBlueprintInterface* agentBlueprint,
                                                     const SpawnInfo& spawnInfo)
{
    const auto& lanePosition = std::get<openScenario::LanePosition>(spawnInfo.position);

    SpawnParameter& spawnParameter = agentBlueprint->GetSpawnParameter();

    Position pos = GetWorld()->LaneCoord2WorldCoord(lanePosition.s,
                                                    lanePosition.offset.value_or(0.0),
                                                    lanePosition.roadId,
                                                    lanePosition.laneId);

    double spawnV = spawnInfo.velocity;

    if(agentBlueprint->GetVehicleModelParameters().vehicleType == AgentVehicleType::Truck)
    {
        spawnV = std::min(90 / 3.6, spawnV);
    }

    //considers adjusted velocity in curvatures
    double kappa = pos.curvature;

    // Note: This could falsify ego and scenario agents
    if (kappa != 0.0)
    {
        double curvatureVelocity;

        curvatureVelocity = 160 * (1 / (std::sqrt((std::abs(kappa)) / 1000)));

        spawnV = std::min(spawnV, curvatureVelocity);
    }

    spawnParameter.positionX = pos.xPos;
    spawnParameter.positionY = pos.yPos;
    spawnParameter.yawAngle  = pos.yawAngle;
    spawnParameter.velocity = spawnV;
    spawnParameter.acceleration = spawnInfo.acceleration.value_or(0.0);

    return true;
}

void SpawnPointPreRunCommon::LogError(const std::string& message)
{
    std::stringstream log;
    log.str(std::string());
    log << COMPONENTNAME << " " << message;
    LOG(CbkLogLevel::Error, log.str());
    throw std::runtime_error(log.str());
}
