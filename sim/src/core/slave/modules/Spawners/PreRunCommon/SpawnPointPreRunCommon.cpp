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

#include "include/agentInterface.h"
#include "include/worldInterface.h"
#include "SpawnPointPreRunCommonParameterExtractor.h"
#include "framework/agentFactory.h"
#include "framework/sampler.h"

SpawnPointPreRunCommon::SpawnPointPreRunCommon(const SpawnPointDependencies* dependencies,
                       const CallbackInterface* callbacks):
    SpawnPointInterface(dependencies->world, callbacks),
    dependencies(*dependencies),
    parameters(ExtractSpawnPointParameters(*(dependencies->parameters.value()), callbacks)),
    worldAnalyzer(dependencies->world)
{}

SpawnPointInterface::Agents SpawnPointPreRunCommon::Trigger([[maybe_unused]]int time)
{
    SpawnPointInterface::Agents newAgents;

    for (const auto &spawnArea : parameters.spawnAreas)
    {
        for (const auto &roadId : spawnArea.roadIds)
        {
                const auto validLaneSpawningRanges = worldAnalyzer.GetValidLaneSpawningRanges(roadId,
                                                                                              spawnArea.sStart,
                                                                                              spawnArea.sEnd,
                                                                                              spawnArea.laneIds,
                                                                                              supportedLaneTypes);

                for (const auto& validSpawningRange : validLaneSpawningRanges)
                {
                    const auto generatedAgents = GenerateAgentsForRange(roadId,
                                                                        validSpawningRange);
                    newAgents.insert(std::cend(newAgents),
                                     std::cbegin(generatedAgents),
                                     std::cend(generatedAgents));
                }
        }
    }

    return newAgents;
}

SpawningAgentProfile SpawnPointPreRunCommon::SampleAgentProfile(bool rightLane)
{
    return Sampler::Sample(rightLane ? parameters.agentProfileLaneMaps.rightLanes : parameters.agentProfileLaneMaps.leftLanes, dependencies.stochastics);
}

SpawnPointInterface::Agents SpawnPointPreRunCommon::GenerateAgentsForRange(const RoadId& roadId,
                                                                           const LaneSpawningRange& validLaneSpawningRange)
{
    SpawnPointInterface::Agents agents;

    const auto& laneId = validLaneSpawningRange.laneId;
    const auto& range = validLaneSpawningRange.range;
    bool generating = true;

    size_t rightLaneCount = worldAnalyzer.GetRightLaneCount(roadId, laneId, range.second);

    while (generating)
    {
        const auto agentProfile = SampleAgentProfile(rightLaneCount == 0);

        try
        {
            auto agentBlueprint = dependencies.agentBlueprintProvider->SampleAgent(agentProfile.name, {});
            agentBlueprint.SetAgentProfileName(agentProfile.name);
            agentBlueprint.SetAgentCategory(AgentCategory::Common);

            const auto agentLength = agentBlueprint.GetVehicleModelParameters().length;
            const auto agentFrontLength = agentBlueprint.GetVehicleModelParameters().distanceReferencePointToLeadingEdge;
            const auto agentRearLength = agentLength - agentFrontLength;

            auto velocity = Sampler::RollForStochasticAttribute(agentProfile.velocity, dependencies.stochastics);

            for (size_t iterator = 0; iterator < rightLaneCount; ++iterator)
            {
                double homogeneity = agentProfile.homogeneities.size() > iterator ? agentProfile.homogeneities[iterator] : 1.0;
                velocity *= 2.0 - homogeneity;
            }

            const auto tGap = Sampler::RollForStochasticAttribute(agentProfile.tGap, dependencies.stochastics);

            const auto route = worldAnalyzer.SampleRoute(roadId,
                                                         laneId,
                                                         dependencies.stochastics);

            const auto spawnInfo = GetNextSpawnCarInfo(roadId, laneId, range, tGap, velocity, agentFrontLength, agentRearLength, route);
            if (!spawnInfo.has_value())
            {
                generating = false;
                break;
            }
            const auto sPosition = std::get<openScenario::LanePosition>(spawnInfo->position).s;
            if (!worldAnalyzer.AreSpawningCoordinatesValid(roadId,
                                                           laneId,
                                                           sPosition,
                                                           0 /* offset */,
                                                           route,
                                                           agentBlueprint.GetVehicleModelParameters())
                || worldAnalyzer.SpawnWillCauseCrash(roadId,
                                                     laneId,
                                                     sPosition,
                                                     agentFrontLength,
                                                     agentRearLength,
                                                     velocity,
                                                     Direction::BACKWARD,
                                                     route))
            {
                generating = false;
                break;
            }

            if (!CalculateSpawnParameter(&agentBlueprint, *spawnInfo, route))
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
            rightLaneCount = worldAnalyzer.GetRightLaneCount(roadId, laneId, sPosition);
        }
        catch (const std::runtime_error& error)
        {
            LogError(error.what());
        }
    }
    return agents;
}

std::optional<SpawnInfo> SpawnPointPreRunCommon::GetNextSpawnCarInfo(const RoadId& roadId,
                                                                     const LaneId& laneId,
                                                                     const Range& range,
                                                                     const double gapInSeconds,
                                                                     const double velocity,
                                                                     const double agentFrontLength,
                                                                     const double agentRearLength,
                                                                     const Route &route) const
{
    const auto spawnDistance = worldAnalyzer.GetNextSpawnPosition(roadId,
                                                                  laneId,
                                                                  range,
                                                                  agentFrontLength,
                                                                  agentRearLength,
                                                                  velocity,
                                                                  gapInSeconds,
                                                                  route,
                                                                  supportedLaneTypes);

    if (!spawnDistance.has_value())
    {
        return {};
    }

    const auto adjustedVelocity = worldAnalyzer.CalculateSpawnVelocityToPreventCrashing(roadId,
                                                                                        laneId,
                                                                                        *spawnDistance,
                                                                                        agentFrontLength,
                                                                                        agentRearLength,
                                                                                        velocity,
                                                                                        route);

    openScenario::LanePosition lanePosition;
    lanePosition.roadId = roadId;
    lanePosition.laneId = laneId;
    lanePosition.offset = 0.0;
    lanePosition.s = spawnDistance.value();

    SpawnInfo spawnInfo;
    spawnInfo.position = lanePosition;
    spawnInfo.velocity = adjustedVelocity;
    spawnInfo.acceleration = 0.0;

    return spawnInfo;
}



bool SpawnPointPreRunCommon::CalculateSpawnParameter(AgentBlueprintInterface* agentBlueprint,
                                                     const SpawnInfo& spawnInfo,
                                                     const Route &route)
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
    spawnParameter.route = route;

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
