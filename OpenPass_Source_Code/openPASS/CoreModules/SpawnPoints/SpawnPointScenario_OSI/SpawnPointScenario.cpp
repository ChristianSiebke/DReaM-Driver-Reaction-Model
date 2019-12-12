/*******************************************************************************
* Copyright (c) 2017, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "SpawnPointScenario.h"

#include "Interfaces/agentInterface.h"
#include "Interfaces/worldInterface.h"
#include "CoreFramework/OpenPassSlave/modelElements/agentBlueprint.h"
#include "CoreFramework/OpenPassSlave/framework/agentFactory.h"

SpawnPointScenario::SpawnPointScenario(const SpawnPointDependencies* dependencies,
                       const CallbackInterface* callbacks):
    SpawnPointInterface(dependencies->world, callbacks),
    dependencies(*dependencies)
{
    if (!dependencies->scenario)
    {
        LogError(std::string(COMPONENTNAME) + " received no scenario (required)");
    }
}

SpawnPointInterface::Agents SpawnPointScenario::Trigger()
{
    Agents agents;

    for (const auto& entity : dependencies.scenario.value()->GetEntities())
    {
        try
        {
            auto agentBlueprint = dependencies.agentBlueprintProvider->SampleAgent(entity.catalogReference.entryName);
            agentBlueprint.SetAgentProfileName(entity.catalogReference.entryName);
            SetPredefinedParameter(agentBlueprint, entity);

            if(!TryGetSpawnDistance(entity.spawnInfo.roadId,
                                    entity.spawnInfo.ILane,
                                    entity.spawnInfo.s))
            {
                LogError("Could not generate a value spawn distance for entity " + entity.name);
            }

            const SpawnInfo spawnInfo = GetNextSpawnCarInfo(entity.spawnInfo.ILane, agentBlueprint, entity);
            if(!CalculateSpawnParameter(agentBlueprint, spawnInfo))
            {
                LogError(" failed to calculate spawn parameters successfully for entity "
                       + entity.name);
            }

            SimulationSlave::Agent* newAgent = dependencies.agentFactory->AddAgent(&agentBlueprint);

            if(newAgent != nullptr)
            {
                agents.emplace_back(newAgent);
            }
            else
            {
                LogError(" failed to add agent successfully for entity "
                       + entity.name);
            }
        }
        catch(const std::runtime_error& error)
        {
            LogError("SpawnPointScenario encountered an Error: " + std::string(error.what()));
        }
    }

    return agents;
}

SpawnInfo SpawnPointScenario::GenerateSpawnInfo(SpawnInfo spawnInfo, int laneId, double egoVelocity, double rearCarLength)
{
    double spawnVelocity = spawnInfo.velocity.value;
    if (spawnVelocity == -999)
    {
        spawnVelocity = egoVelocity;
    }

    if (spawnInfo.acceleration.value == -999)
    {
        spawnInfo.acceleration.value = 0.0;
    }
    else
    {
        CalculateAttributeValue(spawnInfo.acceleration);
    }

    return {spawnInfo.s.value + rearCarLength,
            spawnVelocity,
            spawnInfo.roadId,
            laneId,
            spawnInfo.offset.value,
            spawnInfo.acceleration.value};
}

SpawnInfo SpawnPointScenario::GetNextSpawnCarInfo(const int laneId,
                                                  AgentBlueprintInterface& agentBlueprint,
                                                  const ScenarioEntity& entity)
{
    const double agentLength = agentBlueprint.GetVehicleModelParameters().length;
    const double agentFrontLength = agentBlueprint.GetVehicleModelParameters().distanceReferencePointToLeadingEdge;
    const double agentRearLength = agentLength - agentFrontLength;

    double velocity;

    auto velocityAttr = entity.spawnInfo.velocity;
    CalculateAttributeValue(velocityAttr);
    if (velocityAttr.value == -999)
    {
        LogError("initial velocity not defined for entity " + entity.name);
    }
    velocity = velocityAttr.value;

    SpawnInfo spawnInfo = GenerateSpawnInfo(entity.spawnInfo, laneId, velocity, agentRearLength);
    if (!AreSpawningCoordinatesValid(spawnInfo, agentBlueprint))
    {
        LogError("Coordinates of scenery agent are invalid.");
    }

    return spawnInfo;
}

void SpawnPointScenario::SetPredefinedParameter(AgentBlueprintInterface& agentBlueprint,
                                        const ScenarioEntity& scenarioEntity) const
{
    const SpawnInfo& spawnInfo = scenarioEntity.spawnInfo;
    SpawnParameter spawnParameter;

    spawnParameter.velocity = spawnInfo.velocity.value;
    spawnParameter.SpawningLaneId = spawnInfo.ILane;
    spawnParameter.SpawningRoadId = spawnInfo.roadId;
    spawnParameter.positionX = spawnInfo.s.value;
    spawnParameter.heading = spawnInfo.heading;

    agentBlueprint.SetAgentCategory(scenarioEntity.name == "Ego"
                                        ? AgentCategory::Ego
                                        : AgentCategory::Scenario);
    agentBlueprint.SetObjectName(scenarioEntity.name);
    agentBlueprint.SetSpawnParameter(spawnParameter);
}

bool SpawnPointScenario::CalculateSpawnParameter(AgentBlueprintInterface& agentBlueprint,
                                                 const SpawnInfo& spawnInfo)
{
    SpawnParameter& spawnParameter = agentBlueprint.GetSpawnParameter();
    spawnParameter.SpawningRoadId = spawnInfo.roadId;
    spawnParameter.SpawningLaneId = spawnInfo.ILane;

    double distance = spawnInfo.s.value;
    Position pos = GetWorld()->LaneCoord2WorldCoord(distance, spawnInfo.offset.value, spawnInfo.roadId, spawnInfo.ILane);

    double spawnV = spawnInfo.velocity.value;

    //considers adjusted velocity in curvatures
    double kappa = pos.curvature;

    // Note: This could falsify ego and scenario agents
    if (kappa != 0.0)
    {
        double curvatureVelocity;

        curvatureVelocity = 160 * (1 / (std::sqrt((std::abs(kappa)) / 1000)));

        spawnV = std::min(spawnV, curvatureVelocity);
    }

    spawnParameter.distance = spawnInfo.s.value;
    spawnParameter.positionX = pos.xPos;
    spawnParameter.positionY = pos.yPos;
    spawnParameter.yawAngle  = pos.yawAngle + agentBlueprint.GetSpawnParameter().heading;
    spawnParameter.velocity = spawnV;
    spawnParameter.acceleration = spawnInfo.acceleration.value;

    return true;
}

bool SpawnPointScenario::NewAgentIntersectsWithExistingAgent(const SpawnInfo& spawnInfo,
                                                     const AgentBlueprintInterface& agentBlueprint)
{
    Position pos = GetWorld()->LaneCoord2WorldCoord(spawnInfo.s.value, spawnInfo.offset.value, spawnInfo.roadId, spawnInfo.ILane);
    VehicleModelParameters vehicleModelParameters = agentBlueprint.GetVehicleModelParameters();

    return GetWorld()->IntersectsWithAgent(pos.xPos,
                                           pos.yPos,
                                           pos.yawAngle,
                                           vehicleModelParameters.length,
                                           vehicleModelParameters.width,
                                           vehicleModelParameters.distanceReferencePointToLeadingEdge);
}

bool SpawnPointScenario::TryGetSpawnDistance(std::string roadId, int laneId, SpawnAttribute sAttribute)
{
    int trials = sAttribute.isStochastic ? NUMBER_OF_TRIALS_STOCHASTIC : 1;

    for (int trial = 0; trial < trials; trial++)
    {
        CalculateAttributeValue(sAttribute);
        auto spawnDistance = sAttribute.value;

        if (GetWorld()->IsSValidOnLane(roadId, laneId, spawnDistance))
        {
            return true;
        }
    }

    return false;
}

void SpawnPointScenario::CalculateAttributeValue(SpawnAttribute& attribute)
{
    if (attribute.isStochastic)
    {
        attribute.value = dependencies.sampler->RollForStochasticAttribute(attribute.mean,
                          attribute.stdDeviation,
                          attribute.lowerBoundary,
                          attribute.upperBoundary);
    }
}

bool SpawnPointScenario::IsOffsetValidForLane(std::string roadId, int laneId, double distanceFromStart, double offset, double vehicleWidth)
{
    if (!GetWorld()->IsSValidOnLane(roadId, laneId, distanceFromStart))
    {
        LogError("Invalid offset. Lane is not available: " + std::to_string(laneId) + ". Distance from start: " +
                 std::to_string(distanceFromStart));
        return false;
    }

    //Check if lane width > vehicle width
    double laneWidth = GetWorld()->GetLaneWidth(Route{roadId}, roadId, laneId, distanceFromStart, 0);
    if (vehicleWidth > laneWidth + std::abs(offset))
    {
        LogError("Invalid offset. Lane width < vehicle width: " + std::to_string(laneId) + ". Distance from start: " +
                 std::to_string(distanceFromStart) + ". Lane width: " + std::to_string(laneWidth) + ". Vehicle width: " + std::to_string(
                     vehicleWidth));
        return false;
    }

    //Is vehicle completely inside the lane
    bool isVehicleCompletelyInLane = (laneWidth - vehicleWidth) * 0.5 >= std::abs(offset);
    if (isVehicleCompletelyInLane)
    {
        return true;
    }

    //Is vehicle more than 50 % on the lane
    double allowedRange = laneWidth * 0.5;
    bool outsideAllowedRange =  std::abs(offset) > allowedRange;
    if (outsideAllowedRange)
    {
        LogError("Invalid offset. Vehicle not inside allowed range: " + std::to_string(laneId) + ". Invalid offset: " +
                 std::to_string(offset));
        return false;
    }

    //Is vehicle partly on invalid lane
    bool isOffsetToLeftLane = (offset >= 0);
    int otherLaneId = isOffsetToLeftLane ? (laneId + 1) : (laneId - 1);

    if (!GetWorld()->IsSValidOnLane(roadId, otherLaneId, distanceFromStart)) //other lane is invalid
    {
        LogError("Invalid offset. Other lane is invalid: " + std::to_string(laneId) + ". Invalid offset: " + std::to_string(
                     offset));
        return false;
    }

    return true;
}

void SpawnPointScenario::LogWarning(const std::string& message)
{
    std::stringstream log;
    log.str(std::string());
    log << COMPONENTNAME << " " << message;
    LOG(CbkLogLevel::Warning, log.str());
}

void SpawnPointScenario::LogError(const std::string& message)
{
    std::stringstream log;
    log.str(std::string());
    log << COMPONENTNAME << " " << message;
    LOG(CbkLogLevel::Error, log.str());
    throw std::runtime_error(log.str());
}

bool SpawnPointScenario::AreSpawningCoordinatesValid(const SpawnInfo& spawnInfo,
                                             const AgentBlueprintInterface& agentBlueprint)
{
    if (!GetWorld()->IsSValidOnLane(spawnInfo.roadId, spawnInfo.ILane, spawnInfo.s.value))
    {
        LogWarning("S is not valid for vehicle on lane: " + std::to_string(spawnInfo.ILane) + ". Invalid s: " + std::to_string(
                       spawnInfo.s.value));
        return false;
    }

    if (!IsOffsetValidForLane(spawnInfo.roadId, spawnInfo.ILane, spawnInfo.s.value, spawnInfo.offset.value, agentBlueprint.GetVehicleModelParameters().width))
    {
        LogError("Offset is not valid for vehicle on lane: " + std::to_string(spawnInfo.ILane) + ". Invalid offset: " + std::to_string(
                     spawnInfo.offset.value));

        return false;
    }

    if (NewAgentIntersectsWithExistingAgent(spawnInfo, agentBlueprint))
    {
        LogWarning("New Agent intersects existing agent on lane: " + std::to_string(spawnInfo.ILane) + ".");
        return false;
    }

    return true;
}
