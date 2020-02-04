/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
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

double WorldDataQuery::GetDistanceToEndOfLane(const LaneStream &laneStream, double initialSearchPosition, double maxSearchLength, std::list<LaneType> requestedLaneTypes) const
{
    for (const auto& lane : laneStream.GetElements())
    {
        if (lane.EndS() < initialSearchPosition)
        {
            continue;
        }
        if (lane.StartS() > initialSearchPosition + maxSearchLength)
        {
            return std::numeric_limits<double>::infinity();
        }
        if (std::find(requestedLaneTypes.cbegin(), requestedLaneTypes.cend(), lane().GetLaneType()) == requestedLaneTypes.cend())
        {
            return lane.StartS() - initialSearchPosition;
        }
        if (lane.EndS() > initialSearchPosition + maxSearchLength)
        {
            return std::numeric_limits<double>::infinity();
        }
    }
    return std::max(0.0, laneStream.GetElements().crbegin()->EndS() - initialSearchPosition);
}

OWL::Id WorldDataQuery::GetStreamId(OWL::CLane& lane) const
{
    //Hotfix until spawn point rework
    const auto& road = lane.GetRoad();
    const auto& roadOdId = worldData.GetRoadIdMapping().at(road.GetId());
    const auto laneStream = CreateLaneStream({{roadOdId, true}}, roadOdId, worldData.GetLaneIdMapping().at(lane.GetId()), lane.GetDistance(OWL::MeasurementPoint::RoadStart));
    return laneStream->GetElements().front().element->GetId();
}

std::pair<double, double> WorldDataQuery::GetLimitingDistances(OWL::CLane& lane) const
{
    //Hotfix until spawn point rework
    const auto& road = lane.GetRoad();
    const auto& roadOdId = worldData.GetRoadIdMapping().at(road.GetId());
    const auto laneStream = CreateLaneStream({{roadOdId, true}}, roadOdId, worldData.GetLaneIdMapping().at(lane.GetId()), lane.GetDistance(OWL::MeasurementPoint::RoadStart));

    double lowerDistance = laneStream->GetElements().front().element->GetDistance(OWL::MeasurementPoint::RoadStart);
    double upperDistance = laneStream->GetElements().back().element->GetDistance(OWL::MeasurementPoint::RoadStart);

    return std::make_pair(lowerDistance, upperDistance);
}

OWL::CSection* WorldDataQuery::GetSectionByDistance(std::string odRoadId, double distance) const
{
    distance = std::max(0.0, distance);

    for (const auto& road : worldData.GetRoads())
    {
        if (worldData.GetRoadIdMapping().at(road.first) == odRoadId)
        {
            for (auto tmpSection : road.second->GetSections())
            {
                if (tmpSection->Covers(distance))
                {
                    return tmpSection;
                }
            }
        }
        continue;
    }

    return nullptr;
}

OWL::CRoad* WorldDataQuery::GetRoadByOdId(std::string odRoadId) const
{
    for (const auto& [osiId, odId] : worldData.GetRoadIdMapping())
    {
        if (odRoadId == odId)
        {
            return worldData.GetRoads().at(osiId);
        }
    }
    return  nullptr;
}

const OWL::Interfaces::Junction *WorldDataQuery::GetJunctionByOdId(const std::string &odJunctionId) const
{
    for (const auto& [osiId, odId] : worldData.GetJunctionIdMapping())
    {
        if (odJunctionId == odId)
        {
            return worldData.GetJunctions().at(osiId);
        }
    }
    return  nullptr;
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


bool WorldDataQuery::ExistsDrivingLaneOnSide(std::string roadId, OWL::OdId laneId, double distance, Side side)
{
    OWL::CLane& lane = GetLaneByOdId(roadId, laneId, distance);

    if (side == Side::Left)
    {
        return lane.GetLeftLane().Exists() && (lane.GetLeftLane().GetLaneType() == LaneType::Driving);
    }
    else
    {
        return lane.GetRightLane().Exists() && (lane.GetRightLane().GetLaneType() == LaneType::Driving);
    }
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
        if (worldData.GetLaneIdMapping().at(lane->GetId()) == odLaneId)
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

int WorldDataQuery::GetNumberOfLanes(std::string roadId, double distance)
{
    auto section = GetSectionByDistance(roadId, distance);
    if(!section)
    {
        return 0;
    }
    return section->GetLanes().size();
}


double WorldDataQuery::GetNextValidSOnLaneInDownstream(std::string roadId, OWL::OdId laneId, double initialDistance,
        double stepSizeLookingForValidS)
{
    double maxSearchLength = OWL::EVENTHORIZON;
    double nextValidS = initialDistance + stepSizeLookingForValidS;
    double maxDistance = initialDistance + maxSearchLength;

    while (!IsSValidOnLane(roadId, laneId, nextValidS) && nextValidS < maxDistance)
    {
        nextValidS += stepSizeLookingForValidS;
    }

    if (nextValidS >= maxDistance)
    {
        return static_cast<double>(INFINITY);
    }

    return nextValidS;
}

double WorldDataQuery::GetLastValidSInUpstream(std::string roadId, OWL::OdId laneId, double initialDistance, double stepSizeLookingForValidS)
{
    double maxSearchLength = OWL::EVENTHORIZON;
    double lastValidSOnLane = initialDistance - stepSizeLookingForValidS;
    double minDistance = std::max(0.0, initialDistance - maxSearchLength);

    while (!IsSValidOnLane(roadId, laneId, lastValidSOnLane) && lastValidSOnLane > minDistance)
    {
        lastValidSOnLane -= stepSizeLookingForValidS;
    }

    if (lastValidSOnLane < minDistance)
    {
        return static_cast<double>(-INFINITY);
    }

    return lastValidSOnLane;
}

std::vector<std::pair<double, OWL::Interfaces::TrafficSign *>> WorldDataQuery::GetTrafficSignsInRange(LaneStream laneStream, double startDistance, double searchRange) const
{
    std::vector<std::pair<double, OWL::Interfaces::TrafficSign *>> foundTrafficSigns{};

    for (const auto& lane : laneStream.GetElements())
    {
        if (lane.EndS() < startDistance)
        {
            continue;
        }
        if (lane.StartS() > startDistance + searchRange)
        {
            return foundTrafficSigns;
        }
        auto sortedTrafficSigns = lane().GetTrafficSigns();
        sortedTrafficSigns.sort([lane](const OWL::Interfaces::TrafficSign* first, const OWL::Interfaces::TrafficSign* second)
        {return lane.GetStreamPosition(first->GetS()) < lane.GetStreamPosition(second->GetS());});
        for (const auto& trafficSign : sortedTrafficSigns)
        {
            double trafficSignPosition = lane.GetStreamPosition(trafficSign->GetS() - lane().GetDistance(OWL::MeasurementPoint::RoadStart));
            if (trafficSignPosition >= startDistance && trafficSignPosition <= startDistance + searchRange)
            {
                foundTrafficSigns.push_back({trafficSignPosition - startDistance, trafficSign});
            }
        }

    }

    return foundTrafficSigns;
}

std::vector<LaneMarking::Entity> WorldDataQuery::GetLaneMarkings(const LaneStream& laneStream, double startDistance, double range, Side side) const
{
    std::vector<LaneMarking::Entity> laneMarkings;
    std::map<double, LaneMarking::Entity> doubleLaneMarkings;
    std::vector<OWL::Id> laneBoundaries;
    for(const auto& lane : laneStream.GetElements())
    {
        if (lane.EndS() < startDistance)
        {
            continue;
        }
        if (lane.StartS() > startDistance + range)
        {
            break;
        }

        laneBoundaries = (side == Side::Left) ? lane().GetLeftLaneBoundaries() : lane().GetRightLaneBoundaries();

        for (auto laneBoundaryIndex : laneBoundaries)
        {
            const auto& laneBoundary = worldData.GetLaneBoundaries().at(laneBoundaryIndex);
            if (lane.GetStreamPosition(laneBoundary->GetSStart() - lane().GetDistance(OWL::MeasurementPoint::RoadStart)) <= startDistance + range
                    && lane.GetStreamPosition(laneBoundary->GetSEnd() - lane().GetDistance(OWL::MeasurementPoint::RoadStart)) >= startDistance)
            {
                LaneMarking::Entity laneMarking;
                laneMarking.relativeStartDistance = lane.GetStreamPosition(laneBoundary->GetSStart() - lane().GetDistance(OWL::MeasurementPoint::RoadStart)) - startDistance;
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
    }
    return laneMarkings;
}

LaneQueryResult WorldDataQuery::QueryLane(std::string roadId, int laneId, double distance) const
{
    const auto& lane = GetLaneByOdId(roadId, laneId, distance);
    return BuildLaneQueryResult(lane);
}

std::list<LaneQueryResult> WorldDataQuery::QueryLanes(std::string roadId, double startDistance, double endDistance) const
{
    std::list<LaneQueryResult> laneQueryResults;
    for (const auto& road : worldData.GetRoads())
    {
        if (worldData.GetRoadIdMapping().at(road.first) == roadId)
        {
            for (auto section : road.second->GetSections())
            {
                if (section->CoversInterval(startDistance, endDistance))
                {
                    for (const auto& lane : section->GetLanes())
                    {
                        uint64_t streamId = GetStreamId(*lane);
                        auto streamMatcher = [streamId](const LaneQueryResult & queryResult)
                        {
                            return queryResult.streamId == streamId;
                        };

                        if (std::find_if(laneQueryResults.begin(),
                                         laneQueryResults.end(),
                                         streamMatcher) == laneQueryResults.end())
                        {
                            laneQueryResults.push_back(BuildLaneQueryResult(*lane));
                        }
                    }
                }
            }
            continue;
        }
    }

    return laneQueryResults;
}

LaneQueryResult WorldDataQuery::BuildLaneQueryResult(OWL::CLane& lane) const
{
    if (lane.Exists())
    {
        auto streamId = GetStreamId(lane);
        auto limits = GetLimitingDistances(lane);
        auto laneCategory = (lane.GetRightLaneCount() == 0) ?
                            LaneCategory::RightMostLane :
                            LaneCategory::RegularLane ;
        auto isDrivingLane = lane.GetLaneType() == LaneType::Driving;


        return LaneQueryResult(streamId, limits.first, limits.second, laneCategory, isDrivingLane);
    }

    return LaneQueryResult::InvalidResult();
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
            connection.connectingRoadId = worldData.GetRoadIdMapping().at(connectingRoad->GetId());
            connection.outgoingRoadId = worldData.GetRoadIdMapping().at(connectingRoad->GetSuccessor());
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
    auto connectionInfos = junction->GetIntersections().at(connectingRoadId);
    std::transform(std::begin(connectionInfos), std::end(connectionInfos), std::back_inserter(intersections),
        [&](auto connectionInfo){return IntersectingConnection{worldData.GetRoadIdMapping().at(connectionInfo.intersectingRoad), connectionInfo.relativeRank};});
    return intersections;
}

std::vector<JunctionConnectorPriority> WorldDataQuery::GetPrioritiesOnJunction(std::string junctionId) const
{
    std::vector<JunctionConnectorPriority> priorities;
    const auto junction = GetJunctionByOdId(junctionId);
    for (const auto& [high, low] : junction->GetPriorities())
    {
        priorities.push_back({worldData.GetRoadIdMapping().at(high), worldData.GetRoadIdMapping().at(low)});
    }
    return priorities;
}

RoadNetworkElement WorldDataQuery::GetRoadSuccessor(std::string roadId) const
{
    auto currentRoad = GetRoadByOdId(roadId);
    assert(currentRoad);
    OWL::Id nextElementId = currentRoad->GetSuccessor();
    if (worldData.GetRoads().count(nextElementId) > 0)
    {
        return {RoadNetworkElementType::Road, worldData.GetRoadIdMapping().at(nextElementId)};
    }
    if (worldData.GetJunctions().count(nextElementId) > 0)
    {
        return {RoadNetworkElementType::Junction, worldData.GetJunctionIdMapping().at(nextElementId)};
    }
    return {RoadNetworkElementType::None, ""};
}

RoadNetworkElement WorldDataQuery::GetRoadPredecessor(std::string roadId) const
{
    auto currentRoad = GetRoadByOdId(roadId);
    assert(currentRoad);
    OWL::Id nextElementId = currentRoad->GetPredecessor();
    if (worldData.GetRoads().count(nextElementId) > 0)
    {
        return {RoadNetworkElementType::Road, worldData.GetRoadIdMapping().at(nextElementId)};
    }
    if (worldData.GetJunctions().count(nextElementId) > 0)
    {
        return {RoadNetworkElementType::Junction, worldData.GetJunctionIdMapping().at(nextElementId)};
    }
    return {RoadNetworkElementType::None, ""};
}

std::vector<const OWL::Interfaces::WorldObject*> WorldDataQuery::GetMovingObjectsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const
{
    const auto route = GetRouteLeadingToConnector(connectingRoadId);
    const auto& lanes = GetLanesOfLaneTypeAtDistance(connectingRoadId, 0.0, {LaneType::Driving});
    std::vector<const OWL::Interfaces::WorldObject*> foundMovingObjects;

    for (const auto& lane : lanes)
    {
        auto laneStream = CreateLaneStream(route, connectingRoadId, worldData.GetLaneIdMapping().at(lane->GetId()), 0.0);
        double laneStreamEnd = laneStream->GetElements().back().EndS();

        auto movingObjects = GetObjectsOfTypeInRange<OWL::Interfaces::MovingObject>(*laneStream, laneStreamEnd - range, laneStreamEnd);

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

std::vector<RouteElement> WorldDataQuery::GetRouteLeadingToConnector (std::string connectingRoadId) const
{
     auto incomingRoadId = GetRoadPredecessor(connectingRoadId).id;
     bool incomingRoadLeadsToJunction = GetRoadSuccessor(incomingRoadId).id == worldData.GetJunctionIdMapping().at(GetJunctionOfConnector(connectingRoadId)->GetId());
     std::vector<RouteElement> route{{incomingRoadId, incomingRoadLeadsToJunction}};
     bool reachedEndOfRoadStream = false;
     auto emplace_element_if = [&](const RoadNetworkElement& element)
     {
         if (element.type == RoadNetworkElementType::Road)
         {
             route.insert(route.cbegin(), {element.id, incomingRoadLeadsToJunction});
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

     route.push_back({connectingRoadId, true});
     return route;
}

double WorldDataQuery::GetDistanceUntilObjectEntersConnector(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    const OWL::Interfaces::Junction* junction = GetJunctionOfConnector(intersectingConnectorId);
    const auto& intersections = junction->GetIntersections().at(intersectingConnectorId);
    const auto& intersection = std::find_if(intersections.cbegin(), intersections.cend(),
                                            [this, ownConnectorId](const auto& connectionInfo){return worldData.GetRoadIdMapping().at(connectionInfo.intersectingRoad) == ownConnectorId;});
    if (intersection == intersections.cend())
    {
        return std::numeric_limits<double>::max();
    }
    std::vector<RouteElement> route = GetRouteLeadingToConnector(ownConnectorId);
    const auto laneStream = CreateLaneStream(route, position.mainLocatePoint.roadId, position.mainLocatePoint.laneId, position.mainLocatePoint.roadPosition.s);
    const auto& laneOfObject = GetLaneByOdId(position.mainLocatePoint.roadId, position.mainLocatePoint.laneId, position.mainLocatePoint.roadPosition.s);
    double sStartOfObjectOnLaneStream = laneStream->GetPositionByElementAndS(laneOfObject, position.touchedRoads.at(position.mainLocatePoint.roadId).sStart);
    double sEndOfObjectOnLaneStream = laneStream->GetPositionByElementAndS(laneOfObject, position.touchedRoads.at(position.mainLocatePoint.roadId).sEnd);
    double frontOfObjectOnLaneStream = std::max(sStartOfObjectOnLaneStream, sEndOfObjectOnLaneStream);
    const auto& intersectingLane = GetLaneByOdId(intersectingConnectorId, intersectingLaneId, 0.0);
    const auto ownLane = laneStream->GetElements().rbegin()->element;
    double sStart = intersection->sOffsets.at({intersectingLane.GetId(), ownLane->GetId()}).first;
    double positionOfIntersectionOnLaneStream = laneStream->GetPositionByElementAndS(*ownLane, sStart);
    return positionOfIntersectionOnLaneStream - frontOfObjectOnLaneStream;
}

double WorldDataQuery::GetDistanceUntilObjectLeavesConnector(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    const OWL::Interfaces::Junction* junction = GetJunctionOfConnector(intersectingConnectorId);
    const auto& intersections = junction->GetIntersections().at(intersectingConnectorId);
    const auto& intersection = std::find_if(intersections.cbegin(), intersections.cend(),
                                            [this, ownConnectorId](const auto& connectionInfo){return worldData.GetRoadIdMapping().at(connectionInfo.intersectingRoad) == ownConnectorId;});
    if (intersection == intersections.cend())
    {
        return std::numeric_limits<double>::max();
    }
    std::vector<RouteElement> route = GetRouteLeadingToConnector(ownConnectorId);
    const auto laneStream = CreateLaneStream(route, position.mainLocatePoint.roadId, position.mainLocatePoint.laneId, position.mainLocatePoint.roadPosition.s);
    const auto& laneOfObject = GetLaneByOdId(position.mainLocatePoint.roadId, position.mainLocatePoint.laneId, position.mainLocatePoint.roadPosition.s);
    double sStartOfObjectOnLaneStream = laneStream->GetPositionByElementAndS(laneOfObject, position.touchedRoads.at(position.mainLocatePoint.roadId).sStart);
    double sEndOfObjectOnLaneStream = laneStream->GetPositionByElementAndS(laneOfObject, position.touchedRoads.at(position.mainLocatePoint.roadId).sEnd);
    double rearOfObjectOnLaneStream = std::min(sStartOfObjectOnLaneStream, sEndOfObjectOnLaneStream);
    const auto& intersectingLane = GetLaneByOdId(intersectingConnectorId, intersectingLaneId, 0.0);
    const auto ownLane = laneStream->GetElements().rbegin()->element;
    double sEnd = intersection->sOffsets.at({intersectingLane.GetId(), ownLane->GetId()}).second;
    double positionOfIntersectionOnLaneStream = laneStream->GetPositionByElementAndS(*ownLane, sEnd);
    return positionOfIntersectionOnLaneStream - rearOfObjectOnLaneStream;
}

std::shared_ptr<const LaneStream> WorldDataQuery::CreateLaneStream(const std::vector<RouteElement>& route, std::string startRoadId, OWL::OdId startLaneId, double startDistance) const
{
    std::vector<LaneStreamInfo> lanes;
    OWL::CLane* currentLane = GetOriginatingRouteLane(route, startRoadId, startLaneId, startDistance);
    auto currentRoadOdId = worldData.GetRoadIdMapping().at(currentLane->GetRoad().GetId());
    auto routeIterator = std::find_if(route.cbegin(), route.cend(),
                                      [&](const auto& routeElement){return routeElement.roadId == currentRoadOdId;});
    double currentS = 0.0;
    auto currentRoadId = GetRoadByOdId(routeIterator->roadId)->GetId();
    auto currentRoad = worldData.GetRoads().at(currentRoadId);
    while (currentLane)
    {
        bool inStreamDirection = (routeIterator->inRoadDirection);
        LaneStreamInfo laneStreamInfo;
        laneStreamInfo.element = currentLane;
        laneStreamInfo.sOffset = currentS + (inStreamDirection ? 0 : currentLane->GetLength());
        laneStreamInfo.inStreamDirection = inStreamDirection;
        lanes.push_back(laneStreamInfo);
        currentS += currentLane->GetLength();
        const auto& successorLanes = inStreamDirection ? currentLane->GetNext() : currentLane->GetPrevious();
        currentLane = GetLaneOnRoad(currentRoad, successorLanes);
        if (!currentLane) //If successor is not on this road, go to previous road in the route
        {
            ++routeIterator;
            if (routeIterator == route.cend())
            {
                break;
            }
            currentRoadId = GetRoadByOdId(routeIterator->roadId)->GetId();
            currentRoad = worldData.GetRoads().at(currentRoadId);
            currentLane = GetLaneOnRoad(currentRoad, successorLanes);
        }
    }
    return std::make_shared<const LaneStream>(std::move(lanes));
}

std::shared_ptr<const RoadStream> WorldDataQuery::CreateRoadStream(const std::vector<RouteElement>& route) const
{
    double currentS = 0.0;

    std::vector<RoadStreamInfo> roads;
    std::transform(route.cbegin(),
                   route.cend(),
                   std::back_inserter(roads),
                   [&](const auto& routeElement) -> RoadStreamInfo
    {
        RoadStreamInfo roadStreamInfo;
        roadStreamInfo.element = GetRoadByOdId(routeElement.roadId);
        roadStreamInfo.inStreamDirection = routeElement.inRoadDirection;
        roadStreamInfo.sOffset = currentS + (routeElement.inRoadDirection ? 0 : roadStreamInfo.element->GetLength());

        currentS += roadStreamInfo.element->GetLength();
        return roadStreamInfo;
    });
    return std::make_shared<const RoadStream>(std::move(roads));
}

OWL::CLane* WorldDataQuery::GetOriginatingRouteLane(std::vector<RouteElement> route, std::string startRoadId, OWL::OdId startLaneId, double startDistance) const
{
    OWL::CLane* currentLane = &GetLaneByOdId(startRoadId, startLaneId, startDistance);
    auto routeIterator = std::find_if(route.crbegin(), route.crend(),
                                      [&](const auto& routeElement){
        return routeElement.roadId == startRoadId;
    } );
    bool reachedMostUpstreamLane = false;
    bool inStreamDirection = routeIterator->inRoadDirection;
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
            return worldData.GetRoadIdMapping().at(upStreamRoadId) == routeIterator->roadId;
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
                return worldData.GetRoadIdMapping().at(upStreamRoadId) == routeIterator->roadId;
            });
        }
        if (upstreamLane == upstreamLanes.cend()) //none of the predecessors is on the route
        {
            reachedMostUpstreamLane = true;
            break;
        }
        const auto& next = worldData.GetLanes().at(*upstreamLane)->GetNext();
        inStreamDirection = routeIterator->inRoadDirection;
        currentLane = worldData.GetLanes().at(*upstreamLane);
    }
    return currentLane;
}

double WorldDataQuery::GetDistanceBetweenObjects(const RoadStream& roadStream,
                                                 const ObjectPosition& objectPos,
                                                 const ObjectPosition& targetObjectPos) const
{
    // lanestream get position from s and laneid
    // objects I can get the distance from the start of road, lane
    double objectMinPos = std::numeric_limits<double>::max();
    double objectMaxPos = 0;
    bool objectIsOnStream = false;
    for (const auto& [roadId, roadInterval] : objectPos.touchedRoads)
    {
        const auto road = GetRoadByOdId(roadId);
        if (roadStream.Contains(*road))
        {
            const auto positionSMin = roadStream.GetPositionByElementAndS(*road, roadInterval.sStart);
            const auto positionSMax = roadStream.GetPositionByElementAndS(*road, roadInterval.sEnd);
            objectMinPos = std::min({objectMinPos, positionSMin, positionSMax});
            objectMaxPos = std::max({objectMaxPos, positionSMin, positionSMax});
            objectIsOnStream = true;
        }
    }
    if (!objectIsOnStream)
    {
        return std::numeric_limits<double>::max();
    }

    double targetObjectMinPos = std::numeric_limits<double>::max();
    double targetObjectMaxPos = 0;
    objectIsOnStream = false;
    for (const auto& [roadId, roadInterval] : targetObjectPos.touchedRoads)
    {
        const auto road = GetRoadByOdId(roadId);
        if (roadStream.Contains(*road))
        {
            const auto positionSMin = roadStream.GetPositionByElementAndS(*road, roadInterval.sStart);
            const auto positionSMax = roadStream.GetPositionByElementAndS(*road, roadInterval.sEnd);
            targetObjectMinPos = std::min({targetObjectMinPos, positionSMin, positionSMax});
            targetObjectMaxPos = std::max({targetObjectMaxPos, positionSMin, positionSMax});
            objectIsOnStream = true;
        }
    }
    if (!objectIsOnStream)
    {
        return std::numeric_limits<double>::max();
    }

    if (objectMinPos > targetObjectMaxPos)
    {
        return targetObjectMaxPos - objectMinPos;
    }
    else if (targetObjectMinPos > objectMaxPos)
    {
        return targetObjectMinPos - objectMaxPos;
    }
    else
    {
        return 0;
    }
}

std::string WorldDataQuery::GetNextJunctionIdOnRoute(const Route& route, const ObjectPosition& startPos) const
{
    // find current road for startPos
    const auto startPosIter = std::find_if(route.roads.cbegin(),
                                           route.roads.cend(),
                                           [&startPos](const auto& routeElement) -> bool
    {
        for (const auto& touchedRoad : startPos.touchedRoads)
        {
            if (touchedRoad.first == routeElement.roadId) return true;
        }

        return false;
    });

    std::string nextJunctionId;
    // step through each road and see if it connects to a junction - the first junction detected this way is the next junction on route - if route is appropriately configured
    std::find_if(startPosIter,
                 route.roads.cend(),
                 [this, &route, &nextJunctionId](const auto& routeElement) -> bool
    {
        const auto roadOsiId = GetRoadByOdId(routeElement.roadId)->GetId();
        const auto junctionIter = std::find_if(route.junctions.cbegin(),
                                  route.junctions.cend(),
                                  [this, &roadOsiId, &nextJunctionId](const auto& junctionId) -> bool
        {
            const auto junction = GetJunctionByOdId(junctionId);
            const auto& connectingRoads = junction->GetConnectingRoads();
            const auto connectingRoadIter = std::find_if(connectingRoads.cbegin(),
                                                         connectingRoads.cend(),
                                                         [roadOsiId](const auto connectingRoad) -> bool
            {
                return connectingRoad->GetId() == roadOsiId;
            });

            return connectingRoadIter != connectingRoads.cend();
        });

        if (junctionIter != route.junctions.cend())
        {
            nextJunctionId = *junctionIter;
            return true;
        }
        else return false;
    });

    return nextJunctionId;
}

double WorldDataQuery::GetDistanceToJunction(const Route& route, const ObjectPosition& objectPos, const std::string& junctionId) const
{
    if (std::find(route.junctions.cbegin(),
                  route.junctions.cend(),
                  junctionId) == route.junctions.cend())
    {
        return std::numeric_limits<double>::max();
    }
    const auto junction = GetJunctionByOdId(junctionId);

    const auto& connectingRoads = junction->GetConnectingRoads();
    const auto roadStream = CreateRoadStream(route.roads);
    const auto& connectingRoadInfoIter = std::find_if(roadStream->GetElements().cbegin(),
                                                      roadStream->GetElements().cend(),
                                                      [&connectingRoads](const auto& element) -> bool
    {
        return (std::find_if(connectingRoads.cbegin(),
                             connectingRoads.cend(),
                             [element](const auto connectingRoad) -> bool
        {
            const auto id = element().GetId();
            return id == connectingRoad->GetId();
        }) != connectingRoads.cend());
    });
    if (connectingRoadInfoIter == roadStream->GetElements().cend())
    {
        return std::numeric_limits<double>::max();
    }

    const auto objectPosOnStream = roadStream->GetPositionByElementAndS(*GetRoadByOdId(objectPos.mainLocatePoint.roadId),
                                                                       objectPos.mainLocatePoint.roadPosition.s);

    return connectingRoadInfoIter->StartS() - objectPosOnStream;
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

RelativeWorldView::Junctions WorldDataQuery::GetRelativeJunctions(const RoadStream& roadStream, double startPosition, double range) const
{
    RelativeWorldView::Junctions junctions;
    for (const auto& road : roadStream.GetElements())
    {
        if (road.EndS() < startPosition)
        {
            continue;
        }
        if (road.StartS() > startPosition + range)
        {
            return junctions;
        }
        std::string roadId = worldData.GetRoadIdMapping().at(road().GetId());
        auto junction = GetJunctionOfConnector(roadId);
        if (junction)
        {
            double startS = road.StartS() - startPosition;
            double endS = road.EndS() - startPosition;
            junctions.push_back({startS, endS, roadId});
        }
    }
    return junctions;
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
                return worldData.GetLaneIdMapping().at(lane->GetId());
            }
        }
    }
    return 0;
}

std::map<int, OWL::Id> WorldDataQuery::AddLanesOfSection(const OWL::Interfaces::Lanes& lanesOnSection, bool inStreamDirection, int currentOwnLaneId,
                                                         const std::map<int, OWL::Id>& previousSectionLaneIds, std::vector<RelativeWorldView::Lane>& previousSectionLanes,
                                                         RelativeWorldView::LanesInterval& laneInterval) const
{
    std::map<int, OWL::Id> lanesOnSectionLaneIds{};
    for (const auto& lane : lanesOnSection)
    {
        const auto& laneId = worldData.GetLaneIdMapping().at(lane->GetId());
        bool inDrivingDirection = inStreamDirection ? (laneId < 0) : (laneId > 0);
        int relativeLaneId = inStreamDirection ? (laneId - currentOwnLaneId) : (currentOwnLaneId - laneId);
        bool differentSigns = (currentOwnLaneId * laneId < 0) && currentOwnLaneId != 0;
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

RelativeWorldView::Lanes WorldDataQuery::GetRelativeLanes(const RoadStream& roadStream, double startPosition, int startLaneId, double range) const
{
    RelativeWorldView::Lanes lanes;
    std::map<int, OWL::Id> previousSectionLaneIds;
    int currentOwnLaneId = startLaneId;
    for (const auto& road : roadStream.GetElements())
    {
        if (road.EndS() < startPosition)
        {
            continue;
        }
        if (road.StartS() > startPosition + range)
        {
            return lanes;
        }
        for (const auto& section : road().GetSections())
        {
            const double sectionStart = road.GetStreamPosition(section->GetSOffset() + (road.inStreamDirection ? 0 : section->GetLength()));
            const double sectionEnd = road.GetStreamPosition(section->GetSOffset() + (road.inStreamDirection ? section->GetLength() : 0));
            if (sectionEnd < startPosition)
            {
                continue;
            }
            if (sectionStart > startPosition + range)
            {
                return lanes;
            }
            RelativeWorldView::LanesInterval laneInterval;
            laneInterval.startS = sectionStart;
            laneInterval.endS = sectionEnd;
            const auto& lanesOnSection = section->GetLanes();
            if (previousSectionLaneIds.empty())
            {
                currentOwnLaneId = startLaneId;
            }
            else
            {
                currentOwnLaneId = FindNextEgoLaneId(lanesOnSection, road.inStreamDirection, previousSectionLaneIds);
            }
            auto lanesOnSectionLaneIds = AddLanesOfSection(lanesOnSection, road.inStreamDirection, currentOwnLaneId, previousSectionLaneIds, lanes.back().lanes, laneInterval);
            previousSectionLaneIds = lanesOnSectionLaneIds;
            lanes.push_back(laneInterval);
        }
    }
    return lanes;
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

std::pair<bool, Position> WorldDataQuery::CalculatePositionIfOnLane(double sCoordinate, double tCoordinate, const OWL::Interfaces::Lane& lane) const
{
    const double laneStart = lane.GetDistance(OWL::MeasurementPoint::RoadStart);
    const double laneEnd = lane.GetDistance(OWL::MeasurementPoint::RoadEnd);
    if (sCoordinate >= laneStart && sCoordinate <= laneEnd)
    {
        return {true, GetPositionByDistanceAndLane(lane, sCoordinate, tCoordinate)};
    }
    else
    {
        return {false, {}};
    }
}

Obstruction WorldDataQuery::GetObstruction(const LaneStream& laneStream, double tCoordinate, const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners) const
{
    Position firstPoint;
    Position secondPoint;
    bool foundFirstPoint{false};
    bool foundSecondPoint{false};
    for (const auto& lane : laneStream.GetElements())
    {
        const auto it = otherPosition.touchedRoads.find(worldData.GetRoadIdMapping().at(lane().GetRoad().GetId()));
        if (it == otherPosition.touchedRoads.end())
        {
            continue;
        }
        const double objectStart = it->second.sStart;
        const double objectEnd = it->second.sEnd;
        if (lane.inStreamDirection)
        {
            if(!foundFirstPoint)
            {
                std::tie(foundFirstPoint, firstPoint) = CalculatePositionIfOnLane(objectStart, tCoordinate, lane());
            }
            if(!foundSecondPoint)
            {
                std::tie(foundSecondPoint, secondPoint) = CalculatePositionIfOnLane(objectEnd, tCoordinate, lane());
            }
        }
        else
        {
            if(!foundFirstPoint)
            {
                std::tie(foundFirstPoint, firstPoint) = CalculatePositionIfOnLane(objectEnd, -tCoordinate, lane());
            }
            if(!foundSecondPoint)
            {
                std::tie(foundSecondPoint, secondPoint) = CalculatePositionIfOnLane(objectStart, -tCoordinate, lane());
            }
        }
        if (foundFirstPoint && foundSecondPoint)
        {
            break;
        }
    }
    if (!foundFirstPoint || !foundSecondPoint)
    {
        return Obstruction::Invalid();
    }

    double leftDistance{std::numeric_limits<double>::lowest()};
    double rightDistance{std::numeric_limits<double>::max()};
    for (const auto& corner : objectCorners)
    {
        double distance = CalculatePerpendicularDistance(corner, Common::Line{{firstPoint.xPos, firstPoint.yPos}, {secondPoint.xPos, secondPoint.yPos}});
        leftDistance = std::max(leftDistance, distance);
        rightDistance = std::min(rightDistance, distance);
    }
    return {leftDistance, rightDistance};
}

// explicit template instantiation
template class Stream<OWL::Interfaces::Lane>;
template class Stream<OWL::Interfaces::Road>;
