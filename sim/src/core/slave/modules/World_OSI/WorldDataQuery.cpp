/*******************************************************************************
* Copyright (c) 2018, 2019, 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "WorldDataQuery.h"

static const OWL::Interfaces::Lane* GetLaneOnRoad(const OWL::Interfaces::Road* road, const std::vector<OWL::Id> laneIds)
{
    for (const auto& section : road->GetSections())
    {
        for (const auto& lane : section->GetLanes())
        {
            if (std::count(laneIds.cbegin(), laneIds.cend(), lane->GetId()) > 0)
            {
                return lane;
            }
        }
    }
    return nullptr;
}

WorldDataQuery::WorldDataQuery(const OWL::Interfaces::WorldData &worldData) : worldData{worldData}
{}

template<typename T>
Stream<T> Stream<T>::Reverse() const
{
    std::vector<StreamInfo<T>> newStream;
    double currentS = 0.0;

    for (auto oldStreamInfo = elements.crbegin(); oldStreamInfo != elements.crend(); ++oldStreamInfo)
    {
        StreamInfo<T> streamInfo;
        streamInfo.inStreamDirection = !oldStreamInfo->inStreamDirection;
        streamInfo.element = oldStreamInfo->element;

        auto elementLength = streamInfo.element->GetLength();
        streamInfo.sOffset = currentS + (streamInfo.inStreamDirection ? 0 : elementLength);
        newStream.push_back(streamInfo);

        currentS += elementLength;
    }
    return Stream<T>{std::move(newStream)};
}

template<typename T>
double Stream<T>::GetPositionByElementAndS(const T& element, double sCoordinate) const
{
    for (const auto& elemOnStream : elements)
    {
        if (elemOnStream.element == &element)
        {
            return elemOnStream.GetStreamPosition(sCoordinate - element.GetDistance(OWL::MeasurementPoint::RoadStart));
        }
    }
    return -1.0;
}

template<typename T>
std::pair<double, const T*> Stream<T>::GetElementAndSByPosition(double position) const
{
    for (const auto& element : elements)
    {
        if (element.StartS() <= position && element.EndS() >= position)
        {
            double relativeDistance = element.inStreamDirection ? position - element.sOffset : element.sOffset - position;
            return std::make_pair(relativeDistance, &element());
        }
    }
    return std::make_pair(0.0, nullptr);
}

template<typename T>
bool Stream<T>::Contains(const T& element) const
{
    return std::find_if(elements.cbegin(), elements.cend(),
                        [&element](auto& streamInfo)
                            {return streamInfo.element == &element;})
            != elements.cend();
}

RouteQueryResult<double> WorldDataQuery::GetDistanceToEndOfLane(const LaneMultiStream& laneStream, double initialSearchPosition, double maxSearchLength, const std::vector<LaneType>& requestedLaneTypes) const
{
    return laneStream.Traverse<double, bool>(LaneMultiStream::TraversedFunction<double, bool>{[&](const auto& streamElement, const auto& previousDistance, const auto& laneTypeContinuous)
    {
        if (!laneTypeContinuous || std::find(requestedLaneTypes.cbegin(), requestedLaneTypes.cend(), streamElement().GetLaneType()) == requestedLaneTypes.cend())
        {
            return std::make_tuple<double, bool>(double{previousDistance}, false);
        }
        else if (streamElement.EndS() > initialSearchPosition +  maxSearchLength)
        {
            return std::make_tuple<double, bool>(std::numeric_limits<double>::infinity(), true);
        }
        else
        {
            return std::make_tuple<double, bool>(streamElement.EndS() - initialSearchPosition, true);
        }
    }},
    0.0, true,
    worldData);
}

OWL::CSection* WorldDataQuery::GetSectionByDistance(std::string odRoadId, double distance) const
{
    distance = std::max(0.0, distance);

    auto road = GetRoadByOdId(odRoadId);
    if (!road)
    {
        return nullptr;
    }
    for (auto tmpSection : road->GetSections())
    {
        if (tmpSection->Covers(distance))
        {
            return tmpSection;
        }
    }

    return nullptr;
}

OWL::CRoad* WorldDataQuery::GetRoadByOdId(std::string odRoadId) const
{
    auto road = worldData.GetRoads().find(odRoadId);
    return  road != worldData.GetRoads().cend() ? road->second : nullptr;
}

const OWL::Interfaces::Junction *WorldDataQuery::GetJunctionByOdId(const std::string &odJunctionId) const
{
    auto junction = worldData.GetJunctions().find(odJunctionId);
    return  junction != worldData.GetJunctions().cend() ? junction->second : nullptr;
}

const OWL::Interfaces::Junction *WorldDataQuery::GetJunctionOfConnector(const std::string &connectingRoadId) const
{
    const auto connectingRoad = GetRoadByOdId(connectingRoadId);
    for (auto junction : worldData.GetJunctions())
    {
        const auto& junctionConnections = junction.second->GetConnectingRoads();
        if (std::find(junctionConnections.cbegin(), junctionConnections.cend(), connectingRoad) != junctionConnections.cend())
        {
            return junction.second;
        }
    }
    return nullptr;
}


OWL::CLanes WorldDataQuery::GetLanesOfLaneTypeAtDistance(std::string roadId, double distance, std::list<LaneType> requestedLaneTypes) const
{
    std::list<OWL::CLane*> lanes;
    OWL::CSection* sectionAtDistance = GetSectionByDistance(roadId, distance);

    if (sectionAtDistance)
    {
        for (auto lane : sectionAtDistance->GetLanes())
        {
            if (std::find(requestedLaneTypes.begin(), requestedLaneTypes.end(), lane->GetLaneType()) != requestedLaneTypes.end())
            {
                lanes.push_back(lane);
            }
        }
    }

    return lanes;
}

OWL::CLane& WorldDataQuery::GetLaneByOdId(std::string roadId, OWL::OdId odLaneId, double distance) const
{
    auto section = GetSectionByDistance(roadId, distance);
    if (!section)
    {
        return worldData.GetInvalidLane();
    }

    for (const OWL::Lane* lane : section->GetLanes())
    {
        // if a section covers a point the lanes also do
        if (lane->GetOdId() == odLaneId)
        {
            return *lane;
        }
    }

    return worldData.GetInvalidLane();
}


bool WorldDataQuery::IsSValidOnLane(std::string roadId, OWL::OdId laneId, double distance)
{
    if (distance < 0)
    {
        return false;
    }

    return GetLaneByOdId(roadId, laneId, distance).Exists();
}

RouteQueryResult<std::vector<CommonTrafficSign::Entity>> WorldDataQuery::GetTrafficSignsInRange(LaneMultiStream laneStream, double startDistance, double searchRange) const
{
    const bool backwardsSearch = searchRange < 0;
    const double startPosition = backwardsSearch ? startDistance + searchRange : startDistance;
    const double endPosition = backwardsSearch ? startDistance : startDistance + searchRange;
    return laneStream.Traverse(LaneMultiStream::TraversedFunction<std::vector<CommonTrafficSign::Entity>>{[&](const auto& lane, const auto& previousTrafficSigns)
    {
        std::vector<CommonTrafficSign::Entity> foundTrafficSigns{previousTrafficSigns};

        if (lane.EndS() < startPosition)
        {
            return foundTrafficSigns;
        }
        if (lane.StartS() > endPosition)
        {
            return foundTrafficSigns;
        }
        auto sortedTrafficSigns = lane().GetTrafficSigns();
        sortedTrafficSigns.sort([lane](const auto first, const auto second)
        {return lane.GetStreamPosition(first->GetS()) < lane.GetStreamPosition(second->GetS());});
        for (const auto& trafficSign : sortedTrafficSigns)
        {
            double trafficSignPosition = lane.GetStreamPosition(trafficSign->GetS() - lane().GetDistance(OWL::MeasurementPoint::RoadStart));
            if (startPosition <= trafficSignPosition && trafficSignPosition <= endPosition)
            {
                foundTrafficSigns.push_back(trafficSign->GetSpecification(trafficSignPosition - startDistance));
            }
        }
        return foundTrafficSigns;
    }},
    {},
    worldData);
}

RouteQueryResult<std::vector<CommonTrafficSign::Entity>> WorldDataQuery::GetRoadMarkingsInRange(LaneMultiStream laneStream, double startDistance, double searchRange) const
{
    return laneStream.Traverse(LaneMultiStream::TraversedFunction<std::vector<CommonTrafficSign::Entity>>{[&](const auto& lane, const auto& previousRoadMarkings)
    {
        std::vector<CommonTrafficSign::Entity> foundRoadMarkings{previousRoadMarkings};

        if (lane.EndS() < startDistance)
        {
            return foundRoadMarkings;
        }
        if (lane.StartS() > startDistance + searchRange)
        {
            return foundRoadMarkings;
        }
        auto sortedRoadMarkings = lane().GetRoadMarkings();
        sortedRoadMarkings.sort([lane](const auto first, const auto second)
                                {return lane.GetStreamPosition(first->GetS()) < lane.GetStreamPosition(second->GetS());});

        for (const auto& roadMarking : sortedRoadMarkings)
        {
            double roadMarkingPosition = lane.GetStreamPosition(roadMarking->GetS() - lane().GetDistance(OWL::MeasurementPoint::RoadStart));
            if (startDistance <= roadMarkingPosition && roadMarkingPosition <= startDistance + searchRange)
            {
                foundRoadMarkings.push_back(roadMarking->GetSpecification(roadMarkingPosition - startDistance));
            }
        }
        return foundRoadMarkings;
    }},
    {},
    worldData);
}

RouteQueryResult<std::vector<CommonTrafficLight::Entity>> WorldDataQuery::GetTrafficLightsInRange(LaneMultiStream laneStream, double startDistance, double searchRange) const
{
    const bool backwardsSearch = searchRange < 0;
    const double startPosition = backwardsSearch ? startDistance + searchRange : startDistance;
    const double endPosition = backwardsSearch ? startDistance : startDistance + searchRange;
    return laneStream.Traverse(LaneMultiStream::TraversedFunction<std::vector<CommonTrafficLight::Entity>>{[&](const auto& lane, const auto& previousTraffiLights)
    {
        std::vector<CommonTrafficLight::Entity> foundTrafficLights{previousTraffiLights};

        if (lane.EndS() < startPosition)
        {
            return foundTrafficLights;
        }
        if (lane.StartS() > endPosition)
        {
            return foundTrafficLights;
        }
        auto sortedTrafficLights = lane().GetTrafficLights();
        sortedTrafficLights.sort([lane](const auto first, const auto second)
        {return lane.GetStreamPosition(first->GetS()) < lane.GetStreamPosition(second->GetS());});
        for (const auto& trafficLight : sortedTrafficLights)
        {
            double trafficLightPosition = lane.GetStreamPosition(trafficLight->GetS() - lane().GetDistance(OWL::MeasurementPoint::RoadStart));
            if (startPosition <= trafficLightPosition && trafficLightPosition <= endPosition)
            {
                foundTrafficLights.push_back(trafficLight->GetSpecification(trafficLightPosition - startDistance));
            }
        }
        return foundTrafficLights;
    }},
    {},
    worldData);
}

RouteQueryResult<std::vector<LaneMarking::Entity>> WorldDataQuery::GetLaneMarkings(const LaneMultiStream& laneStream, double startDistance, double range, Side side) const
{
    return laneStream.Traverse(LaneMultiStream::TraversedFunction<std::vector<LaneMarking::Entity>>{[&](const auto& lane, const auto& previousLaneMarkings)
    {
        std::vector<LaneMarking::Entity> laneMarkings{previousLaneMarkings};
        std::map<double, LaneMarking::Entity> doubleLaneMarkings;
        if (lane.EndS() < startDistance)
        {
            return laneMarkings;;
        }
        if (lane.StartS() > startDistance + range)
        {
            return laneMarkings;;
        }

        const auto& laneBoundaries = ((side == Side::Right) xor lane.inStreamDirection) ? lane().GetLeftLaneBoundaries() : lane().GetRightLaneBoundaries();

        for (auto laneBoundaryIndex : laneBoundaries)
        {
            const auto& laneBoundary = worldData.GetLaneBoundaries().at(laneBoundaryIndex);
            const double boundaryStart = lane.inStreamDirection ? laneBoundary->GetSStart() : std::min(laneBoundary->GetSEnd(), lane().GetLength());
            const double boundaryEnd = lane.inStreamDirection ? std::min(laneBoundary->GetSEnd(), lane().GetLength()) : laneBoundary->GetSStart();
            const double boundaryStreamStart = lane.GetStreamPosition(boundaryStart - lane().GetDistance(OWL::MeasurementPoint::RoadStart));
            const double boundaryStreamEnd = lane.GetStreamPosition(boundaryEnd - lane().GetDistance(OWL::MeasurementPoint::RoadStart));
            if (boundaryStreamStart <= startDistance + range
                    && boundaryStreamEnd >= startDistance)
            {
                LaneMarking::Entity laneMarking;
                laneMarking.relativeStartDistance = boundaryStreamStart - startDistance;
                laneMarking.width = laneBoundary->GetWidth();
                laneMarking.type = laneBoundary->GetType();
                laneMarking.color = laneBoundary->GetColor();
                if (laneBoundary->GetSide() == OWL::LaneMarkingSide::Single)
                {
                    laneMarkings.push_back(laneMarking);
                }
                else
                {
                    if (doubleLaneMarkings.count(laneMarking.relativeStartDistance) == 0)
                    {
                        doubleLaneMarkings.insert(std::make_pair(laneMarking.relativeStartDistance, laneMarking));
                    }
                    else
                    {
                        auto& otherLaneMarking = doubleLaneMarkings.at(laneMarking.relativeStartDistance);
                        LaneMarking::Type leftType;
                        LaneMarking::Type rightType;
                        if (laneBoundary->GetSide() == OWL::LaneMarkingSide::Left)
                        {
                            leftType = laneMarking.type;
                            rightType = otherLaneMarking.type;
                        }
                        else
                        {
                            leftType = otherLaneMarking.type;
                            rightType = laneMarking.type;
                        }
                        LaneMarking::Type combinedType;
                        if (leftType == LaneMarking::Type::Solid && rightType == LaneMarking::Type::Solid)
                        {
                            combinedType = LaneMarking::Type::Solid_Solid;
                        }
                        else if (leftType == LaneMarking::Type::Solid && rightType == LaneMarking::Type::Broken)
                        {
                            combinedType = LaneMarking::Type::Solid_Broken;
                        }
                        else if (leftType == LaneMarking::Type::Broken && rightType == LaneMarking::Type::Solid)
                        {
                            combinedType = LaneMarking::Type::Broken_Solid;
                        }
                        else if (leftType == LaneMarking::Type::Broken && rightType == LaneMarking::Type::Broken)
                        {
                            combinedType = LaneMarking::Type::Broken_Broken;
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type of double lane boundary");
                        }
                        laneMarking.type = combinedType;
                        laneMarkings.push_back(laneMarking);
                    }
                }
            }
        }

        return laneMarkings;
    }},
    {},
    worldData);
}

std::vector<JunctionConnection> WorldDataQuery::GetConnectionsOnJunction(std::string junctionId, std::string incomingRoadId) const
{
    const auto& junction = GetJunctionByOdId(junctionId);
    const auto& incomingRoad = GetRoadByOdId(incomingRoadId);
    std::vector<JunctionConnection> connections;
    for (const auto connectingRoad : junction->GetConnectingRoads())
    {
        if (connectingRoad->GetPredecessor() == incomingRoad->GetId())
        {
            JunctionConnection connection;
            connection.connectingRoadId = connectingRoad->GetId();
            connection.outgoingRoadId = connectingRoad->GetSuccessor();
            const auto& outgoingRoad = worldData.GetRoads().at(connectingRoad->GetSuccessor());
            connection.outgoingStreamDirection =
                    junction->GetId() == (outgoingRoad->IsInStreamDirection() ? outgoingRoad->GetPredecessor() : outgoingRoad->GetSuccessor());
            connections.push_back(connection);
        }
    }
    return connections;
}

std::vector<IntersectingConnection> WorldDataQuery::GetIntersectingConnections(std::string connectingRoadId) const
{
    std::vector<IntersectingConnection> intersections;
    const auto& junction = GetJunctionOfConnector(connectingRoadId);
    auto connectionInfos = junction->GetIntersections().find(connectingRoadId);
    if (connectionInfos != junction->GetIntersections().end())
    {
        std::transform(connectionInfos->second.cbegin(), connectionInfos->second.cend(), std::back_inserter(intersections),
                       [&](const OWL::IntersectionInfo& connectionInfo){return IntersectingConnection{connectionInfo.intersectingRoad, connectionInfo.relativeRank};});
    }
    return intersections;
}

std::vector<JunctionConnectorPriority> WorldDataQuery::GetPrioritiesOnJunction(std::string junctionId) const
{
    std::vector<JunctionConnectorPriority> priorities;
    const auto junction = GetJunctionByOdId(junctionId);
    for (const auto& [high, low] : junction->GetPriorities())
    {
        priorities.push_back({high, low});
    }
    return priorities;
}

RoadNetworkElement WorldDataQuery::GetRoadSuccessor(std::string roadId) const
{
    auto currentRoad = GetRoadByOdId(roadId);
    assert(currentRoad);
    const auto& nextElementId = currentRoad->GetSuccessor();
    if (worldData.GetRoads().count(nextElementId) > 0)
    {
        return {RoadNetworkElementType::Road, nextElementId};
    }
    if (worldData.GetJunctions().count(nextElementId) > 0)
    {
        return {RoadNetworkElementType::Junction, nextElementId};
    }
    return {RoadNetworkElementType::None, ""};
}

RoadNetworkElement WorldDataQuery::GetRoadPredecessor(std::string roadId) const
{
    auto currentRoad = GetRoadByOdId(roadId);
    assert(currentRoad);
    const auto& nextElementId = currentRoad->GetPredecessor();
    if (worldData.GetRoads().count(nextElementId) > 0)
    {
        return {RoadNetworkElementType::Road, nextElementId};
    }
    if (worldData.GetJunctions().count(nextElementId) > 0)
    {
        return {RoadNetworkElementType::Junction, nextElementId};
    }
    return {RoadNetworkElementType::None, ""};
}

std::vector<const OWL::Interfaces::WorldObject*> WorldDataQuery::GetMovingObjectsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const
{
    const auto [route, start, end] = GetRouteLeadingToConnector(connectingRoadId);
    const auto& lanes = GetLanesOfLaneTypeAtDistance(connectingRoadId, 0.0, {LaneType::Driving});
    std::vector<const OWL::Interfaces::WorldObject*> foundMovingObjects;

    for (const auto& lane : lanes)
    {
        auto laneStream = CreateLaneMultiStream(route, start, lane->GetOdId(), 0.0);
        double laneStreamEnd = laneStream->GetPositionByVertexAndS(end, GetRoadByOdId(connectingRoadId)->GetLength());

        auto movingObjects = GetObjectsOfTypeInRange<OWL::Interfaces::MovingObject>(*laneStream, laneStreamEnd - range, laneStreamEnd).at(end);

        for (auto movingObject : movingObjects)
        {
            if (std::count(foundMovingObjects.cbegin(), foundMovingObjects.cend(), movingObject) == 0)
            {
                foundMovingObjects.emplace_back(movingObject);
            }
        }
    }

    return foundMovingObjects;
}

std::tuple<RoadGraph, RoadGraphVertex, RoadGraphVertex> WorldDataQuery::GetRouteLeadingToConnector (std::string connectingRoadId) const
{
     auto incomingRoadId = GetRoadPredecessor(connectingRoadId).id;
     bool incomingRoadLeadsToJunction = GetRoadSuccessor(incomingRoadId).id == GetJunctionOfConnector(connectingRoadId)->GetId();
     RoadGraph route;
     auto end = add_vertex(RouteElement{connectingRoadId, true}, route);
     auto current = add_vertex(RouteElement{incomingRoadId, incomingRoadLeadsToJunction}, route);
     add_edge(current, end, route);
     bool reachedEndOfRoadStream = false;
     auto emplace_element_if = [&](const RoadNetworkElement& element)
     {
         if (element.type == RoadNetworkElementType::Road)
         {
             auto next = add_vertex(RouteElement{element.id, incomingRoadLeadsToJunction}, route);
             add_edge(next, current, route);
             current = next;
             incomingRoadId = element.id;
             return true;
         }
         else
         {
             return false;
         }
     };

     while (!reachedEndOfRoadStream)
     {
         if (incomingRoadLeadsToJunction)
         {
             reachedEndOfRoadStream = !emplace_element_if(GetRoadPredecessor(incomingRoadId));
         }
         else
         {
             reachedEndOfRoadStream = !emplace_element_if(GetRoadSuccessor(incomingRoadId));
         }
     }

     return {route, current, end};
}

double WorldDataQuery::GetDistanceUntilObjectEntersConnector(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    //TODO This function needs to be made working again if OpponentAgent is implemented
//    const OWL::Interfaces::Junction* junction = GetJunctionOfConnector(intersectingConnectorId);
//    const auto& intersections = junction->GetIntersections().at(intersectingConnectorId);
//    const auto& intersection = std::find_if(intersections.cbegin(), intersections.cend(),
//                                            [this, ownConnectorId](const auto& connectionInfo){return worldData.GetRoadIdMapping().at(connectionInfo.intersectingRoad) == ownConnectorId;});
//    if (intersection == intersections.cend())
//    {
//        return std::numeric_limits<double>::max();
//    }
//    std::vector<RouteElement> route = GetRouteLeadingToConnector(ownConnectorId);
//    if (std::find_if(route.cbegin(), route.cend(),[&](const auto& element) {return element.roadId == position.mainLocatePoint.roadId;}) == route.end())
//    {
//        route.push_back({position.mainLocatePoint.roadId, position.mainLocatePoint.laneId < 0}); //Hotfix
//    }
//    const auto laneStream = CreateLaneStream(route, position.mainLocatePoint.roadId, position.mainLocatePoint.laneId, position.mainLocatePoint.roadPosition.s);
//    const auto& laneOfObject = GetLaneByOdId(position.mainLocatePoint.roadId, position.mainLocatePoint.laneId, position.mainLocatePoint.roadPosition.s);
//    double sStartOfObjectOnLaneStream = laneStream->GetPositionByElementAndS(laneOfObject, position.touchedRoads.at(position.mainLocatePoint.roadId).sStart);
//    double sEndOfObjectOnLaneStream = laneStream->GetPositionByElementAndS(laneOfObject, position.touchedRoads.at(position.mainLocatePoint.roadId).sEnd);
//    double frontOfObjectOnLaneStream = std::max(sStartOfObjectOnLaneStream, sEndOfObjectOnLaneStream);
//    const auto& intersectingLane = GetLaneByOdId(intersectingConnectorId, intersectingLaneId, 0.0);
//    const auto ownLane = std::find_if(laneStream->GetElements().cbegin(), laneStream->GetElements().cend(),
//                 [&](const LaneStreamInfo& element){return worldData.GetRoadIdMapping().at(element.element->GetRoad().GetId()) == ownConnectorId;});
//    double sStart = intersection->sOffsets.at({intersectingLane.GetId(), ownLane->element->GetId()}).first;
//    double positionOfIntersectionOnLaneStream = laneStream->GetPositionByElementAndS(*ownLane->element, sStart);
//    return positionOfIntersectionOnLaneStream - frontOfObjectOnLaneStream;
    return 0;
}

double WorldDataQuery::GetDistanceUntilObjectLeavesConnector(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    //TODO This function needs to be made working again if OpponentAgent is implemented
//    const OWL::Interfaces::Junction* junction = GetJunctionOfConnector(intersectingConnectorId);
//    const auto& intersections = junction->GetIntersections().at(intersectingConnectorId);
//    const auto& intersection = std::find_if(intersections.cbegin(), intersections.cend(),
//                                            [this, ownConnectorId](const auto& connectionInfo){return worldData.GetRoadIdMapping().at(connectionInfo.intersectingRoad) == ownConnectorId;});
//    if (intersection == intersections.cend())
//    {
//        return std::numeric_limits<double>::max();
//    }
//    std::vector<RouteElement> route = GetRouteLeadingToConnector(ownConnectorId);
//    if (std::find_if(route.cbegin(), route.cend(),[&](const auto& element) {return element.roadId == position.mainLocatePoint.roadId;}) == route.end())
//    {
//        route.push_back({position.mainLocatePoint.roadId, position.mainLocatePoint.laneId < 0}); //Hotfix
//    }
//    const auto laneStream = CreateLaneStream(route, position.mainLocatePoint.roadId, position.mainLocatePoint.laneId, position.mainLocatePoint.roadPosition.s);
//    const auto& laneOfObject = GetLaneByOdId(position.mainLocatePoint.roadId, position.mainLocatePoint.laneId, position.mainLocatePoint.roadPosition.s);
//    double sStartOfObjectOnLaneStream = laneStream->GetPositionByElementAndS(laneOfObject, position.touchedRoads.at(position.mainLocatePoint.roadId).sStart);
//    double sEndOfObjectOnLaneStream = laneStream->GetPositionByElementAndS(laneOfObject, position.touchedRoads.at(position.mainLocatePoint.roadId).sEnd);
//    double rearOfObjectOnLaneStream = std::min(sStartOfObjectOnLaneStream, sEndOfObjectOnLaneStream);
//    const auto& intersectingLane = GetLaneByOdId(intersectingConnectorId, intersectingLaneId, 0.0);
//    const auto ownLane = std::find_if(laneStream->GetElements().cbegin(), laneStream->GetElements().cend(),
//                 [&](const LaneStreamInfo& element){return worldData.GetRoadIdMapping().at(element.element->GetRoad().GetId()) == ownConnectorId;});
//    double sEnd = intersection->sOffsets.at({intersectingLane.GetId(), ownLane->element->GetId()}).second;
//    double positionOfIntersectionOnLaneStream = laneStream->GetPositionByElementAndS(*ownLane->element, sEnd);
//    return positionOfIntersectionOnLaneStream - rearOfObjectOnLaneStream;
    return 0;
}

std::shared_ptr<const LaneMultiStream> WorldDataQuery::CreateLaneMultiStream(const RoadGraph& roadGraph, RoadGraphVertex start, OWL::OdId startLaneId, double startDistance) const
{
    const auto& routeElement = get(RouteElement(), roadGraph, start);
    const auto& startLane = GetLaneByOdId(routeElement.roadId, startLaneId, startDistance);
    if (!startLane.Exists())
    {
        return std::make_shared<const LaneMultiStream>(CreateLaneMultiStreamRecursive(roadGraph, start, 0.0, nullptr));
    }
    return std::make_shared<const LaneMultiStream>(CreateLaneMultiStreamRecursive(roadGraph, start, 0.0, &startLane));
}

std::unique_ptr<RoadStream> WorldDataQuery::CreateRoadStream(const std::vector<RouteElement>& route) const
{
    double currentS = 0.0;

    std::vector<RoadStreamElement> roads;
    std::transform(route.cbegin(),
                   route.cend(),
                   std::back_inserter(roads),
                   [&](const auto& routeElement) -> RoadStreamElement
    {
        RoadStreamElement roadStreamInfo;
        roadStreamInfo.road = GetRoadByOdId(routeElement.roadId);
        roadStreamInfo.inStreamDirection = routeElement.inOdDirection;
        roadStreamInfo.sOffset = currentS + (routeElement.inOdDirection ? 0 : roadStreamInfo.road->GetLength());

        currentS += roadStreamInfo.road->GetLength();
        return roadStreamInfo;
    });
    return std::make_unique<RoadStream>(std::move(roads));
}

std::shared_ptr<const RoadMultiStream> WorldDataQuery::CreateRoadMultiStream(const RoadGraph& roadGraph, RoadGraphVertex start) const
{
    return std::make_shared<const RoadMultiStream>(CreateRoadMultiStreamRecursive(roadGraph, start, 0.0));
}

RoadMultiStream::Node WorldDataQuery::CreateRoadMultiStreamRecursive(const RoadGraph& roadGraph, const RoadGraphVertex& current, double sOffset) const
{
    const auto routeElement = get(RouteElement(), roadGraph, current);
    const auto road = GetRoadByOdId(routeElement.roadId);
    if (!road)
    {
        throw std::runtime_error("Unknown road \"" + routeElement.roadId + "\"");
    }
    auto roadLength = road->GetLength();
    std::vector<RoadMultiStream::Node> next{};
    for (auto [successor, successorsEnd] = adjacent_vertices(current, roadGraph); successor != successorsEnd; successor++)
    {
        next.push_back(CreateRoadMultiStreamRecursive(roadGraph, *successor, sOffset + roadLength));
    }
    auto streamInfo = std::optional<RoadStreamInfo>(std::in_place_t(), road, sOffset + (routeElement.inOdDirection ? 0 : roadLength), routeElement.inOdDirection);
    RoadMultiStream::Node root{streamInfo, {next}, current};

    return root;
}

LaneMultiStream::Node WorldDataQuery::CreateLaneMultiStreamRecursive(const RoadGraph& roadGraph, const RoadGraphVertex& current, double sOffset, const OWL::Lane* lane) const
{
    const auto routeElement = get(RouteElement(), roadGraph, current);
    std::vector<LaneMultiStream::Node> next{};
    auto laneLength = lane ? lane->GetLength() : 0.0;
    const auto& laneSuccessors = lane ? (routeElement.inOdDirection ? lane->GetNext() : lane->GetPrevious()) : std::vector<OWL::Id>{};
    bool foundSuccessorOnSameRoad = false;
    if (laneSuccessors.size() == 1)
    {
        const auto& laneSuccessor = worldData.GetLanes().at(laneSuccessors.front());
        const auto& successorRoadId = laneSuccessor->GetRoad().GetId();
        if (successorRoadId == routeElement.roadId)
        {
            next.push_back(CreateLaneMultiStreamRecursive(roadGraph, current, sOffset + laneLength, laneSuccessor));
            foundSuccessorOnSameRoad = true;
        }
    }
    if (!foundSuccessorOnSameRoad)
    {
        for (auto [successor, successorsEnd] = adjacent_vertices(current, roadGraph); successor != successorsEnd; successor++)
        {
            auto successorRoadId = get(RouteElement(), roadGraph, *successor).roadId;
            auto laneSuccessorId = std::find_if(laneSuccessors.begin(), laneSuccessors.end(), [&](const auto& laneSuccessorId)
            {
                const auto& laneSuccessor = worldData.GetLanes().at(laneSuccessorId);
                const auto& laneSuccessorRoadId = laneSuccessor->GetRoad().GetId();
                return laneSuccessorRoadId == successorRoadId;
            });
            if (laneSuccessorId != laneSuccessors.end())
            {
                next.push_back(CreateLaneMultiStreamRecursive(roadGraph, *successor, sOffset + laneLength, worldData.GetLanes().at(*laneSuccessorId)));
            }
            else
            {
                next.push_back(CreateLaneMultiStreamRecursive(roadGraph, *successor, sOffset + laneLength, nullptr));
            }
        }
    }
    auto streamInfo = lane ? std::optional<LaneStreamInfo>(std::in_place_t(), lane, sOffset + (routeElement.inOdDirection ? 0 : laneLength), routeElement.inOdDirection)
                           : std::nullopt;
    LaneMultiStream::Node root{streamInfo, {next}, current};

    return root;
}

OWL::CLane* WorldDataQuery::GetOriginatingRouteLane(std::vector<RouteElement> route, std::string startRoadId, OWL::OdId startLaneId, double startDistance) const
{
    OWL::CLane* currentLane = &GetLaneByOdId(startRoadId, startLaneId, startDistance);
    auto routeIterator = std::find_if(route.crbegin(), route.crend(),
                                      [&](const auto& routeElement){
        return routeElement.roadId == startRoadId;
    } );
    bool reachedMostUpstreamLane = false;
    bool inStreamDirection = routeIterator->inOdDirection;
    while (!reachedMostUpstreamLane)
    {
        auto upstreamLanes = inStreamDirection ? currentLane->GetPrevious() : currentLane->GetNext();
        if (upstreamLanes.empty())
        {
            reachedMostUpstreamLane = true;
            break;
        }
        //search predecessor in current road
        auto upstreamLane = std::find_if(upstreamLanes.cbegin(), upstreamLanes.cend(),
                                         [routeIterator, this](const OWL::Id& laneId)
        {
            const auto upStreamLane = worldData.GetLanes().at(laneId);
            const auto upStreamRoadId = upStreamLane->GetRoad().GetId();
            return upStreamRoadId == routeIterator->roadId;
        });
        if (upstreamLane == upstreamLanes.cend()) //no predecessor in current road -> go to previous road
        {
            ++routeIterator;
            if (routeIterator == route.crend())
            {
                reachedMostUpstreamLane = true; //no previous road -> reached begin of the route
                break;
            }
            //search predecessor in previous road
            upstreamLane = std::find_if(upstreamLanes.cbegin(), upstreamLanes.cend(),
                                        [routeIterator, this](const OWL::Id& laneId)
            {
                const auto upStreamLane = worldData.GetLanes().at(laneId);
                const auto upStreamRoadId = upStreamLane->GetRoad().GetId();
                return upStreamRoadId == routeIterator->roadId;
            });
        }
        if (upstreamLane == upstreamLanes.cend()) //none of the predecessors is on the route
        {
            reachedMostUpstreamLane = true;
            break;
        }
        const auto& next = worldData.GetLanes().at(*upstreamLane)->GetNext();
        inStreamDirection = routeIterator->inOdDirection;
        currentLane = worldData.GetLanes().at(*upstreamLane);
    }
    return currentLane;
}

RouteQueryResult<LongitudinalDistance> WorldDataQuery::GetDistanceBetweenObjects(const RoadMultiStream& roadStream,
                                                                                 const ObjectPosition& objectPos,
                                                                                 const std::optional<double> objectReferenceS,
                                                                                 const ObjectPosition& targetObjectPos) const
{
    const auto& rootElement = roadStream.GetRoot().element.value();
    const auto& rootRoadId = rootElement.element->GetId();
    double objectPositionStart = rootElement.GetStreamPosition(objectPos.touchedRoads.at(rootRoadId).sStart);
    double objectPositionEnd = rootElement.GetStreamPosition(objectPos.touchedRoads.at(rootRoadId).sEnd);
    double objectPositionMin = std::min(objectPositionStart, objectPositionEnd);
    double objectPositionMax = std::max(objectPositionStart, objectPositionEnd);
    double objectPositionReferencPoint{0};
    if (objectReferenceS)
    {
        objectPositionReferencPoint  = rootElement.GetStreamPosition(objectReferenceS.value());
    }
    return roadStream.Traverse<LongitudinalDistance>(RoadMultiStream::TraversedFunction<LongitudinalDistance>{[&](const auto& road, const auto& previousResult)
    {
        const auto& roadId = road.element->GetId();
        if ((previousResult.netDistance.has_value() && previousResult.referencePoint.has_value())
            || targetObjectPos.touchedRoads.count(roadId) == 0)
        {
            return previousResult;
        }
        double targetObjectPositionStart = road.GetStreamPosition(targetObjectPos.touchedRoads.at(roadId).sStart);
        double targetObjectPositionEnd = road.GetStreamPosition(targetObjectPos.touchedRoads.at(roadId).sEnd);
        double targetObjectPositionMin = std::min(targetObjectPositionStart, targetObjectPositionEnd);
        double targetObjectPositionMax = std::max(targetObjectPositionStart, targetObjectPositionEnd);
        std::optional<double> referenceDistance = std::nullopt;
        if (objectReferenceS && targetObjectPos.referencePoint.find(roadId) != targetObjectPos.referencePoint.end())
        {
            referenceDistance = road.GetStreamPosition(targetObjectPos.referencePoint.at(roadId).roadPosition.s) - objectPositionReferencPoint;
        }
        else if (previousResult.referencePoint.has_value())
        {
            referenceDistance = previousResult.referencePoint;
        }
        if (objectPositionMin > targetObjectPositionMax)
        {
            return LongitudinalDistance{targetObjectPositionMax - objectPositionMin, referenceDistance};
        }
        else if (targetObjectPositionMin > objectPositionMax)
        {
            return LongitudinalDistance{targetObjectPositionMin - objectPositionMax, referenceDistance};
        }
        else
        {
            return LongitudinalDistance{0, referenceDistance};
        }
    }},
    {},
    worldData);
}

Position WorldDataQuery::GetPositionByDistanceAndLane(const OWL::Interfaces::Lane& lane, double distanceOnLane, double offset) const
{
    const auto& referencePoint = lane.GetInterpolatedPointsAtDistance(distanceOnLane).reference;
    auto yaw = lane.GetDirection(distanceOnLane);

    return Position
    {
        referencePoint.x - std::sin(yaw) * offset,
        referencePoint.y + std::cos(yaw) * offset,
        yaw,
        lane.GetCurvature(distanceOnLane)
    };
}

RouteQueryResult<RelativeWorldView::Junctions> WorldDataQuery::GetRelativeJunctions(const RoadMultiStream& roadStream, double startPosition, double range) const
{
    return roadStream.Traverse(RoadMultiStream::TraversedFunction<RelativeWorldView::Junctions>{[&](const auto& road, const auto& previousResult)
    {
        if (road.EndS() < startPosition)
        {
            return previousResult;
        }
        if (road.StartS() > startPosition + range)
        {
            return previousResult;
        }
        RelativeWorldView::Junctions junctions{previousResult};
        std::string roadId = road().GetId();
        auto junction = GetJunctionOfConnector(roadId);
        if (junction)
        {
            double startS = road.StartS() - startPosition;
            double endS = road.EndS() - startPosition;
            junctions.push_back({startS, endS, roadId});
        }
        return junctions;
    }},
    {},
    worldData);
}

std::optional<int> GetIdOfPredecessor(std::vector<OWL::Id> predecessors, std::map<int, OWL::Id> previousSectionLaneIds)
{
    auto it = std::find_if(previousSectionLaneIds.cbegin(), previousSectionLaneIds.cend(),
                 [&](const auto& lane){return std::count(predecessors.cbegin(), predecessors.cend(), lane.second) > 0;});
    if (it == previousSectionLaneIds.cend())
    {
        return std::nullopt;
    }
    return it->first;
}

int WorldDataQuery::FindNextEgoLaneId (const OWL::Interfaces::Lanes& lanesOnSection, bool inStreamDirection, std::map<int, OWL::Id> previousSectionLaneIds) const
{
    for (const auto& lane : lanesOnSection)
    {
        const auto predecessors = inStreamDirection ? lane->GetPrevious() : lane->GetNext();
        std::optional<int> predecessorRelativeId = GetIdOfPredecessor(predecessors, previousSectionLaneIds);
        if (predecessorRelativeId)
        {
            if (predecessorRelativeId == 0)
            {
                return lane->GetOdId();
            }
        }
    }
    return 0;
}

std::map<int, OWL::Id> WorldDataQuery::AddLanesOfSection(const OWL::Interfaces::Lanes& lanesOnSection, bool inStreamDirection, int currentOwnLaneId, bool includeOncoming,
                                                         const std::map<int, OWL::Id>& previousSectionLaneIds, std::vector<RelativeWorldView::Lane>& previousSectionLanes,
                                                         RelativeWorldView::LanesInterval& laneInterval) const
{
    std::map<int, OWL::Id> lanesOnSectionLaneIds{};
    for (const auto& lane : lanesOnSection)
    {
        const auto& laneId = lane->GetOdId();
        bool inDrivingDirection = inStreamDirection ? (laneId < 0) : (laneId > 0);
        if(!includeOncoming && !inDrivingDirection)
        {
            continue;
        }
        int relativeLaneId = inStreamDirection ? (laneId - currentOwnLaneId) : (currentOwnLaneId - laneId);
        bool differentSigns = currentOwnLaneId * laneId < 0;
        if (differentSigns)
        {
            relativeLaneId += (relativeLaneId > 0) ? -1 : 1;
        }
        lanesOnSectionLaneIds.insert({relativeLaneId, lane->GetId()});
        const auto predecessors = inStreamDirection ? lane->GetPrevious() : lane->GetNext();
        std::optional<int> predecessorRelativeId = GetIdOfPredecessor(predecessors, previousSectionLaneIds);
        laneInterval.lanes.push_back({relativeLaneId, inDrivingDirection, lane->GetLaneType(), predecessorRelativeId, std::nullopt});
        if (predecessorRelativeId)
        {
            auto predecessor = std::find_if(previousSectionLanes.begin(), previousSectionLanes.end(),
                                            [&](const auto& lane){return lane.relativeId == predecessorRelativeId;});
            predecessor->successor = relativeLaneId;
        }
    }
    return lanesOnSectionLaneIds;
}

RouteQueryResult<RelativeWorldView::Lanes> WorldDataQuery::GetRelativeLanes(const RoadMultiStream& roadStream, double startPosition, int startLaneId, double range, bool includeOncoming) const
{
    int currentOwnLaneId = startLaneId;
    return roadStream.Traverse<RelativeWorldView::Lanes, std::map<int, OWL::Id>>(
     RoadMultiStream::TraversedFunction<RelativeWorldView::Lanes, std::map<int, OWL::Id>>{[&](const auto& road, const auto& previousResult, const auto& previousLaneIds)
    {
        std::map<int, OWL::Id> previousSectionLaneIds{previousLaneIds};
        if (road.EndS() < startPosition)
        {
            return std::make_tuple(previousResult, previousLaneIds);
        }
        if (road.StartS() > startPosition + range)
        {
            return std::make_tuple(previousResult, previousLaneIds);
        }
        RelativeWorldView::Lanes relativeLanes{previousResult};
        auto sections = road().GetSections();
        if (!road.inStreamDirection)
        {
            sections.reverse();
        }
        for (const auto& section : sections)
        {
            const double sectionStart = road.GetStreamPosition(section->GetSOffset() + (road.inStreamDirection ? 0 : section->GetLength()));
            const double sectionEnd = road.GetStreamPosition(section->GetSOffset() + (road.inStreamDirection ? section->GetLength() : 0));
            if (sectionEnd < startPosition)
            {
                continue;
            }
            if (sectionStart > startPosition + range)
            {
                return std::make_tuple(relativeLanes, previousSectionLaneIds);
            }
            RelativeWorldView::LanesInterval laneInterval;
            laneInterval.startS = sectionStart - startPosition;
            laneInterval.endS = sectionEnd - startPosition;
            const auto& lanesOnSection = section->GetLanes();
            if (previousSectionLaneIds.empty())
            {
                currentOwnLaneId = startLaneId;
            }
            else
            {
                currentOwnLaneId = FindNextEgoLaneId(lanesOnSection, road.inStreamDirection, previousSectionLaneIds);
            }
            auto lanesOnSectionLaneIds = AddLanesOfSection(lanesOnSection, road.inStreamDirection, currentOwnLaneId, includeOncoming, previousSectionLaneIds, relativeLanes.back().lanes, laneInterval);
            previousSectionLaneIds = lanesOnSectionLaneIds;
            relativeLanes.push_back(laneInterval);
        }
        return std::make_tuple(relativeLanes, previousSectionLaneIds);
    }},
    {},
    {},
    worldData);
}

RouteQueryResult<std::optional<int>> WorldDataQuery::GetRelativeLaneId(const RoadMultiStream &roadStream, double ownPosition, int ownLaneId, GlobalRoadPositions targetPosition) const
{
    std::optional<int> currentOwnLaneId;
    std::optional<int> currentTargetLaneId;
    return roadStream.Traverse<std::optional<int>, std::map<int, OWL::Id>>(
     RoadMultiStream::TraversedFunction<std::optional<int>, std::map<int, OWL::Id>>{[&](const auto& road, const auto& previousResult, const auto& previousLaneIds)->std::tuple<std::optional<int>, std::map<int, OWL::Id>>
    {
        if (previousResult.has_value())
        {
            return std::make_tuple(previousResult, previousLaneIds);
        }
        const auto& roadId = road.element->GetId();
        auto positionOnRoad = helper::map::query(targetPosition, roadId);
        auto streamPosition = road.GetStreamPosition(positionOnRoad->roadPosition.s);
        auto sections = road().GetSections();
        if (!road.inStreamDirection)
        {
            sections.reverse();
        }
        std::map<int, OWL::Id> previousSectionLaneIds{previousLaneIds};
        for (const auto& section : sections)
        {
            const double sectionStart = road.GetStreamPosition(section->GetSOffset() + (road.inStreamDirection ? 0 : section->GetLength()));
            const double sectionEnd = road.GetStreamPosition(section->GetSOffset() + (road.inStreamDirection ? section->GetLength() : 0));
            bool onSection = positionOnRoad.has_value() && sectionStart <= streamPosition && sectionEnd >= streamPosition;
            const auto& lanesOnSection = section->GetLanes();
            if (sectionStart <= ownPosition && sectionEnd >= ownPosition)
            {
                if (currentTargetLaneId.has_value())
                {
                    currentTargetLaneId = FindNextEgoLaneId(lanesOnSection, road.inStreamDirection, previousSectionLaneIds);
                }
                else
                {
                    currentOwnLaneId = ownLaneId;
                }
            }
            else
            {
                if (onSection)
                {
                    currentTargetLaneId = positionOnRoad->laneId;
                }
                else if (currentTargetLaneId.has_value())
                {
                    currentTargetLaneId = FindNextEgoLaneId(lanesOnSection, road.inStreamDirection, previousSectionLaneIds);
                }
                if (currentOwnLaneId.has_value())
                {
                    currentOwnLaneId = FindNextEgoLaneId(lanesOnSection, road.inStreamDirection, previousSectionLaneIds);
                }
            }
            if (!currentOwnLaneId && !currentTargetLaneId)
            {
                continue;
            }
            previousSectionLaneIds = {};
            for (auto lane : section->GetLanes())
            {
                const auto& laneId = lane->GetOdId();
                const auto currentId = currentOwnLaneId.has_value() ? currentOwnLaneId.value() : currentTargetLaneId.value();
                int relativeLaneId = road.inStreamDirection ? (laneId - currentId) : (currentId - laneId);
                bool differentSigns = currentId * laneId < 0;
                if (differentSigns)
                {
                    relativeLaneId += (relativeLaneId > 0) ? -1 : 1;
                }
                if (currentOwnLaneId.has_value())
                {
                    if (onSection && positionOnRoad->laneId == laneId)
                    {
                        return std::make_tuple(relativeLaneId, previousLaneIds);
                    }
                }
                else
                {
                    if (sectionStart <= ownPosition && sectionEnd >= ownPosition && ownLaneId == laneId)
                    {
                        return std::make_tuple(-relativeLaneId, previousLaneIds);
                    }
                }
                previousSectionLaneIds.insert({relativeLaneId, lane->GetId()});
            }
         }
         return std::make_tuple(std::nullopt, previousSectionLaneIds);
    }},
    {},
    {},
    worldData);
}

RouteQueryResult<std::optional<double> > WorldDataQuery::GetLaneCurvature(const LaneMultiStream& laneStream, double position) const
{
    return laneStream.Traverse<std::optional<double>>(LaneMultiStream::TraversedFunction<std::optional<double>>{[&](const auto& lane, const auto& previousResult)
    {
        if (lane.StartS() <= position && lane.EndS() >= position)
        {
            return std::optional<double>(std::in_place_t(), lane.element->GetCurvature(lane.GetElementPosition(position) + lane.element->GetDistance(OWL::MeasurementPoint::RoadStart)));
        }
        return previousResult;
    }},
    std::nullopt,
    worldData);
}

RouteQueryResult<std::optional<double> > WorldDataQuery::GetLaneWidth(const LaneMultiStream& laneStream, double position) const
{
    return laneStream.Traverse<std::optional<double>>(LaneMultiStream::TraversedFunction<std::optional<double>>{[&](const auto& lane, const auto& previousResult)
    {
        if (lane.StartS() <= position && lane.EndS() >= position)
        {
            return std::optional<double>(std::in_place_t(), lane.element->GetWidth(lane.GetElementPosition(position) + lane.element->GetDistance(OWL::MeasurementPoint::RoadStart)));
        }
        return previousResult;
    }},
    std::nullopt,
    worldData);
}

RouteQueryResult<std::optional<double> > WorldDataQuery::GetLaneDirection(const LaneMultiStream& laneStream, double position) const
{
    return laneStream.Traverse<std::optional<double>>(LaneMultiStream::TraversedFunction<std::optional<double>>{[&](const auto& lane, const auto& previousResult)
    {
        if (lane.StartS() <= position && lane.EndS() >= position)
        {
            return std::optional<double>(std::in_place_t(), lane.element->GetDirection(lane.GetElementPosition(position) + lane.element->GetDistance(OWL::MeasurementPoint::RoadStart)));
        }
        return previousResult;
    }},
    std::nullopt,
    worldData);
}

double CalculatePerpendicularDistance(const Common::Vector2d& point, const Common::Line& line)
{
    const double lamdba = line.directionalVector.Dot(point - line.startPoint)
                    / line.directionalVector.Dot(line.directionalVector);
    Common::Vector2d foot = line.startPoint + line.directionalVector * lamdba;
    Common::Vector2d vectorToLeft{-line.directionalVector.y, line.directionalVector.x};
    Common::Vector2d distance = point - foot;
    bool isLeft = distance.Dot(vectorToLeft) >= 0;
    return isLeft ? distance.Length() : -distance.Length();
}

std::optional<Position> WorldDataQuery::CalculatePositionIfOnLane(double sCoordinate, double tCoordinate, const OWL::Interfaces::Lane& lane) const
{
    const double laneStart = lane.GetDistance(OWL::MeasurementPoint::RoadStart);
    const double laneEnd = lane.GetDistance(OWL::MeasurementPoint::RoadEnd);
    if (sCoordinate >= laneStart && sCoordinate <= laneEnd)
    {
        return GetPositionByDistanceAndLane(lane, sCoordinate, tCoordinate);
    }
    else
    {
        return std::nullopt;
    }
}

RouteQueryResult<Obstruction> WorldDataQuery::GetObstruction(const LaneMultiStream& laneStream, double tCoordinate,
                                                             const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners, const Common::Vector2d& mainLaneLocator) const
{
    return laneStream.Traverse<Obstruction, std::pair<std::optional<Position>,std::optional<Position>>>(LaneMultiStream::TraversedFunction<Obstruction, std::pair<std::optional<Position>,std::optional<Position>>>
    {[&](const auto& lane, const auto& previousResult, const auto& previousPoints)
     {
         std::optional<Position> firstPoint{previousPoints.first};
         std::optional<Position> secondPoint{previousPoints.second};
         const auto it = otherPosition.touchedRoads.find(lane().GetRoad().GetId());
         if (it == otherPosition.touchedRoads.end())
         {
             return std::make_tuple(previousResult, std::make_pair(firstPoint, secondPoint));
         }
         const double objectStart = it->second.sStart;
         const double objectEnd = it->second.sEnd;
         if (lane.inStreamDirection)
         {
             if(!firstPoint)
             {
                 firstPoint = CalculatePositionIfOnLane(objectStart, tCoordinate, lane());
             }
             if(!secondPoint)
             {
                 secondPoint = CalculatePositionIfOnLane(objectEnd, tCoordinate, lane());
             }
         }
         else
         {
             if(!firstPoint)
             {
                 firstPoint = CalculatePositionIfOnLane(objectEnd, -tCoordinate, lane());
             }
             if(!secondPoint)
             {
                 secondPoint = CalculatePositionIfOnLane(objectStart, -tCoordinate, lane());
             }
         }
         if (!firstPoint || !secondPoint)
         {
             return std::make_tuple(Obstruction::Invalid(), std::make_pair(firstPoint, secondPoint));
         }

         double leftDistance{std::numeric_limits<double>::lowest()};
         double rightDistance{std::numeric_limits<double>::max()};
         for (const auto& corner : objectCorners)
         {
             double distance = CalculatePerpendicularDistance(corner, Common::Line{{firstPoint.value().xPos, firstPoint.value().yPos}, {secondPoint.value().xPos, secondPoint.value().yPos}});
             leftDistance = std::max(leftDistance, distance);
             rightDistance = std::min(rightDistance, distance);
         }
         double mainLocatorDistance = CalculatePerpendicularDistance(mainLaneLocator, Common::Line{{firstPoint.value().xPos, firstPoint.value().yPos}, {secondPoint.value().xPos, secondPoint.value().yPos}});
         return std::make_tuple(Obstruction{leftDistance, rightDistance, mainLocatorDistance}, std::make_pair(firstPoint, secondPoint));
     }},
     Obstruction::Invalid(),
     std::make_pair(std::nullopt, std::nullopt),
     worldData);
}

// explicit template instantiation
template class Stream<OWL::Interfaces::Lane>;
template class Stream<OWL::Interfaces::Road>;
