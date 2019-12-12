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

#include "CoreFramework/OpenPassSlave/framework/agentBlueprintProvider.h"
#include "CoreFramework/OpenPassSlave/framework/agentFactory.h"

SpawnPointRuntimeCommon::SpawnPointRuntimeCommon(const SpawnPointDependencies* dependencies,
                                                 const CallbackInterface * const callbacks):
    SpawnPointInterface(dependencies->world, callbacks),
    dependencies(*dependencies),
    parameters(SpawnPointRuntimeCommonParameterExtractor::ExtractSpawnPointParameters(*(dependencies->parameters.value()),
                                                                                      dependencies->sampler)),
    worldAnalyzer(dependencies->world)
{}

SpawnPointInterface::Agents SpawnPointRuntimeCommon::Trigger()
{
    if(parameters.carsPerSecond <= 0.0)
    {
        std::stringstream log;
        log.str(std::string());
        log << COMPONENTNAME << ": Unable to spawn agents (carsPerSecond 0 or negative: " << parameters.carsPerSecond << ")";
        LOG(CbkLogLevel::Warning, log.str());
        return {};
    }

    SpawnPointInterface::Agents agents;

    std::for_each(std::cbegin(parameters.laneIds),
                  std::cend(parameters.laneIds),
                  [&](const auto laneId) -> void
    {
        if (auto spawnDetail = GetSpawnDetailsForLane(laneId))
        {
            if (ShouldSpawnAgent(*spawnDetail,
                                 laneId))
            {
                SimulationSlave::Agent* newAgent = dependencies.agentFactory->AddAgent(&(spawnDetail->second));

                if (newAgent != nullptr)
                {
                    agents.emplace_back(newAgent);
                }

                queuedSpawnDetails.erase(laneId);
            }
            else
            {
                queuedSpawnDetails[laneId] = *spawnDetail;
            }
        }
    });

    return agents;
}

std::optional<SpawnDetails> SpawnPointRuntimeCommon::GetSpawnDetailsForLane(const LaneId laneId)
{
    using namespace helper;
    auto spawnDetail = map::query(queuedSpawnDetails,
                                  laneId);
    return spawnDetail.has_value()
         ? spawnDetail
         : GenerateSpawnDetailsForLane(laneId);
}

SpawnDetails SpawnPointRuntimeCommon::GenerateSpawnDetailsForLane(const LaneId laneId)
{
    const auto agentProfileName = dependencies.sampler->SampleStringProbability(parameters.agentProfiles);
    try
    {
        auto agentBlueprint = dependencies.agentBlueprintProvider->SampleAgent(agentProfileName);
        agentBlueprint.SetAgentProfileName(agentProfileName);
        agentBlueprint.SetAgentCategory(AgentCategory::Common);

        const auto velocity = dependencies.sampler->RollForStochasticAttribute(parameters.trafficVelocityDistribution.mean,
                                                                               parameters.trafficVelocityDistribution.standardDeviation,
                                                                               parameters.trafficVelocityDistribution.min,
                                                                               parameters.trafficVelocityDistribution.max);

        CalculateSpawnParameter(&agentBlueprint,
                                parameters.roadId,
                                laneId,
                                parameters.spawnLocation,
                                velocity);

        return SpawnDetails(RollGapBetweenCars(), agentBlueprint);
    }
    catch (const std::runtime_error& error)
    {
        LogError("Encountered an Error while generating Spawn Details: " + std::string(error.what()));
    }
}

double SpawnPointRuntimeCommon::RollGapBetweenCars() const
{
    double gapInSeconds = dependencies.sampler->RollGapBetweenCars(parameters.carsPerSecond);

    if (!dependencies.sampler->RollFor(parameters.platoonRate))
    {
        gapInSeconds += dependencies.sampler->RollGapExtension(NON_PLATOON_GAP_EXTENSION);
    }

    return gapInSeconds;
}

bool SpawnPointRuntimeCommon::ShouldSpawnAgent(const SpawnDetails& spawnDetails,
                                               const LaneId laneId) const
{
    const auto [gapInSeconds, agentBlueprint] = spawnDetails;
    const auto vehicleModelParameters = agentBlueprint.GetVehicleModelParameters();

    if (worldAnalyzer.AreSpawningCoordinatesValid(parameters.roadId,
                                                  laneId,
                                                  agentBlueprint.GetSpawnParameter().distance,
                                                  0 /* offset */,
                                                  vehicleModelParameters))
    {
        const auto agentLength = vehicleModelParameters.length;
        const auto agentFrontLength = vehicleModelParameters.distanceReferencePointToLeadingEdge;
        const auto agentRearLength = agentLength - agentFrontLength;
        const auto intendedVelocity = agentBlueprint.GetSpawnParameter().velocity;
        const Route routeForRoadId{parameters.roadId};
        const Range spawnRangeForLane{parameters.spawnLocation, std::numeric_limits<double>::max()};

        // if nextSpawnPosition evaluates to true, our spawn position is valid
        if (const auto nextSpawnPosition = worldAnalyzer.GetNextSpawnPosition(routeForRoadId,
                                                                              parameters.roadId,
                                                                              laneId,
                                                                              spawnRangeForLane,
                                                                              agentFrontLength,
                                                                              agentRearLength,
                                                                              intendedVelocity,
                                                                              gapInSeconds,
                                                                              Direction::FORWARD))
        {
            return !(worldAnalyzer.SpawnWillCauseCrash(routeForRoadId,
                                                       parameters.roadId,
                                                       laneId,
                                                       parameters.spawnLocation,
                                                       agentFrontLength,
                                                       agentRearLength,
                                                       intendedVelocity,
                                                       Direction::FORWARD)
                  || worldAnalyzer.SpawnWillCauseCrash(routeForRoadId,
                                                       parameters.roadId,
                                                       laneId,
                                                       parameters.spawnLocation,
                                                       agentFrontLength,
                                                       agentRearLength,
                                                       intendedVelocity,
                                                       Direction::BACKWARD));
        }
    }

    return false;
}

void SpawnPointRuntimeCommon::CalculateSpawnParameter(AgentBlueprintInterface* agentBlueprint,
                                                      const RoadId& roadId,
                                                      const LaneId laneId,
                                                      const SPosition sPosition,
                                                      const double velocity) const
{
    SpawnParameter& spawnParameter = agentBlueprint->GetSpawnParameter();
    spawnParameter.SpawningRoadId = roadId;
    spawnParameter.SpawningLaneId = laneId;

    double distance = sPosition;
    Position pos = GetWorld()->LaneCoord2WorldCoord(distance, 0 /* offset */, roadId, laneId);

    double spawnV = velocity;

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

    spawnParameter.distance = sPosition;
    spawnParameter.positionX = pos.xPos;
    spawnParameter.positionY = pos.yPos;
    spawnParameter.yawAngle  = pos.yawAngle + spawnParameter.heading;
    spawnParameter.velocity = spawnV;
    spawnParameter.acceleration = 0;
}

void SpawnPointRuntimeCommon::LogError(const std::string& message)
{
    std::stringstream log;
    log.str(std::string());
    log << COMPONENTNAME << " " << message;
    LOG(CbkLogLevel::Error, log.str());
    throw std::runtime_error(log.str());
}
