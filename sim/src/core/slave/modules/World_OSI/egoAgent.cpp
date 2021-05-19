/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "egoAgent.h"

const AgentInterface* EgoAgent::GetAgent() const
{
    return agent;
}

void EgoAgent::SetRoadGraph(const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target)
{
    graphValid = true;
    this->roadGraph = roadGraph;
    this->current = current;
    alternatives.clear();
    for (auto [vertex, verticesEnd] = vertices(roadGraph); vertex != verticesEnd; ++vertex)
    {
        if (out_degree(*vertex, roadGraph) == 0) //The alternatives are the leaves of the graph
        {
            alternatives.push_back(*vertex);
        }
    }
    SetWayToTarget(target);
    Update();
}

void EgoAgent::Update()
{
    UpdatePositionInGraph();
    if (graphValid)
    {
        //buffer mainLocatePosition to prevent multiple map lookups (yields better performance)
        mainLocatePosition = agent->GetObjectPosition().mainLocatePoint.at(GetRoadId());
    }
}

void EgoAgent::UpdatePositionInGraph()
{
    if (!graphValid)
    {
        return;
    }
    auto roadIds = GetAgent()->GetRoads(MeasurementPoint::Front);
    if (std::find(roadIds.cbegin(), roadIds.cend(), GetRoadId()) == roadIds.end())
    {
        if (rootOfWayToTargetGraph > 0)
        {
            rootOfWayToTargetGraph--;
            auto routeElement = get(RouteElement(), wayToTarget, rootOfWayToTargetGraph);
            if (std::find(roadIds.cbegin(), roadIds.cend(), routeElement.roadId) == roadIds.end())
            {
                graphValid = false;
                return;
            }
            auto [successorBegin, successorsEnd] = adjacent_vertices(current, roadGraph);
                    auto successor = std::find_if(successorBegin, successorsEnd, [&](const auto& vertex){return get(RouteElement(), roadGraph, vertex) == routeElement;});
            if (successor == successorsEnd)
            {
                graphValid = false;
                return;
            }
            current = *successor;
        }
        else
        {
            graphValid = false;
        }
    }
}

bool EgoAgent::HasValidRoute() const
{
    return graphValid;
}

void EgoAgent::SetNewTarget(size_t alternativeIndex)
{
    SetWayToTarget(alternatives[alternativeIndex]);
}

const std::string& EgoAgent::GetRoadId() const
{
    return get(RouteElement(), roadGraph, current).roadId;
}

double EgoAgent::GetDistanceToEndOfLane(double range, int relativeLane) const
{
    return world->GetDistanceToEndOfLane(wayToTarget,
                                         rootOfWayToTargetGraph,
                                         GetLaneIdFromRelative(relativeLane),
                                         GetMainLocatePosition().roadPosition.s,
                                         range).at(0);
}

double EgoAgent::GetDistanceToEndOfLane(double range, int relativeLane, const LaneTypes& acceptableLaneTypes) const
{
    return world->GetDistanceToEndOfLane(wayToTarget,
                                         rootOfWayToTargetGraph,
                                         GetLaneIdFromRelative(relativeLane),
                                         GetMainLocatePosition().roadPosition.s,
                                         range, acceptableLaneTypes).at(0);
}

RelativeWorldView::Lanes EgoAgent::GetRelativeLanes(double range, int relativeLane) const
{
    return world->GetRelativeLanes(wayToTarget,
                                   rootOfWayToTargetGraph,
                                   GetLaneIdFromRelative(relativeLane),
                                   GetMainLocatePosition().roadPosition.s,
                                   range).at(0);
}

std::vector<const WorldObjectInterface*> EgoAgent::GetObjectsInRange(double backwardRange, double forwardRange, int relativeLane) const
{
    auto objectsInRange = world->GetObjectsInRange(wayToTarget,
                                                   rootOfWayToTargetGraph,
                                                   GetLaneIdFromRelative(relativeLane),
                                                   GetMainLocatePosition().roadPosition.s,
                                                   backwardRange,
                                                   forwardRange).at(0);

    auto self = std::find(objectsInRange.cbegin(), objectsInRange.cend(), GetAgent());

    if (self != objectsInRange.cend())
    {
        objectsInRange.erase(self);
    }

    return objectsInRange;
}

std::vector<const AgentInterface*> EgoAgent::GetAgentsInRange(double backwardRange, double forwardRange, int relativeLane) const
{
    auto agentsInRange =  world->GetAgentsInRange(wayToTarget,
                                                  rootOfWayToTargetGraph,
                                                  GetLaneIdFromRelative(relativeLane),
                                                  GetMainLocatePosition().roadPosition.s,
                                                  backwardRange,
                                                  forwardRange).at(0);

    auto self = std::find(agentsInRange.cbegin(), agentsInRange.cend(), GetAgent());

    if (self != agentsInRange.cend())
    {
        agentsInRange.erase(self);
    }

    return agentsInRange;
}

std::vector<CommonTrafficSign::Entity> EgoAgent::GetTrafficSignsInRange(double range, int relativeLane) const
{
    return world->GetTrafficSignsInRange(wayToTarget,
                                         rootOfWayToTargetGraph,
                                         GetLaneIdFromRelative(relativeLane),
                                         GetMainLocatePosition().roadPosition.s,
                                         range).at(0);
}

std::vector<CommonTrafficSign::Entity> EgoAgent::GetRoadMarkingsInRange(double range, int relativeLane) const
{
    return world->GetRoadMarkingsInRange(wayToTarget,
                                         rootOfWayToTargetGraph,
                                         GetLaneIdFromRelative(relativeLane),
                                         GetMainLocatePosition().roadPosition.s,
                                         range).at(0);
}

std::vector<LaneMarking::Entity> EgoAgent::GetLaneMarkingsInRange(double range, Side side, int relativeLane) const
{
    return world->GetLaneMarkings(wayToTarget,
                                  rootOfWayToTargetGraph,
                                  GetLaneIdFromRelative(relativeLane),
                                  GetMainLocatePosition().roadPosition.s,
                                  range,
                                  side).at(0);
}

LongitudinalDistance EgoAgent::GetDistanceToObject(const WorldObjectInterface* otherObject) const
{
    if (!otherObject)
    {
        return {};
    }

    const auto& objectPos = agent->GetObjectPosition();
    const auto referencePoint = GetReferencePointPosition();
    const auto& otherObjectPos = otherObject->GetObjectPosition();
    const auto distance = world->GetDistanceBetweenObjects(wayToTarget, rootOfWayToTargetGraph,
                                                           objectPos, referencePoint ? std::optional<double>{referencePoint->roadPosition.s} : std::nullopt,
                                                           otherObjectPos).at(0);

    return distance;
}

Obstruction EgoAgent::GetObstruction(const WorldObjectInterface* otherObject) const
{
    const auto boundingBox = otherObject->GetBoundingBox2D();
    std::vector<Common::Vector2d> objectCorners;
    for (const auto& point : boundingBox.outer())
    {
        objectCorners.emplace_back(bg::get<0>(point), bg::get<1>(point));
    }
    objectCorners.pop_back(); //Boost polygon contains first point also as last point
    double mainLocatorX = otherObject->GetPositionX() + std::cos(otherObject->GetYaw()) * otherObject->GetDistanceReferencePointToLeadingEdge();
    double mainLocatorY = otherObject->GetPositionY() + std::sin(otherObject->GetYaw()) * otherObject->GetDistanceReferencePointToLeadingEdge();
    return world->GetObstruction(wayToTarget,
                                 rootOfWayToTargetGraph,
                                 agent->GetObjectPosition().mainLocatePoint.at(GetRoadId()),
                                 otherObject->GetObjectPosition(),
                                 objectCorners,
                                 {mainLocatorX, mainLocatorY}).at(0);
}

double EgoAgent::GetRelativeYaw() const
{
    if (get(RouteElement(), roadGraph, current).inOdDirection)
    {
        return GetMainLocatePosition().roadPosition.hdg;
    }
    else
    {
        return std::fmod(GetMainLocatePosition().roadPosition.hdg + 2 * M_PI, 2 * M_PI) - M_PI;
    }
}

double EgoAgent::GetPositionLateral() const
{
    return get(RouteElement(), roadGraph, current).inOdDirection ? GetMainLocatePosition().roadPosition.t : -GetMainLocatePosition().roadPosition.t;
}

double EgoAgent::GetLaneRemainder(Side side) const
{
    const auto& roadId = GetRoadId();
    return side == Side::Left ? agent->GetObjectPosition().touchedRoads.at(roadId).remainder.left
                              : agent->GetObjectPosition().touchedRoads.at(roadId).remainder.right;
}

double EgoAgent::GetLaneWidth(int relativeLane) const
{
    return world->GetLaneWidth(GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().roadPosition.s);
}

std::optional<double> EgoAgent::GetLaneWidth(double distance, int relativeLane) const
{
    return world->GetLaneWidth(wayToTarget, rootOfWayToTargetGraph, GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().roadPosition.s, distance).at(0);
}

double EgoAgent::GetLaneCurvature(int relativeLane) const
{
    return world->GetLaneCurvature(GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().roadPosition.s);
}

std::optional<double> EgoAgent::GetLaneCurvature(double distance, int relativeLane) const
{
    return world->GetLaneCurvature(wayToTarget, rootOfWayToTargetGraph, GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().roadPosition.s, distance).at(0);
}

double EgoAgent::GetLaneDirection(int relativeLane) const
{
    return world->GetLaneWidth(GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().roadPosition.s);
}

std::optional<double> EgoAgent::GetLaneDirection(double distance, int relativeLane) const
{
    return world->GetLaneWidth(wayToTarget, rootOfWayToTargetGraph, GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().roadPosition.s, distance).at(0);
}

const GlobalRoadPosition& EgoAgent::GetMainLocatePosition() const
{
    return mainLocatePosition;
}

std::optional<GlobalRoadPosition> EgoAgent::GetReferencePointPosition() const
{
    auto referencePoint = agent->GetObjectPosition().referencePoint;
    auto referencePointPosition = referencePoint.find(GetRoadId());
    if (referencePointPosition != referencePoint.end())
    {
        return referencePointPosition->second;
    }
    size_t steps = 1;
    while(auto routeElement = GetPreviousRoad(steps))
    {
        referencePointPosition = referencePoint.find(routeElement.value().roadId);
        if (referencePointPosition != referencePoint.end())
        {
            return referencePointPosition->second;
        }
        steps++;
    }
    return std::nullopt;
}

std::optional<RoadGraphVertex> EgoAgent::GetReferencePointVertex() const
{
    auto referencePoint = agent->GetObjectPosition().referencePoint;
    for (size_t steps = 0; rootOfWayToTargetGraph + steps < num_vertices(wayToTarget); ++steps)
    {
        auto routeElement = get(RouteElement(), wayToTarget, rootOfWayToTargetGraph + steps);
        auto referencePointPosition = referencePoint.find(routeElement.roadId);
        if (referencePointPosition != referencePoint.end())
        {
            return rootOfWayToTargetGraph + steps;
        }
    }
    return std::nullopt;
}
int EgoAgent::GetLaneIdFromRelative(int relativeLaneId) const
{
    const auto& routeElement = get(RouteElement(), roadGraph, current);
    const auto mainLaneId = GetMainLocatePosition().laneId;
    if (routeElement.inOdDirection)
    {
        return  mainLaneId + relativeLaneId + (relativeLaneId >= -mainLaneId ? 1 : 0);
    }
    else
    {
        return  mainLaneId - relativeLaneId + (relativeLaneId <= -mainLaneId ? -1 : 0);
    }
}

std::optional<RouteElement> EgoAgent::GetPreviousRoad(size_t steps) const
{
    if (rootOfWayToTargetGraph + steps >= num_vertices(wayToTarget))
    {
        return std::nullopt;
    }
    return get(RouteElement(), wayToTarget, rootOfWayToTargetGraph + steps);
}

Position EgoAgent::GetWorldPosition(double sDistance, double tDistance, double yaw) const
{
    auto currentPosition = GetReferencePointPosition().value();
    auto targetVertex = GetReferencePointVertex().value();
    auto roadId = currentPosition.roadId;
    auto laneId = currentPosition.laneId;
    auto inOdDirection = get(RouteElement(), wayToTarget, rootOfWayToTargetGraph).inOdDirection;
    auto targetS = currentPosition.roadPosition.s + (inOdDirection ? sDistance : -sDistance);
    auto targetT = currentPosition.roadPosition.t + (inOdDirection ? tDistance : -tDistance);
    if (laneId < 0)
    {
        targetT -= 0.5 * world->GetLaneWidth(roadId, laneId, currentPosition.roadPosition.s);
        for (int intermediaryLane = -1; intermediaryLane > laneId; --intermediaryLane)
        {
            targetT -= world->GetLaneWidth(roadId, intermediaryLane, currentPosition.roadPosition.s);
        }
    }
    else
    {
        targetT += 0.5 * world->GetLaneWidth(roadId, laneId, currentPosition.roadPosition.s);
        for (int intermediaryLane = 1; intermediaryLane < laneId; ++intermediaryLane)
        {
            targetT += world->GetLaneWidth(roadId, intermediaryLane, currentPosition.roadPosition.s);
        }
    }
    bool finished = false;
    while (!finished)
    {
        if (targetS < 0)
        {
            targetVertex--;
            auto routeElement = get(RouteElement(), wayToTarget, targetVertex);
            if (routeElement.inOdDirection)
            {
                targetS = -targetS;
            }
            else
            {
                targetS += world->GetRoadLength(routeElement.roadId);
            }
            roadId = routeElement.roadId;
        }
        else if (targetS > world->GetRoadLength(roadId))
        {
            targetVertex--;
            auto routeElement = get(RouteElement(), wayToTarget, targetVertex);
            if (routeElement.inOdDirection)
            {
                targetS -= world->GetRoadLength(roadId);
            }
            else
            {
                targetS = world->GetRoadLength(roadId) + world->GetRoadLength(routeElement.roadId) - targetS;
            }
            roadId = routeElement.roadId;
        }
        else
        {
            finished = true;
        }
    }
    if (inOdDirection != get(RouteElement(), wayToTarget, targetVertex).inOdDirection)
    {
        targetT = -targetT;
    }
    if (!get(RouteElement(), wayToTarget, targetVertex).inOdDirection)
    {
        yaw = CommonHelper::SetAngleToValidRange(yaw + M_PI);
    }
    RoadPosition newPosition{targetS, targetT, yaw};
    return world->RoadCoord2WorldCoord(newPosition, roadId);
}

void EgoAgent::SetWayToTarget(RoadGraphVertex targetVertex)
{
    wayToTarget = RoadGraph{};
    RoadGraphVertex wayPoint = targetVertex;
    auto vertex1 = add_vertex(get(RouteElement(), roadGraph, wayPoint), wayToTarget);
    while (wayPoint != current)
    {
        for (auto [edge, edgesEnd] = edges(roadGraph); edge != edgesEnd; ++edge)
        {
            if (target(*edge, roadGraph) == wayPoint)
            {
                wayPoint = source(*edge, roadGraph);
                auto vertex2 = add_vertex(get(RouteElement(), roadGraph, wayPoint), wayToTarget);
                add_edge(vertex2, vertex1, wayToTarget);
                vertex1 = vertex2;
                break;
            }
        }
    }
    rootOfWayToTargetGraph = vertex1;
}

template<>
ExecuteReturn<DistanceToEndOfLane> EgoAgentInterface::executeQuery(DistanceToEndOfLaneParameter param) const
{
    return executeQueryDistanceToEndOfLane(param);
}

template<>
ExecuteReturn<ObjectsInRange> EgoAgentInterface::executeQuery(ObjectsInRangeParameter param) const
{
    return executeQueryObjectsInRange(param);
}

ExecuteReturn<DistanceToEndOfLane> EgoAgent::executeQueryDistanceToEndOfLane(DistanceToEndOfLaneParameter param) const
{
    auto queryResult = world->GetDistanceToEndOfLane(wayToTarget,
                                                     current,
                                                     GetLaneIdFromRelative(param.relativeLane),
                                                     GetMainLocatePosition().roadPosition.s,
                                                     param.range);
    std::vector<DistanceToEndOfLane> results;
    std::transform(alternatives.cbegin(), alternatives.cend(), std::back_inserter(results),
                   [&](const RoadGraphVertex& alternative){return queryResult.at(alternative);});
    return {results};
}

ExecuteReturn<ObjectsInRange> EgoAgent::executeQueryObjectsInRange(ObjectsInRangeParameter param) const
{
    auto queryResult = world->GetObjectsInRange(wayToTarget,
                                                current,
                                                GetLaneIdFromRelative(param.relativeLane),
                                                GetMainLocatePosition().roadPosition.s,
                                                param.backwardRange,
                                                param.forwardRange);
    std::vector<ObjectsInRange> results;
    std::transform(alternatives.cbegin(), alternatives.cend(), std::back_inserter(results),
                   [&](const RoadGraphVertex& alternative){return queryResult.at(alternative);});
    return {results};
}

RelativeWorldView::Junctions EgoAgent::GetRelativeJunctions(double range) const
{
    return world->GetRelativeJunctions(wayToTarget,
                                       rootOfWayToTargetGraph,
                                       GetMainLocatePosition().roadPosition.s,
                                       range).at(0);
}
