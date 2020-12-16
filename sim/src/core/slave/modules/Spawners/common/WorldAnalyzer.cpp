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
#include "include/agentInterface.h"
#include "common/RoutePlanning/RouteCalculation.h"
#include "common/commonTools.h"

namespace
{
    static constexpr double EPSILON = 0.001;
    static constexpr double MINIMUM_SEPARATION_BUFFER = 5.0;
    static constexpr double TTC_THRESHHOLD = 2.0;
    static constexpr double TTC_ENDOFLANE = 4.0;
    static constexpr double ASSUMED_TTB = 1.0;
    static constexpr double ASSUMED_BRAKING_ACCELERATION = -6.0;
    static constexpr double ASSUMED_FRONT_AGENT_ACCELERATION = -10;
    static constexpr size_t MAX_ROADGRAPH_DEPTH = 10; //! Limits search depths in case of cyclic network

    static inline double GetSeparation(const double gapInSeconds,
                                       const double velocity,
                                       const double consideredCarLengths)
    {
        const auto minimumBuffer = MINIMUM_SEPARATION_BUFFER + consideredCarLengths;

        return std::max((gapInSeconds * velocity) + consideredCarLengths, minimumBuffer);
    }
}

bool WorldAnalyzer::ValidateRoadIdInDirection(const RoadId& roadId,
                                              const LaneId laneId,
                                              const double distanceOnLane,
                                              const LaneTypes& validLaneTypes) const
{
    if (!world->IsDirectionalRoadExisting(roadId, laneId < 0))
    {
        loggingCallback("Invalid spawn information. RoadId: " + roadId + " not existing for laneId:" + std::to_string(laneId) + " and will be ignored.");
        return false;
    }

    if (!world->IsLaneTypeValid(roadId, laneId, distanceOnLane, validLaneTypes))
    {
        return false;
    }

    return true;
}

Route WorldAnalyzer::SampleRoute(const std::string &roadId,
                                 const int laneId,
                                 StochasticsInterface* stochastics) const
{
    Route route;
    auto [roadGraph , root] = world->GetRoadGraph({roadId, laneId < 0}, MAX_ROADGRAPH_DEPTH);
            
    route.roadGraph = roadGraph;
    route.root = root;
    route.target = RouteCalculation::SampleRoute(roadGraph,
                                                 root,
                                                 world->GetEdgeWeights(roadGraph),
                                                 *stochastics);

    return route;
}

LaneSpawningRanges WorldAnalyzer::GetValidLaneSpawningRanges(const RoadId& roadId,
                                                             const SPosition sStart,
                                                             const SPosition sEnd,
                                                             const std::vector<int>& validLaneIds,
                                                             const LaneTypes &supportedLaneTypes) const
{
    LaneSpawningRanges validLaneSpawningRanges {};

    auto laneSections = world->GetLaneSections(roadId);

    // By reversing the furthest section is the first element. This allows to spawn from end of the road to the start.
    std::reverse(laneSections.begin(), laneSections.end());

    for (const auto& laneSection : laneSections)
    {
        // Check if LaneSection is within defined spawning range
        if (laneSection.startS > sEnd || laneSection.endS < sStart)
        {
            continue;
        }

        const auto startOfRange = std::max(sStart, laneSection.startS);
        const auto endOfRange = std::min(sEnd, laneSection.endS);

        for (const auto& laneId : laneSection.laneIds)
        {
            if (std::find(validLaneIds.begin(), validLaneIds.end(), laneId) == validLaneIds.end())
            {
                continue;
            }

            if(!ValidateRoadIdInDirection(roadId, laneId, startOfRange, supportedLaneTypes))
            {
                continue;
            }

            const auto [roadGraph, vertex] = world->GetRoadGraph(RouteElement{roadId, laneId < 0}, 1);

            const auto agentsInLane = world->GetAgentsInRange(roadGraph,
                                                              vertex,
                                                              laneId,
                                                              startOfRange,
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
                validLaneSpawningRanges.emplace_back(laneId, startOfRange, endOfRange);
            }
            else
            {
                const auto frontAgent = scenarioAgents.back();
                const auto frontAgentDistance = frontAgent->GetObjectPosition().touchedRoads.at(roadId).sEnd + EPSILON;

                const auto rearAgent = scenarioAgents.front();
                const auto rearAgentDistance = rearAgent->GetObjectPosition().touchedRoads.at(roadId).sStart - EPSILON;

                const auto validRangesBasedOnScenario = GetValidSpawningInformationForRange(laneId,
                                                                                            startOfRange,
                                                           endOfRange,
                                                           rearAgentDistance,
                                                           frontAgentDistance);

                validLaneSpawningRanges.insert(validLaneSpawningRanges.end(), validRangesBasedOnScenario.begin(), validRangesBasedOnScenario.end());
            }
        }
    }

    return validLaneSpawningRanges;
}

std::optional<double> WorldAnalyzer::GetNextSpawnPosition(const RoadId& roadId,
                                                          const LaneId laneId,
                                                          const Range& bounds,
                                                          const double agentFrontLength,
                                                          const double agentRearLength,
                                                          const double intendedVelocity,
                                                          const double gapInSeconds,
                                                          const Route &route,
                                                          const LaneTypes& supportedLaneTypes) const
{
    double spawnDistance;

    const auto maxSearchPosition = bounds.second + intendedVelocity * gapInSeconds;

    const auto downstreamObjects = world->GetAgentsInRange(route.roadGraph,
                                                           route.root,
                                                           laneId,
                                                           bounds.first,
                                                           0,
                                                           maxSearchPosition).at(route.target);
    const AgentInterface* firstDownstreamObject = nullptr;
    if (!downstreamObjects.empty())
    {
        firstDownstreamObject = downstreamObjects.front();
    }

    if (!firstDownstreamObject || firstDownstreamObject->GetDistanceToStartOfRoad(MeasurementPoint::Rear, roadId) > maxSearchPosition)
    {
        spawnDistance = bounds.second - agentFrontLength;

        const auto distanceToEndOfDrivingLane = world->GetDistanceToEndOfLane(route.roadGraph,
                                                                              route.root,
                                                                              laneId,
                                                                              bounds.second - EPSILON,
                                                                              maxSearchPosition,
                                                                              supportedLaneTypes).at(route.target);

        const auto minDistanceToEndOfLane = intendedVelocity * TTC_ENDOFLANE;

        if (distanceToEndOfDrivingLane < minDistanceToEndOfLane)
        {
            const auto difference = minDistanceToEndOfLane - distanceToEndOfDrivingLane;
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
                                                              const double intendedVelocity,
                                                              const Route &route) const
{
    const auto intendedVehicleFrontPosition = intendedSpawnPosition + agentFrontLength;
    const auto maxSearchDistance = intendedVehicleFrontPosition + (intendedVelocity * TTC_THRESHHOLD);

    const auto nextObjectsInLane =  world->GetAgentsInRange(route.roadGraph,
                                                            route.root,
                                                            laneId,
                                                            intendedSpawnPosition - agentRearLength,
                                                            0,
                                                            maxSearchDistance).at(route.target);
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

bool WorldAnalyzer::ValidMinimumSpawningDistanceToObjectInFront(const LaneId laneId,
                                                                const SPosition sPosition,
                                                                const Route &route,
                                                                const VehicleModelParameters& vehicleModelParameters) const
{
    const double distanceReferencePointToFrontAxle = vehicleModelParameters.boundingBoxDimensions.length * 0.5 + vehicleModelParameters.boundingBoxCenter.x;
    const double rearLength = vehicleModelParameters.boundingBoxDimensions.length * 0.5 - vehicleModelParameters.boundingBoxCenter.x;

    const auto agentsInRangeResult = world->GetAgentsInRange(route.roadGraph,
                                                             route.root,
                                                             laneId,
                                                             sPosition,
                                                             rearLength,
                                                             distanceReferencePointToFrontAxle + MINIMUM_SEPARATION_BUFFER);

    if(!agentsInRangeResult.at(route.target).empty())
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
                                                const Route& route,
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

    if (!IsOffsetValidForLane(roadId, laneId, sPosition, offset, vehicleModelParameters.boundingBoxDimensions.width))
    {
        loggingCallback("Offset is not valid for vehicle on lane: " + std::to_string(laneId) + ". Invalid offset: " + std::to_string(
                     offset));

        return false;
    }

    if (!ValidMinimumSpawningDistanceToObjectInFront(laneId, sPosition, route, vehicleModelParameters))
    {
        loggingCallback("New Agent does not fullfill the required minimum distance on lane: " + std::to_string(laneId) + ".");
        return false;
    }

    return true;
}

LaneSpawningRanges WorldAnalyzer::GetValidSpawningInformationForRange(const int laneId,
                                                                      const double sStart,
                                                                      const double sEnd,
                                                                      const double firstScenarioAgentSPosition,
                                                                      const double lastScenarioAgentSPosition)
{
    // if the stream segment this SpawnPoint is responsible for is surrounded by scenario agents,
    // no valid spawn locations exist here
    if (firstScenarioAgentSPosition < sStart
        && lastScenarioAgentSPosition > sEnd)
    {
        return {};
    }
    // if the stream segment this SpawnPoint is responsible for has one scenario agent within
    // its range and one without, only a portion of the specified range is valid
    LaneSpawningRanges validLaneSpawningInformation {};
    if ((firstScenarioAgentSPosition < sStart && lastScenarioAgentSPosition < sStart)
     || (firstScenarioAgentSPosition > sEnd && lastScenarioAgentSPosition > sEnd))
    {
        validLaneSpawningInformation.emplace_back(laneId, sStart, sEnd);
        return validLaneSpawningInformation;
    }
    // if the first scenario s position is within our range, exclude everything after that (except as below)
    if (firstScenarioAgentSPosition > sStart
     && firstScenarioAgentSPosition < sEnd)
    {
        validLaneSpawningInformation.emplace_back(laneId, sStart, firstScenarioAgentSPosition);
    }

    // if the last scenario s position is within our range, exclude everything before that (except as above)
    if (lastScenarioAgentSPosition > sStart
     && lastScenarioAgentSPosition < sEnd)
    {
        validLaneSpawningInformation.emplace_back(laneId, lastScenarioAgentSPosition, sEnd);
    }

    return validLaneSpawningInformation;
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

    const double distanceReferencePointToLeadingEdge = vehicleModelParameters.boundingBoxDimensions.length * 0.5 + vehicleModelParameters.boundingBoxCenter.x;
    return world->IntersectsWithAgent(pos.xPos,
                                      pos.yPos,
                                      pos.yawAngle,
                                      vehicleModelParameters.boundingBoxDimensions.length,
                                      vehicleModelParameters.boundingBoxDimensions.width,
                                      distanceReferencePointToLeadingEdge);
}

bool WorldAnalyzer::SpawnWillCauseCrash(const RoadId& roadId,
                                        const LaneId laneId,
                                        const SPosition sPosition,
                                        const double agentFrontLength,
                                        const double agentRearLength,
                                        const double velocity,
                                        const Direction direction,
                                        const Route &route) const
{
    const auto searchForward = (direction == Direction::FORWARD);
    const auto nextObjectsInLane = world->GetObjectsInRange(route.roadGraph,
                                                            route.root,
                                                            laneId,
                                                            sPosition,
                                                            searchForward ? 0 : std::numeric_limits<double>::infinity(),
                                                            searchForward ? std::numeric_limits<double>::infinity() : 0).at(route.target);
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
