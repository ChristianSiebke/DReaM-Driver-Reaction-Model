/*******************************************************************************
* Copyright (c) 2017, 2019, 2020, 2021 in-tech GmbH
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
    parameters(ExtractSpawnPointParameters(*(dependencies->parameters.value()), *dependencies->world, callbacks)),
    worldAnalyzer(dependencies->world)
{}

SpawnPointInterface::Agents SpawnPointPreRunCommon::Trigger([[maybe_unused]]int time)
{
    SpawnPointInterface::Agents newAgents;

    for (const auto &spawnArea : parameters.spawnAreas)
    {
        const auto validLaneSpawningRanges = worldAnalyzer.GetValidLaneSpawningRanges(spawnArea.laneStream,
                                                                                      spawnArea.sStart,
                                                                                      spawnArea.sEnd,
                                                                                      supportedLaneTypes);

        for (const auto& validSpawningRange : validLaneSpawningRanges)
        {
            const auto generatedAgents = GenerateAgentsForRange(spawnArea.laneStream,
                                                                validSpawningRange);
            newAgents.insert(std::cend(newAgents),
                             std::cbegin(generatedAgents),
                             std::cend(generatedAgents));
        }
    }

    return newAgents;
}

SpawningAgentProfile SpawnPointPreRunCommon::SampleAgentProfile(bool rightLane)
{
    return Sampler::Sample(rightLane ? parameters.agentProfileLaneMaps.rightLanes : parameters.agentProfileLaneMaps.leftLanes, dependencies.stochastics);
}

SpawnPointInterface::Agents SpawnPointPreRunCommon::GenerateAgentsForRange(const std::unique_ptr<LaneStreamInterface>& laneStream,
                                                                           const Range& range)
{
    SpawnPointInterface::Agents agents;

    bool generating = true;

    size_t rightLaneCount = worldAnalyzer.GetRightLaneCount(laneStream, range.second);

    while (generating)
    {
        const auto agentProfile = SampleAgentProfile(rightLaneCount == 0);

        try
        {
            auto agentBlueprint = dependencies.agentBlueprintProvider->SampleAgent(agentProfile.name, {});
            agentBlueprint.SetAgentProfileName(agentProfile.name);
            agentBlueprint.SetAgentCategory(AgentCategory::Common);

            const auto agentLength = agentBlueprint.GetVehicleModelParameters().boundingBoxDimensions.length;
            const auto agentFrontLength = agentLength * 0.5 + agentBlueprint.GetVehicleModelParameters().boundingBoxCenter.x;
            const auto agentRearLength = agentLength - agentFrontLength;

            auto velocity = Sampler::RollForStochasticAttribute(agentProfile.velocity, dependencies.stochastics);

            for (size_t iterator = 0; iterator < rightLaneCount; ++iterator)
            {
                double homogeneity = agentProfile.homogeneities.size() > iterator ? agentProfile.homogeneities[iterator] : 1.0;
                velocity *= 2.0 - homogeneity;
            }

            const auto tGap = Sampler::RollForStochasticAttribute(agentProfile.tGap, dependencies.stochastics);

            const auto spawnInfo = GetNextSpawnCarInfo(laneStream, range, tGap, velocity, agentFrontLength, agentRearLength);
            if (!spawnInfo.has_value())
            {
                generating = false;
                break;
            }
            if (!worldAnalyzer.AreSpawningCoordinatesValid(spawnInfo->roadId,
                                                           spawnInfo->laneId,
                                                           spawnInfo->s,
                                                           0,
                                                           GetStochasticOrPredefinedValue(parameters.minimumSeparationBuffer, dependencies.stochastics),
                                                           spawnInfo->spawnParameter.route,
                                                           agentBlueprint.GetVehicleModelParameters())
                || worldAnalyzer.SpawnWillCauseCrash(laneStream,
                                                     spawnInfo->streamPosition,
                                                     agentFrontLength,
                                                     agentRearLength,
                                                     velocity,
                                                     Direction::BACKWARD))
            {
                generating = false;
                break;
            }

            agentBlueprint.SetSpawnParameter(spawnInfo->spawnParameter);

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
            rightLaneCount = worldAnalyzer.GetRightLaneCount(laneStream, spawnInfo->streamPosition);
        }
        catch (const std::runtime_error& error)
        {
            LogError(error.what());
        }
    }
    return agents;
}

std::optional<SpawnInfo> SpawnPointPreRunCommon::GetNextSpawnCarInfo(const std::unique_ptr<LaneStreamInterface>& laneStream,
                                                                     const Range& range,
                                                                     const double gapInSeconds,
                                                                     const double velocity,
                                                                     const double agentFrontLength,
                                                                     const double agentRearLength) const
{
    auto spawnDistance = worldAnalyzer.GetNextSpawnPosition(laneStream,
                                                            range,
                                                            agentFrontLength,
                                                            agentRearLength,
                                                            velocity,
                                                            gapInSeconds,
                                                            GetStochasticOrPredefinedValue(parameters.minimumSeparationBuffer, dependencies.stochastics));

    if (!spawnDistance.has_value())
    {
        return {};
    }

    auto roadPosition = laneStream->GetRoadPosition({spawnDistance.value(), 0});
    auto route = worldAnalyzer.SampleRoute(roadPosition.roadId,
                                           roadPosition.laneId,
                                           dependencies.stochastics);

    //We don't want to spawn agents too close to the end of lane, so adjust the spawn position if neccessary
    spawnDistance = worldAnalyzer.CalculateAdjustedSpawnDistanceToEndOfLane(roadPosition.laneId, roadPosition.roadPosition.s, spawnDistance.value(), velocity, route, supportedLaneTypes);
    if (spawnDistance.value() < range.first)
    {
        return {};
    }
    roadPosition = laneStream->GetRoadPosition({spawnDistance.value(), 0});

    const auto adjustedVelocity = worldAnalyzer.CalculateSpawnVelocityToPreventCrashing(laneStream,
                                                                                        *spawnDistance,
                                                                                        agentFrontLength,
                                                                                        agentRearLength,
                                                                                        velocity);

    auto worldPosition = GetWorld()->LaneCoord2WorldCoord(roadPosition.roadPosition.s, roadPosition.roadPosition.t, roadPosition.roadId, roadPosition.laneId);

    //considers adjusted velocity in curvatures
    double spawnV = adjustedVelocity;
    double kappa = worldPosition.curvature;
    if (kappa != 0.0)
    {
        double curvatureVelocity;

        curvatureVelocity = 160 * (1 / (std::sqrt((std::abs(kappa)) / 1000)));

        spawnV = std::min(spawnV, curvatureVelocity);
    }

    SpawnInfo spawnInfo;

    spawnInfo.roadId = roadPosition.roadId;
    spawnInfo.laneId = roadPosition.laneId;
    spawnInfo.s = roadPosition.roadPosition.s;
    spawnInfo.streamPosition = spawnDistance.value();
    spawnInfo.spawnParameter.positionX = worldPosition.xPos;
    spawnInfo.spawnParameter.positionY = worldPosition.yPos;
    spawnInfo.spawnParameter.yawAngle  = CommonHelper::SetAngleToValidRange(worldPosition.yawAngle + (roadPosition.laneId < 0 ? 0 : M_PI));
    spawnInfo.spawnParameter.velocity = spawnV;
    spawnInfo.spawnParameter.acceleration = 0.0;
    spawnInfo.spawnParameter.route = route;

    return spawnInfo;
}


void SpawnPointPreRunCommon::LogError(const std::string& message)
{
    std::stringstream log;
    log.str(std::string());
    log << COMPONENTNAME << " " << message;
    LOG(CbkLogLevel::Error, log.str());
    throw std::runtime_error(log.str());
}
