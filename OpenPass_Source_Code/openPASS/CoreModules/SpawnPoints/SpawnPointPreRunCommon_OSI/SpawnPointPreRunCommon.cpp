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
#include "CoreFramework/OpenPassSlave/framework/sampler.h"

SpawnPointPreRunCommon::SpawnPointPreRunCommon(const SpawnPointDependencies* dependencies,
                       const CallbackInterface* callbacks):
    SpawnPointInterface(dependencies->world, callbacks),
    dependencies(*dependencies),
    parameters(ExtractSpawnPointParameters(*(dependencies->parameters.value()))),
    worldAnalyzer(dependencies->world)
{}

SpawnPointInterface::Agents SpawnPointPreRunCommon::Trigger([[maybe_unused]]int time)
{
    SpawnPointInterface::Agents newAgents;

    for (const auto &spawnArea : parameters.spawnAreas)
    {
        const Route routeForRoadId{spawnArea.roadId};

        size_t laneIndex = 0;
        for (const auto laneId : spawnArea.laneIds)
        {

            const auto validLaneSpawningRanges = worldAnalyzer.GetValidLaneSpawningRanges(routeForRoadId,
                                                                                          spawnArea.roadId,
                                                                                          laneId,
                                                                                          spawnArea.sStart,
                                                                                          spawnArea.sEnd);
            if (validLaneSpawningRanges)
            {
                for (const auto& spawningRange : *validLaneSpawningRanges)
                {
                    const auto generatedAgents = GenerateAgentsForRange(laneId,
                                                                        spawnArea.roadId,
                                                                        spawningRange,
                                                                        laneIndex);
                    newAgents.insert(std::cend(newAgents),
                                     std::cbegin(generatedAgents),
                                     std::cend(generatedAgents));
                }
            }

            laneIndex++;
        }
    }

    return newAgents;
}

SpawningAgentProfile SpawnPointPreRunCommon::SampleAgentProfile(bool rightLane)
{
    return Sampler::Sample(rightLane ? parameters.agentProfileLaneMaps.rightLanes : parameters.agentProfileLaneMaps.leftLanes, dependencies.stochastics);
}

SpawnPointInterface::Agents SpawnPointPreRunCommon::GenerateAgentsForRange(const LaneId laneId,
                                                                           const RoadId roadId,
                                                                           const Range& range,
                                                                           size_t laneIndex)
{
    const auto routeForRoadId = Route{roadId};
    SpawnPointInterface::Agents agents;
    bool generating = true;

    while (generating)
    {
        const auto agentProfile = SampleAgentProfile(laneIndex == 0);

        try
        {
            auto agentBlueprint = dependencies.agentBlueprintProvider->SampleAgent(agentProfile.name, {});
            agentBlueprint.SetAgentProfileName(agentProfile.name);
            agentBlueprint.SetAgentCategory(AgentCategory::Common);

            const auto agentLength = agentBlueprint.GetVehicleModelParameters().length;
            const auto agentFrontLength = agentBlueprint.GetVehicleModelParameters().distanceReferencePointToLeadingEdge;
            const auto agentRearLength = agentLength - agentFrontLength;

            auto velocity = Sampler::RollForStochasticAttribute(agentProfile.velocity, dependencies.stochastics);

            for (size_t iterator = 0; iterator < laneIndex; ++iterator)
            {
                double homogeneity = agentProfile.homogeneities.size() > iterator ? agentProfile.homogeneities[iterator] : agentProfile.homogeneities.back();
                velocity *= 2 - homogeneity;
            }

            const auto tGap = Sampler::RollForStochasticAttribute(agentProfile.tGap, dependencies.stochastics);

            const auto spawnInfo = GetNextSpawnCarInfo(roadId, laneId, range, tGap, velocity, agentFrontLength, agentRearLength);
            if (!spawnInfo)
            {
                generating = false;
                break;
            }
            const auto sPosition = std::get<openScenario::LanePosition>(spawnInfo->position).s;
            if (!worldAnalyzer.AreSpawningCoordinatesValid(roadId,
                                                           laneId,
                                                           sPosition,
                                                           0 /* offset */,
                                                           agentBlueprint.GetVehicleModelParameters())
                || worldAnalyzer.SpawnWillCauseCrash(routeForRoadId,
                                                     roadId,
                                                     laneId,
                                                     sPosition,
                                                     agentFrontLength,
                                                     agentRearLength,
                                                     velocity,
                                                     Direction::BACKWARD))
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

std::optional<SpawnInfo> SpawnPointPreRunCommon::GetNextSpawnCarInfo(const RoadId roadId,
                                                                     const LaneId laneId,
                                                         const Range& range,
                                                         const double gapInSeconds,
                                                         const double velocity,
                                                         const double agentFrontLength,
                                                         const double agentRearLength) const
{
    std::optional<SpawnInfo> optionalSpawnInfo;

    const auto routeForRoadId = Route{roadId};
    const auto spawnDistance = worldAnalyzer.GetNextSpawnPosition(routeForRoadId,
                                                                  roadId,
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
                                                                                        roadId,
                                                                                        laneId,
                                                                                        *spawnDistance,
                                                                                        agentFrontLength,
                                                                                        agentRearLength,
                                                                                        velocity);

    openScenario::LanePosition lanePosition;
    lanePosition.roadId = roadId;
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


    Position pos = GetWorld()->LaneCoord2WorldCoord(lanePosition.s,
                                                    lanePosition.offset.value_or(0.0),
                                                    lanePosition.roadId,
                                                    lanePosition.laneId);

    double spawnV = spawnInfo.velocity;

    //considers adjusted velocity in curvatures
    double kappa = pos.curvature;

    // Note: This could falsify ego and scenario agents
    if (kappa != 0.0)
    {
        double curvatureVelocity;

        curvatureVelocity = 160 * (1 / (std::sqrt((std::abs(kappa)) / 1000)));

        spawnV = std::min(spawnV, curvatureVelocity);
    }

    SpawnParameter& spawnParameter = agentBlueprint->GetSpawnParameter();
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
