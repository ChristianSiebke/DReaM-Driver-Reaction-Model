/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
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
    static constexpr double MINIMUM_SEPARATION_BUFFER = 5.0;
    static constexpr double MIN_DISTANCE_TO_END_OF_LANE = 50.0;

    static constexpr double TTC_THRESHHOLD = 2.0;
    static constexpr double ASSUMED_TTB = 1.0;
    static constexpr double ASSUMED_BRAKING_ACCELERATION = -6.0;
    static constexpr double ASSUMED_FRONT_AGENT_ACCELERATION = -10;

    static inline double GetSeparation(const double gapInSeconds,
                                       const double velocity,
                                       const double consideredCarLengths)
    {
        const auto minimumBuffer = MINIMUM_SEPARATION_BUFFER + consideredCarLengths;

        return std::max((gapInSeconds * velocity) + consideredCarLengths, minimumBuffer);
    }

    static inline std::pair<RoadGraph, RoadGraphVertex> GetSingleVertexRoadGraph(const RouteElement& routeElement)
    {
        RoadGraph roadGraph;
        auto vertex = add_vertex(routeElement, roadGraph);
        return {roadGraph, vertex};
    }
}

std::optional<ValidLaneSpawningRanges> WorldAnalyzer::GetValidLaneSpawningRanges(const RoadId& roadId,
                                                                                 const LaneId laneId,
                                                                                 const SPosition sStart,
                                                                                 const SPosition sEnd) const
{
    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, laneId < 0});
    const auto agentsInLane = world->GetAgentsInRange(roadGraph,
                                                      vertex,
                                                      laneId,
                                                      sStart,
                                                      std::numeric_limits<double>::infinity(),
                                                      std::numeric_limits<double>::infinity()).at(vertex);

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
        const auto frontAgentDistance = frontAgent->GetObjectPosition().referencePoint.at(roadId).roadPosition.s + frontAgent->GetVehicleModelParameters().distanceReferencePointToLeadingEdge;

        const auto rearAgent = scenarioAgents.front();
        const auto vehicleModelParameters = rearAgent->GetVehicleModelParameters();
        const auto distanceReferencePointToRearEdge = vehicleModelParameters.length - vehicleModelParameters.distanceReferencePointToLeadingEdge;
        const auto rearAgentDistance = rearAgent->GetObjectPosition().referencePoint.at(roadId).roadPosition.s - distanceReferencePointToRearEdge;

        return GetValidSpawningInformationForRange(sStart,
                                                   sEnd,
                                                   rearAgentDistance,
                                                   frontAgentDistance);
    }
}

std::optional<double> WorldAnalyzer::GetNextSpawnPosition(const RoadId& roadId,
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
    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, laneId < 0});
    const auto downstreamObjects = world->GetAgentsInRange(roadGraph,
                                                           vertex,
                                                           laneId,
                                                           bounds.first,
                                                           direction == Direction::FORWARD ? 0 : maxSearchPosition,
                                                           direction == Direction::FORWARD ? maxSearchPosition : 0).at(vertex);
    const AgentInterface* firstDownstreamObject = nullptr;
    if (!downstreamObjects.empty())
    {
        firstDownstreamObject = direction == Direction::FORWARD ? downstreamObjects.front() : downstreamObjects.back();
    }
  if (!firstDownstreamObject || firstDownstreamObject->GetDistanceToStartOfRoad(MeasurementPoint::Rear, roadId) > maxSearchPosition)
  {
      const auto drivingLaneLength = world->GetDistanceToEndOfLane(roadGraph,
                                                                   vertex,
                                                                   laneId,
                                                                   0,
                                                                   std::numeric_limits<double>::max(),
                                                                   {LaneType::Driving}).at(vertex);
      spawnDistance = std::min(bounds.second, drivingLaneLength) - agentFrontLength;

      const auto distanceToEndOfDrivingLane = world->GetDistanceToEndOfLane(roadGraph,
                                                                            vertex,
                                                                            laneId,
                                                                            spawnDistance + agentFrontLength,
                                                                            bounds.second,
                                                                            {LaneType::Driving}).at(vertex);

      if (distanceToEndOfDrivingLane < MIN_DISTANCE_TO_END_OF_LANE)
      {
          const auto difference = MIN_DISTANCE_TO_END_OF_LANE - distanceToEndOfDrivingLane;
          spawnDistance -= difference;
      }
  }
  else
  {
      const auto frontCarRearLength = firstDownstreamObject->GetLength()
                                    - firstDownstreamObject->GetDistanceReferencePointToLeadingEdge();
      const auto separation = GetSeparation(gapInSeconds,
                                            intendedVelocity,
                                            frontCarRearLength + agentFrontLength);

      const auto frontCarPositionOnRoad = firstDownstreamObject->GetDistanceToStartOfRoad(MeasurementPoint::Rear, roadId);
      spawnDistance = frontCarPositionOnRoad - separation;
  }

    if (spawnDistance - agentRearLength < bounds.first)
    {
        return {};
    }

    return spawnDistance;
}

double WorldAnalyzer::CalculateSpawnVelocityToPreventCrashing(const RoadId& roadId,
                                                              const LaneId laneId,
                                                              const double intendedSpawnPosition,
                                                              const double agentFrontLength,
                                                              const double agentRearLength,
                                                              const double intendedVelocity) const
{
    const auto intendedVehicleFrontPosition = intendedSpawnPosition + agentFrontLength;
    const auto maxSearchDistance = intendedVehicleFrontPosition + (intendedVelocity * TTC_THRESHHOLD);

    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, laneId < 0});
    const auto nextObjectsInLane =  world->GetAgentsInRange(roadGraph,
                                                            vertex,
                                                            laneId,
                                                            intendedSpawnPosition - agentRearLength,
                                                            0,
                                                            maxSearchDistance).at(vertex);
    const AgentInterface* opponent = nullptr;
    if (!nextObjectsInLane.empty())
    {
        opponent = nextObjectsInLane.front();

        const auto &relativeVelocity = intendedVelocity - opponent->GetVelocity();
        if (relativeVelocity <= 0.0)
        {
            return intendedVelocity;
        }

        const auto& relativeDistance = opponent->GetDistanceToStartOfRoad(MeasurementPoint::Rear, roadId) - intendedVehicleFrontPosition;

        if (relativeDistance / relativeVelocity < TTC_THRESHHOLD)
        {
            return opponent->GetVelocity() + (relativeDistance / TTC_THRESHHOLD);
        }
    }

    return intendedVelocity;
}

bool WorldAnalyzer::ValidMinimumSpawningDistanceToObjectInFront(const RoadId& roadId,
                                                                const LaneId laneId,
                                                                const SPosition sPosition,
                                                                const VehicleModelParameters& vehicleModelParameters) const
{
    const double rearLength = vehicleModelParameters.length - vehicleModelParameters.distanceReferencePointToFrontAxle;

    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, laneId < 0});
    const auto agentsInRangeResult = world->GetAgentsInRange(roadGraph, vertex, laneId, sPosition, rearLength, vehicleModelParameters.distanceReferencePointToFrontAxle + MINIMUM_SEPARATION_BUFFER);

    if(!agentsInRangeResult.at(vertex).empty())
    {
        loggingCallback("Minimum distance required to previous agent not valid on lane: " + std::to_string(laneId) + ".");
        return false;
    }

    return true;
}

bool WorldAnalyzer::AreSpawningCoordinatesValid(const RoadId& roadId,
                                                const LaneId laneId,
                                                const SPosition sPosition,
                                                const double offset,
                                                const VehicleModelParameters& vehicleModelParameters) const
{
    if (laneId >= 0) //HOTFIX: Spawning on left lanes is currently not supported
    {
        return false;
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

    if (!ValidMinimumSpawningDistanceToObjectInFront(roadId, laneId, sPosition, vehicleModelParameters))
    {
        loggingCallback("New Agent does not fullfill the required minimum distance on lane: " + std::to_string(laneId) + ".");
        return false;
    }

    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, laneId < 0});
    if (world->GetDistanceToEndOfLane(roadGraph, vertex, laneId, sPosition, INFINITY, {LaneType::Driving}).at(vertex) < MIN_DISTANCE_TO_END_OF_LANE)
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
    double laneWidth = world->GetLaneWidth(roadId, laneId, distanceFromStart);
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

bool WorldAnalyzer::SpawnWillCauseCrash(const RoadId& roadId,
                                        const LaneId laneId,
                                        const SPosition sPosition,
                                        const double agentFrontLength,
                                        const double agentRearLength,
                                        const double velocity,
                                        const Direction direction) const
{
    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, laneId < 0});
    const auto searchForward = (direction == Direction::FORWARD);
    const auto nextObjectsInLane = world->GetObjectsInRange(roadGraph,
                                                            vertex,
                                                            laneId,
                                                            sPosition,
                                                            searchForward ? 0 : std::numeric_limits<double>::infinity(),
                                                            searchForward ? std::numeric_limits<double>::infinity() : 0).at(vertex);
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
        spaceBetweenObjects = opponent->GetDistanceToStartOfRoad(MeasurementPoint::Rear, roadId) - (sPosition + agentFrontLength);
        velocityOfFrontObject = opponent->GetVelocity();
        accelerationOfFrontObject = ASSUMED_FRONT_AGENT_ACCELERATION;
    }
    else
    {
        velocityOfRearObject = opponent->GetVelocity();
        accelerationOfRearObject = ASSUMED_BRAKING_ACCELERATION;
        spaceBetweenObjects = (sPosition - agentRearLength) - opponent->GetDistanceToStartOfRoad(MeasurementPoint::Front, roadId);
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

size_t WorldAnalyzer::GetRightLaneCount(const RoadId& roadId, const LaneId& laneId, const double sPosition) const
{
    size_t rightLaneCount{0};
    RoadGraph roadGraph;
    const auto start = add_vertex(RouteElement{roadId, laneId < 0}, roadGraph);
    const auto relativeLanes = world->GetRelativeLanes(roadGraph, start, laneId, sPosition, 0).at(start);
    for (const auto& lane : relativeLanes.front().lanes)
    {
        if (lane.relativeId < 0 && lane.type == LaneType::Driving)
        {
            ++rightLaneCount;
        }
    }
    return rightLaneCount;
}
