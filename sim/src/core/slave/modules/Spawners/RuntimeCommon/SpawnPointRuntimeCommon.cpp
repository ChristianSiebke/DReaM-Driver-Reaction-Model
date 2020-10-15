/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "SpawnPointRuntimeCommon.h"
#include "SpawnPointRuntimeCommonParameterExtractor.h"

#include "framework/agentBlueprintProvider.h"
#include "framework/agentFactory.h"
#include "framework/sampler.h"

SpawnPointRuntimeCommon::SpawnPointRuntimeCommon(const SpawnPointDependencies* dependencies,
                                                 const CallbackInterface * const callbacks):
    SpawnPointInterface(dependencies->world, callbacks),
    dependencies(*dependencies),
    worldAnalyzer(dependencies->world),
    parameters(SpawnPointRuntimeCommonParameterExtractor::ExtractSpawnPointParameters(*(dependencies->parameters.value()), worldAnalyzer))
{
    for (const auto& spawnPosition : parameters.spawnPositions)
    {
        queuedSpawnDetails.push_back(GenerateSpawnDetailsForLane(spawnPosition, 0));
    }
}

SpawnPointInterface::Agents SpawnPointRuntimeCommon::Trigger(int time)
{
    SpawnPointInterface::Agents agents;

    for (size_t i = 0; i < parameters.spawnPositions.size(); ++i)
    {
        if (time >= queuedSpawnDetails[i].spawnTime && AreSpawningCoordinatesValid(queuedSpawnDetails[i], parameters.spawnPositions[i]))
        {
            AdjustVelocityForCrash(queuedSpawnDetails[i], parameters.spawnPositions[i]);
            SimulationSlave::Agent* newAgent = dependencies.agentFactory->AddAgent(&(queuedSpawnDetails[i].agentBlueprint));

            if (newAgent != nullptr)
            {
                agents.emplace_back(newAgent);
            }

            queuedSpawnDetails[i] = GenerateSpawnDetailsForLane(parameters.spawnPositions[i], time);
        }
    }

    return agents;
}

SpawnDetails SpawnPointRuntimeCommon::GenerateSpawnDetailsForLane(const SpawnPosition sceneryInformation, int time)
{
    auto rightLaneCount = worldAnalyzer.GetRightLaneCount(sceneryInformation.roadId, sceneryInformation.laneId, sceneryInformation.sPosition);
    const auto agentProfile = SampleAgentProfile(rightLaneCount == 0);
    try
    {
        auto agentBlueprint = dependencies.agentBlueprintProvider->SampleAgent(agentProfile.name, {});
        agentBlueprint.SetAgentProfileName(agentProfile.name);
        agentBlueprint.SetAgentCategory(AgentCategory::Common);

        const auto tGap = Sampler::RollForStochasticAttribute(agentProfile.tGap, dependencies.stochastics);

        auto velocity = Sampler::RollForStochasticAttribute(agentProfile.velocity, dependencies.stochastics);

        for (size_t i = 0; i < rightLaneCount; ++i)
        {
            double homogeneity = agentProfile.homogeneities.size() > i ? agentProfile.homogeneities[i] : 1.0;
            velocity *= 2.0 - homogeneity;
        }

        CalculateSpawnParameter(&agentBlueprint,
                                sceneryInformation.roadId,
                                sceneryInformation.laneId,
                                sceneryInformation.sPosition,
                                velocity);

        return SpawnDetails{static_cast<int>(tGap * 1000) + time, agentBlueprint};
    }
    catch (const std::runtime_error& error)
    {
        LogError("Encountered an Error while generating Spawn Details: " + std::string(error.what()));
    }
}

void SpawnPointRuntimeCommon::AdjustVelocityForCrash(SpawnDetails& spawnDetails,
                                                     const SpawnPosition& sceneryInformation) const
{
    const auto agentFrontLength = spawnDetails.agentBlueprint.GetVehicleModelParameters().distanceReferencePointToLeadingEdge;
    const auto agentRearLength = spawnDetails.agentBlueprint.GetVehicleModelParameters().length - spawnDetails.agentBlueprint.GetVehicleModelParameters().distanceReferencePointToLeadingEdge;
    const auto intendedVelocity = spawnDetails.agentBlueprint.GetSpawnParameter().velocity;

    if (spawnDetails.agentBlueprint.GetSpawnParameter().route.has_value())
    {
        spawnDetails.agentBlueprint.GetSpawnParameter().velocity = worldAnalyzer.CalculateSpawnVelocityToPreventCrashing(sceneryInformation.roadId,
                                                                                                                         sceneryInformation.laneId,
                                                                                                                         sceneryInformation.sPosition,
                                                                                                                         agentFrontLength,
                                                                                                                         agentRearLength,
                                                                                                                         intendedVelocity,
                                                                                                                         spawnDetails.agentBlueprint.GetSpawnParameter().route.value());
    }
    else
    {
        LogErrorAndThrow("Can't adjust spawn velocity because of missing route.");
    }
}

bool SpawnPointRuntimeCommon::AreSpawningCoordinatesValid(const SpawnDetails& spawnDetails,
                                                   const SpawnPosition& sceneryInformation) const
{
    const auto vehicleModelParameters = spawnDetails.agentBlueprint.GetVehicleModelParameters();

    if (spawnDetails.agentBlueprint.GetSpawnParameter().route.has_value())
    {
            return worldAnalyzer.AreSpawningCoordinatesValid(sceneryInformation.roadId,
                                                             sceneryInformation.laneId,
                                                             sceneryInformation.sPosition,
                                                             0 /* offset */,
                                                             spawnDetails.agentBlueprint.GetSpawnParameter().route.value(),
                                                             vehicleModelParameters);
    }
    else
    {
        LogErrorAndThrow("Can't validate spawning coordinates because of missing route.");
    }

}

void SpawnPointRuntimeCommon::CalculateSpawnParameter(AgentBlueprintInterface* agentBlueprint,
                                                      const RoadId& roadId,
                                                      const LaneId laneId,
                                                      const SPosition sPosition,
                                                      const double velocity) const
{
    Position pos = GetWorld()->LaneCoord2WorldCoord(sPosition, 0 /* offset */, roadId, laneId);

    double spawnV = velocity;

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
    spawnParameter.acceleration = 0;
    spawnParameter.route = worldAnalyzer.SampleRoute(roadId,
                                                     laneId,
                                                     dependencies.stochastics);
}

SpawningAgentProfile SpawnPointRuntimeCommon::SampleAgentProfile(bool rightLane)
{
    return Sampler::Sample(rightLane ? parameters.agentProfileLaneMaps.rightLanes : parameters.agentProfileLaneMaps.leftLanes, dependencies.stochastics);
}

void SpawnPointRuntimeCommon::LogError(const std::string& message)
{
    std::stringstream log;
    log.str(std::string());
    log << COMPONENTNAME << " " << message;
    LOG(CbkLogLevel::Error, log.str());
    throw std::runtime_error(log.str());
}
