/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "WorldAnalyzer.h"
#include "Interfaces/agentInterface.h"
#include "Common/commonTools.h"

namespace
{
    // used to prevent agents from being re-found as next agent in lane in forward GetNextAgentInLane() searches
    static constexpr double FORWARD_SEARCH_PADDING = 0.01;

    static constexpr double MINIMUM_GAP_IN_SECONDS = 1.0;
    static constexpr double MINIMUM_SEPARATION_BUFFER = 2.0;
    static constexpr double MIN_DISTANCE_TO_END_OF_LANE = 50.0;

    static constexpr double ASSUMED_TTB = 1.0;
    static constexpr double ASSUMED_BRAKING_ACCELERATION = -6.0;
    static constexpr double ASSUMED_FRONT_AGENT_ACCELERATION = -10;
    static constexpr double VELOCITY_STEP_KM_H = 10.0;


    static VehicleRearAndFrontCoordinates CalculateVehicleBoundingSCoordinates(const AgentInterface * const scenarioAgentForStream)
    {
        const auto vehicleModelParameters = scenarioAgentForStream->GetVehicleModelParameters();
        const auto agentLength = vehicleModelParameters.length;
        const auto agentFrontLength = vehicleModelParameters.distanceReferencePointToLeadingEdge;
        const auto agentRearLength = agentLength - agentFrontLength;
        const auto agentS = scenarioAgentForStream->GetRoadPosition().s;

        const auto agentRearS = agentS - agentRearLength;
        const auto agentFrontS = agentS + agentFrontLength;
        return std::make_pair(agentRearS, agentFrontS);
    }

    static inline double GetSeparation(const double gapInSeconds,
                                       const double velocity,
                                       const double consideredCarLengths)
    {
        const auto minimumBuffer = MINIMUM_SEPARATION_BUFFER + consideredCarLengths + MINIMUM_GAP_IN_SECONDS * velocity;

        return std::max((gapInSeconds * velocity) + consideredCarLengths, minimumBuffer);
    }

    static inline double CalculateFullBrakingDistance(const double velocity,
                                                      const double ttb,
                                                      const double brakingAcceleration)
    {
        return velocity * ttb + velocity * velocity / (2 * std::abs(brakingAcceleration));
    }
}

std::optional<ValidLaneSpawningRanges> WorldAnalyzer::GetValidLaneSpawningRanges(const Route& routeForRoadId,
                                                                                 const RoadId& roadId,
                                                                                 const LaneId laneId,
                                                                                 const SPosition sStart,
                                                                                 const SPosition sEnd) const
{
    const auto agentsInLane = world->GetAgentsInRange(routeForRoadId,
                                                      roadId,
                                                      laneId,
                                                      sStart,
                                                      std::numeric_limits<double>::infinity(),
                                                      std::numeric_limits<double>::infinity());

    std::vector<const AgentInterface*> scenarioAgents {};
    for (auto agent : agentsInLane)
    {
        if(agent->GetAgentCategory() == AgentCategory::Ego
           || agent->GetAgentCategory() == AgentCategory::Scenario)
        {
            scenarioAgents.push_back(agent);
        }
    }

    if(scenarioAgents.empty())
    {
        ValidLaneSpawningRanges validLaneSpawningRanges{{sStart, sEnd}};
        return std::make_optional(validLaneSpawningRanges);
    }
    else
    {
        const auto frontAgent = scenarioAgents.back();
        const auto frontAgentDistance = frontAgent->GetRoadPosition().s + frontAgent->GetVehicleModelParameters().distanceReferencePointToLeadingEdge;

        const auto rearAgent = scenarioAgents.front();
        const auto vehicleModelParameters = rearAgent->GetVehicleModelParameters();
        const auto distanceReferencePointToRearEdge = vehicleModelParameters.length - vehicleModelParameters.distanceReferencePointToLeadingEdge;
        const auto rearAgentDistance = rearAgent->GetRoadPosition().s - distanceReferencePointToRearEdge;

        return GetValidSpawningInformationForRange(sStart,
                                                   sEnd,
                                                   rearAgentDistance,
                                                   frontAgentDistance);
    }
}

std::optional<double> WorldAnalyzer::GetNextSpawnPosition(const Route& routeForRoadId,
                                                          const RoadId& roadId,
                                                          const LaneId laneId,
                                                          const Range& bounds,
                                                          const double agentFrontLength,
                                                          const double agentRearLength,
                                                          const double intendedVelocity,
                                                          const double gapInSeconds,
                                                          const Direction direction) const
{
    double spawnDistance;

    const auto maxSearchPosition = bounds.second + intendedVelocity * gapInSeconds;
    const auto downstreamObjects = world->GetAgentsInRange(routeForRoadId,
                                                           roadId,
                                                           laneId,
                                                           bounds.first,
                                                           direction == Direction::FORWARD ? 0 : maxSearchPosition,
                                                           direction == Direction::FORWARD ? maxSearchPosition : 0);
    const AgentInterface* firstDownstreamObject = nullptr;
    if (!downstreamObjects.empty())
    {
        firstDownstreamObject = direction == Direction::FORWARD ? downstreamObjects.front() : downstreamObjects.back();
    }
  if (!firstDownstreamObject || firstDownstreamObject->GetDistanceToStartOfRoad() > maxSearchPosition)
  {
      const auto drivingLaneLength = world->GetDistanceToEndOfLane(routeForRoadId,
                                                                   roadId,
                                                                   laneId,
                                                                   0,
                                                                   std::numeric_limits<double>::max(),
                                                                   {LaneType::Driving});
      spawnDistance = std::min(bounds.second, drivingLaneLength) - agentFrontLength;

      const auto distanceToEndOfDrivingLane = world->GetDistanceToEndOfLane(routeForRoadId,
                                                                            roadId,
                                                                            laneId,
                                                                            spawnDistance + agentFrontLength,
                                                                            bounds.second,
                                                                            {LaneType::Driving});

      if (distanceToEndOfDrivingLane < MIN_DISTANCE_TO_END_OF_LANE)
      {
          const auto difference = MIN_DISTANCE_TO_END_OF_LANE - distanceToEndOfDrivingLane;
          spawnDistance -= difference;
      }
  }
  else
  {
      const auto frontCarPositionOnRoad = firstDownstreamObject->GetDistanceToStartOfRoad();
      const auto frontCarRearLength = firstDownstreamObject->GetLength()
                                    - firstDownstreamObject->GetDistanceReferencePointToLeadingEdge();
      const auto separation = GetSeparation(gapInSeconds,
                                            intendedVelocity,
                                            frontCarRearLength + agentFrontLength);

      spawnDistance = frontCarPositionOnRoad - separation;
  }

    if (spawnDistance - agentRearLength < bounds.first)
    {
        return std::nullopt;
    }

    return spawnDistance;
}

double WorldAnalyzer::CalculateSpawnVelocityToPreventCrashing(const Route& routeForRoadId,
                                                              const RoadId& roadId,
                                                              const LaneId laneId,
                                                              const double intendedSpawnPosition,
                                                              const double agentFrontLength,
                                                              const double agentRearLength,
                                                              const double intendedVelocity) const
{
    const auto vehicleLength = agentFrontLength + agentRearLength;
    const auto intendedVehicleFrontPosition = intendedSpawnPosition + agentFrontLength;
    const auto fullBrakingDistance = CalculateFullBrakingDistance(intendedVelocity,
                                                                  ASSUMED_TTB,
                                                                  ASSUMED_BRAKING_ACCELERATION);
    const auto maxSearchDistance = intendedVehicleFrontPosition + fullBrakingDistance;

    auto opponentSearchDistance = intendedVehicleFrontPosition - vehicleLength;
    auto adjustedVelocity = intendedVelocity;

    while (opponentSearchDistance <= maxSearchDistance)
    {
        double freeSpace = 0;
        double frontObjectVelocity = 0;
        double frontObjectAcceleration = 0;

        const auto nextObjectsInLane =  world->GetAgentsInRange(routeForRoadId,
                                                                roadId,
                                                                laneId,
                                                                opponentSearchDistance,
                                                                0,
                                                                std::numeric_limits<double>::infinity());
        const AgentInterface* opponent = nullptr;
        if (!nextObjectsInLane.empty())
        {
            opponent = nextObjectsInLane.front();
            freeSpace = opponent->GetDistanceToStartOfRoad(MeasurementPoint::Rear) - intendedVehicleFrontPosition;
            frontObjectVelocity = opponent->GetVelocity();
            frontObjectAcceleration = ASSUMED_FRONT_AGENT_ACCELERATION;
        }
        else
        {
            freeSpace = world->GetDistanceToEndOfLane(routeForRoadId,
                                                      roadId,
                                                      laneId,
                                                      intendedVehicleFrontPosition,
                                                      maxSearchDistance,
                                                      {LaneType::Driving});
        }

        if (freeSpace < fullBrakingDistance)
        {
            while (adjustedVelocity > 0
                && TrafficHelperFunctions::WillCrash(freeSpace,
                                                     adjustedVelocity,
                                                     ASSUMED_BRAKING_ACCELERATION,
                                                     frontObjectVelocity,
                                                     frontObjectAcceleration,
                                                     ASSUMED_TTB))
            {
                adjustedVelocity -= CommonHelper::KilometerPerHourToMeterPerSecond(VELOCITY_STEP_KM_H);
            }
            adjustedVelocity = std::max(0.0, adjustedVelocity);
        }

        if (!opponent)
        {
            return adjustedVelocity;
        }

        opponentSearchDistance = opponent->GetDistanceToStartOfRoad(MeasurementPoint::Front);
    }

    return adjustedVelocity;
}

bool WorldAnalyzer::AreSpawningCoordinatesValid(const RoadId& roadId,
                                                const LaneId laneId,
                                                const SPosition sPosition,
                                                const double offset,
                                                const VehicleModelParameters& vehicleModelParameters) const
{
    if (laneId >= 0) //HOTFIX: Spawning on left lanes is currently not supported
    {        return false;
    }
    if (!world->IsSValidOnLane(roadId, laneId, sPosition))
    {
        loggingCallback("S is not valid for vehicle on lane: " + std::to_string(laneId) + ". Invalid s: " + std::to_string(
                       sPosition));
        return false;
    }

    if (!IsOffsetValidForLane(roadId, laneId, sPosition, offset, vehicleModelParameters.width))
    {
        loggingCallback("Offset is not valid for vehicle on lane: " + std::to_string(laneId) + ". Invalid offset: " + std::to_string(
                     offset));

        return false;
    }

    if (NewAgentIntersectsWithExistingAgent(roadId, laneId, sPosition, offset, vehicleModelParameters))
    {
        loggingCallback("New Agent intersects existing agent on lane: " + std::to_string(laneId) + ".");
        return false;
    }

    if (world->GetDistanceToEndOfLane(Route{roadId, laneId < 0}, roadId, laneId, sPosition, INFINITY, {LaneType::Driving}) < MIN_DISTANCE_TO_END_OF_LANE)
    {
        loggingCallback("End of lane: " + std::to_string(laneId) + " is too close.");
        return false;
    }
    return true;
}

std::optional<ValidLaneSpawningRanges> WorldAnalyzer::GetValidSpawningInformationForRange(const double sStart,
                                                                                          const double sEnd,
                                                                                          const double firstScenarioAgentSPosition,
                                                                                          const double lastScenarioAgentSPosition)
{
    // if the stream segment this SpawnPoint is responsible for is surrounded by scenario agents,
    // no valid spawn locations exist here
    if (firstScenarioAgentSPosition < sStart
        && lastScenarioAgentSPosition > sEnd)
    {
        return std::nullopt;
    }
    // if the stream segment this SpawnPoint is responsible for has one scenario agent within
    // its range and one without, only a portion of the specified range is valid
    ValidLaneSpawningRanges validLaneSpawningInformation;
    if ((firstScenarioAgentSPosition < sStart && lastScenarioAgentSPosition < sStart)
     || (firstScenarioAgentSPosition > sEnd && lastScenarioAgentSPosition > sEnd))
    {
        validLaneSpawningInformation.emplace_back(sStart, sEnd);
        return std::make_optional(validLaneSpawningInformation);
    }
    // if the first scenario s position is within our range, exclude everything after that (except as below)
    if (firstScenarioAgentSPosition > sStart
     && firstScenarioAgentSPosition < sEnd)
    {
        validLaneSpawningInformation.emplace_back(sStart, firstScenarioAgentSPosition);
    }

    // if the last scenario s position is within our range, exclude everything before that (except as above)
    if (lastScenarioAgentSPosition > sStart
     && lastScenarioAgentSPosition < sEnd)
    {
        validLaneSpawningInformation.emplace_back(lastScenarioAgentSPosition, sEnd);
    }

    return std::make_optional(validLaneSpawningInformation);
}

bool WorldAnalyzer::IsOffsetValidForLane(const RoadId& roadId,
                                         const LaneId laneId,
                                         const SPosition distanceFromStart,
                                         const double offset,
                                         const double vehicleWidth) const
{
    if (!world->IsSValidOnLane(roadId, laneId, distanceFromStart))
    {
        loggingCallback("Invalid offset. Lane is not available: " + std::to_string(laneId) + ". Distance from start: " +
                 std::to_string(distanceFromStart));
        return false;
    }

    //Check if lane width > vehicle width
    double laneWidth = world->GetLaneWidth(Route{roadId}, roadId, laneId, distanceFromStart, 0);
    if (vehicleWidth > laneWidth + std::abs(offset))
    {
        loggingCallback("Invalid offset. Lane width < vehicle width: " + std::to_string(laneId) + ". Distance from start: " +
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
        loggingCallback("Invalid offset. Vehicle not inside allowed range: " + std::to_string(laneId) + ". Invalid offset: " +
                 std::to_string(offset));
        return false;
    }

    //Is vehicle partly on invalid lane
    bool isOffsetToLeftLane = (offset >= 0);
    int otherLaneId = isOffsetToLeftLane ? (laneId + 1) : (laneId - 1);

    if (!world->IsSValidOnLane(roadId, otherLaneId, distanceFromStart)) //other lane is invalid
    {
        loggingCallback("Invalid offset. Other lane is invalid: " + std::to_string(laneId) + ". Invalid offset: " + std::to_string(
                     offset));
        return false;
    }

    return true;
}

bool WorldAnalyzer::NewAgentIntersectsWithExistingAgent(const RoadId& roadId,
                                                        const LaneId laneId,
                                                        const SPosition sPosition,
                                                        const double offset,
                                                        const VehicleModelParameters& vehicleModelParameters) const
{
    Position pos = world->LaneCoord2WorldCoord(sPosition, offset, roadId, laneId);

    return world->IntersectsWithAgent(pos.xPos,
                                      pos.yPos,
                                      pos.yawAngle,
                                      vehicleModelParameters.length,
                                      vehicleModelParameters.width,
                                      vehicleModelParameters.distanceReferencePointToLeadingEdge);
}

bool WorldAnalyzer::SpawnWillCauseCrash(const Route& route,
                                        const RoadId& roadId,
                                        const LaneId laneId,
                                        const SPosition sPosition,
                                        const double agentFrontLength,
                                        const double agentRearLength,
                                        const double velocity,
                                        const Direction direction) const
{
    const auto searchForward = (direction == Direction::FORWARD);
    const auto nextObjectsInLane = world->GetObjectsInRange(route,
                                                            roadId,
                                                            laneId,
                                                            sPosition,
                                                            searchForward ? 0 : std::numeric_limits<double>::infinity(),
                                                            searchForward ? std::numeric_limits<double>::infinity() : 0);
    const WorldObjectInterface* opponent = nullptr;
    if (!nextObjectsInLane.empty())
    {
        opponent = searchForward ? nextObjectsInLane.front() : nextObjectsInLane.back();
    }

    if (!opponent)
    {
        return false;
    }

    double velocityOfRearObject;
    double accelerationOfRearObject;

    double velocityOfFrontObject;
    double accelerationOfFrontObject;

    double spaceBetweenObjects;

    if (searchForward)
    {
        velocityOfRearObject = velocity;
        accelerationOfRearObject = ASSUMED_BRAKING_ACCELERATION;
        spaceBetweenObjects = opponent->GetDistanceToStartOfRoad(MeasurementPoint::Rear) - (sPosition + agentFrontLength);
        velocityOfFrontObject = opponent->GetVelocity();
        accelerationOfFrontObject = ASSUMED_FRONT_AGENT_ACCELERATION;
    }
    else
    {
        velocityOfRearObject = opponent->GetVelocity();
        accelerationOfRearObject = ASSUMED_BRAKING_ACCELERATION;
        spaceBetweenObjects = (sPosition - agentRearLength) - opponent->GetDistanceToStartOfRoad(MeasurementPoint::Front);
        velocityOfFrontObject = velocity;
        accelerationOfFrontObject = ASSUMED_FRONT_AGENT_ACCELERATION;
    }

    return TrafficHelperFunctions::WillCrash(spaceBetweenObjects,
                                             velocityOfRearObject,
                                             accelerationOfRearObject,
                                             velocityOfFrontObject,
                                             accelerationOfFrontObject,
                                             ASSUMED_TTB);
}
