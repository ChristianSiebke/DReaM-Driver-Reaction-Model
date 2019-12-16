/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include <numeric>
#include <utility>
#include <tuple>
#include "OWL/DataTypes.h"
#include "WorldData.h"
#include <numeric>
#include <algorithm>

//! This class represents one element of a Stream. StreamInfo can contain elements of the following types:
//! OWL::Lane
//! OWL::Road
template<typename T>
struct StreamInfo
{
    const T* element;  //!element represented by this object
    double sOffset;         //!S Offset of the start point of the element from the beginning of the stream
    bool inStreamDirection; //!Specifies whether the direction of the element is the same as the direction of the stream

    const T& operator()() const
    {
        return *element;
    }

    //! Transform the s coordinate on the element to the s coordinate on the element stream
    //!
    //! \param elementPosition position relative to the start of the element
    //! \return position relative to the start of the element stream
    double GetStreamPosition (double elementPosition) const
    {
        return sOffset + (inStreamDirection ? elementPosition : -elementPosition);
    }

    //! Returns the element stream position of the start of lane
    double StartS () const
    {
        return sOffset - (inStreamDirection ? 0 : element->GetLength());
    }

    //! Returns the element stream position of the end of lane
    double EndS () const
    {
        return sOffset + (inStreamDirection ? element->GetLength() : 0);
    }
};

//! This class represent a consecutive sequence of elements.
//! It is used by various queries to search along consecutive elements within a range starting at some start point
//! The elements supported by this class are:
//! OWL::Lane*
//! OWL::Road*
template<typename T>
class Stream
{
public:
    Stream() = default;
    explicit Stream(const std::vector<StreamInfo<T>>& elements) :
        elements(elements)
    {}

    const std::vector<StreamInfo<T>>& GetElements() const
    {
        return elements;
    }

    //! Returns an element stream that has the same lanes in reverse order
    Stream<T> Reverse() const;

    //! Transform the sCoordinate on the given element to a position on the stream
    //! The element must be part of the stream
    double GetPositionByElementAndS(const T& element, double sCoordinate) const;

    //! Returns the element and s coordinate corresponding to the given position on the stream
    std::pair<double, const T*> GetElementAndSByPosition(double position) const;

    //! Returns true if the specified element is contained in this element stream, false otherwise
    bool Contains(const T& element) const;

private:
    std::vector<StreamInfo<T>> elements;
};

using LaneStreamInfo = StreamInfo<OWL::Interfaces::Lane>;
using LaneStream = Stream<OWL::Interfaces::Lane>;
using RoadStreamInfo = StreamInfo<OWL::Interfaces::Road>;
using RoadStream = Stream<OWL::Interfaces::Road>;

//! Helper class for complex queries on the world data
class WorldDataQuery
{
public:
    WorldDataQuery(const OWL::Interfaces::WorldData& worldData);

    //! Checks if object is of type T and within the specified range when supplied an offset
    //! Returns true if so; false, otherwise
    //!
    //! \param worldObject  object to check
    //! \param minS         minimum valid s position within the road
    //! \param maxS         maximum valid s position within the road
    //! \param lane         lane on which the object is
    template<typename T>
    bool ObjectIsOfTypeAndWithinRange(const OWL::Interfaces::WorldObject * const worldObject, const double minS, const double maxS, const LaneStreamInfo& lane) const
    {
        const auto& roadId = worldData.GetRoadIdMapping().at(lane.element->GetRoad().GetId());
        return (worldObject->Is<T>() &&
                lane.GetStreamPosition(worldObject->GetDistance(OWL::MeasurementPoint::RoadEnd, roadId) - lane.element->GetDistance(OWL::MeasurementPoint::RoadStart)) > minS &&
                lane.GetStreamPosition(worldObject->GetDistance(OWL::MeasurementPoint::RoadStart, roadId) - lane.element->GetDistance(OWL::MeasurementPoint::RoadStart)) < maxS);
    }

    //! Searches along a lane stream starting at initialSearchDistance and returns the first object
    //! found that is of type T. Returns a nullptr if there is no such object within maxSearchLength
    //! T can be one of WorldObject, MovingObject or StationaryObject
    template <typename T>
    OWL::Interfaces::WorldObject* GetNextObjectInLane(const LaneStream& laneStream, double initialSearchDistance,
            double maxSearchLength) const
    {
        const auto& lanesOnStream = laneStream.GetElements();
        for (const auto& lane : lanesOnStream)
        {
            if (lane.EndS() < initialSearchDistance) //Ignore lanes ending before initialSearchDistance
            {
                continue;
            }
            if (lane.StartS() > initialSearchDistance + maxSearchLength) //Stop if reached maxSearchLength
            {
                break;
            }
            const auto& roadId = worldData.GetRoadIdMapping().at(lane.element->GetRoad().GetId());
            double positionOfFoundObject = std::numeric_limits<double>::infinity();
            OWL::Interfaces::WorldObject* foundObject{nullptr};
            for (const auto& object : lane.element->GetWorldObjects())
            {
                if (ObjectIsOfTypeAndWithinRange<T>(object, initialSearchDistance, initialSearchDistance + maxSearchLength, lane))
                {
                    if (lane.GetStreamPosition(object->GetDistance(OWL::MeasurementPoint::RoadStart, roadId) - lane.element->GetDistance(OWL::MeasurementPoint::RoadStart)) < positionOfFoundObject)
                    {
                        foundObject = object;
                        positionOfFoundObject = lane.GetStreamPosition(object->GetDistance(OWL::MeasurementPoint::RoadStart, roadId));
                    }
                }
            }
            if (foundObject)
            {
                return foundObject;
            }
        }
        return nullptr;
    }

    //! Searches along a lane stream in reverse direction starting at initialSearchDistance + maxSearchLength and returns the first object
    //! found that is of type T. Returns a nullptr if there is no such object within maxSearchLength
    //! T can be one of WorldObject, MovingObject or StationaryObject
    template <typename T>
    OWL::Interfaces::WorldObject* GetLastObjectInLane(const LaneStream& laneStream, double initialSearchDistance,
            double maxSearchLength) const
    {
        const auto& lanesOnStream = laneStream.GetElements();
        for (auto lane = lanesOnStream.crbegin(); lane != lanesOnStream.crend(); ++lane)
        {
            if (lane->StartS() > initialSearchDistance + maxSearchLength) //Ignore lanes behind maxSearchLength
            {
                continue;
            }
            if (lane->EndS() < initialSearchDistance) //Stop if reached initialSearchDistance
            {
                break;
            }
            const auto& roadId = worldData.GetRoadIdMapping().at(lane->element->GetRoad().GetId());
            double positionOfFoundObject = -std::numeric_limits<double>::infinity();
            OWL::Interfaces::WorldObject* foundObject{nullptr};
            for (const auto& object : lane->element->GetWorldObjects())
            {
                if (ObjectIsOfTypeAndWithinRange<T>(object, initialSearchDistance, initialSearchDistance + maxSearchLength, *lane))
                {
                    if (lane->GetStreamPosition(object->GetDistance(OWL::MeasurementPoint::RoadStart, roadId) - lane->element->GetDistance(OWL::MeasurementPoint::RoadStart)) > positionOfFoundObject)
                    {
                        foundObject = object;
                        positionOfFoundObject = lane->GetStreamPosition(object->GetDistance(OWL::MeasurementPoint::RoadStart, roadId));
                    }
                }
            }
            if (foundObject)
            {
                return foundObject;
            }
        }
        return nullptr;
    }

    //! Searches along a lane stream starting at initialSearchDistance and returns the all objects
    //! found until initialSearchDistance + maxSearchLength that are of type T.
    //! Returns an empty vector if there is no such object within maxSearchLength
    //! T can be one of WorldObject, MovingObject or StationaryObject
    template<typename T>
    std::vector<const OWL::Interfaces::WorldObject*> GetObjectsOfTypeInRange(const LaneStream& laneStream,
                                                                             const double startDistance,
                                                                             const double endDistance) const
    {
        const auto& lanesOnStream = laneStream.GetElements();
        std::vector<const OWL::Interfaces::WorldObject*> foundObjects;
        for (const auto& lane : lanesOnStream)
        {
            if (lane.EndS() < startDistance) //Ignore lanes ending before startDistance
            {
                continue;
            }
            if (lane.StartS() > endDistance) //Stop if reached endDistance
            {
                break;
            }
            const auto& roadId = worldData.GetRoadIdMapping().at(lane.element->GetRoad().GetId());
            auto sortedObjects = lane.element->GetWorldObjects();
            if (lane.inStreamDirection)
            {
                sortedObjects.sort(
                                [lane, roadId](OWL::Interfaces::WorldObject* first, OWL::Interfaces::WorldObject* second)
                                {
                                    return first->GetDistance(OWL::MeasurementPoint::RoadStart, roadId) < second->GetDistance(OWL::MeasurementPoint::RoadStart, roadId);
                                }
                                );
            }
            else
            {
                sortedObjects.sort(
                                [lane, roadId](OWL::Interfaces::WorldObject* first, OWL::Interfaces::WorldObject* second)
                                {
                                    return first->GetDistance(OWL::MeasurementPoint::RoadStart, roadId) > second->GetDistance(OWL::MeasurementPoint::RoadStart, roadId);
                                }
                                );
            }
            for (const auto object : sortedObjects)
            {
                if (ObjectIsOfTypeAndWithinRange<T>(object, startDistance, endDistance, lane))
                {
                    if (std::count(foundObjects.begin(), foundObjects.end(), object) == 0)
                    {
                        foundObjects.push_back(object);
                    }
                }
            }
        }
        return foundObjects;
    }

    //! Iterates over a LaneStream until either the type of the next lane does not match one of the specified LaneTypes
    //! or maxSearchLength is reached. Returns the relative distance to the end of last matching lane.
    //! Returns INFINITY if end of lane is outside maxSearchLength.
    //! Returns 0 if lane does not exist or LaneType does not match at initialSearchDistance
    //!
    //! @param laneStream lane stream to search along
    //! @param id OpenDrive Id of lane
    //! @param initialSearchPosition start s-coordinate
    //! @param maxSearchLength maxmium look ahead distance
    //! @param requestedLaneTypes filter of LaneTypes
    double GetDistanceToEndOfLane(const LaneStream& laneStream, double initialSearchPosition, double maxSearchLength,
                                  std::list<LaneType> requestedLaneTypes) const;

    //! Checks if given s-coordinate is valid for specified laneId.
    //!
    //! @param roadId OpenDrive id of road
    //! @param laneId OpenDrive Id of lane
    //! @param distance s-coordinate
    bool IsSValidOnLane(std::string roadId, OWL::OdId laneId, double distance);

    //! Returns number of lanes at given s-coordinate
    //!
    //! @param roadId OpenDrive id of road
    //! @param distance s-coordinate
    int GetNumberOfLanes(std::string roadId, double distance);

    //! Returns lane at specified distance.
    //! Returns InvalidLane if there is no lane at given distance and OpenDriveId
    //!
    //! @param roadId OpenDrive id of road
    //! @param odLaneId OpendDrive Id of Lane
    //! @param distance s-coordinate
    OWL::CLane& GetLaneByOdId(std::string odRoadId, OWL::OdId odLaneId, double distance) const;

    //! Returns section at specified distance.
    //! Returns nullptr if there is no section at given distance
    //!
    //! @param odRaodId ID of road in OpenDrive
    //! @param distance s-coordinate
    OWL::CSection* GetSectionByDistance(std::string odRoadId, double distance) const;

    //! Returns the OWL road with the specified OpenDrive id
    OWL::CRoad *GetRoadByOdId(std::string odRoadId) const;

    //! Returns the junction with the specified OpenDrive id
    const OWL::Interfaces::Junction *GetJunctionByOdId(const std::string &odJunctionId) const;

    //! Returns the junction that the specified Connector is part of
    const OWL::Interfaces::Junction *GetJunctionOfConnector(const std::string &connectingRoadId) const;

    //! Returns all lanes of given LaneType at specified distance.
    //!
    //! @param distance s-coordinate
    //! @param requestedLaneTypes filter of laneTypes
    OWL::CLanes GetLanesOfLaneTypeAtDistance(std::string roadId, double distance,
            std::list<LaneType> requestedLaneTypes) const;

    //! Returns next s where there is a valid lane with given laneId.
    //! Returns INFINITY if no valid is s is found.
    //! Internally IsSValidOnLane is called at initialDistance + n*stepSizeLookingForValidS to probe for a valid s.
    //! OWL::EVENTHORIZON is used as maximum look ahead distance
    //!
    //! @param laneId OpenDrive id of lane
    //! @param initialDistance s-coordinate
    //! @param stepSizeLookingForValidS step-size for probing for valid s
    double GetNextValidSOnLaneInDownstream(std::string roadId, OWL::OdId laneId, double initialDistance,
                                           double stepSizeLookingForValidS) ;

    //! Returns largest s not larger than initialDistance where there is a valid lane with given laneId.
    //! Returns -INFINITY if no valid is s is found.
    //! Internally IsSValidOnLane is called at initialDistance - n*stepSizeLookingForValidS to probe for a valid s.
    //! OWL::EVENTHORIZON is used as maximum look ahead distance
    //!
    //! @param laneId OpenDrive id of lane
    //! @param initialDistance s-coordinate
    //! @param stepSizeLookingForValidS step-size for probing for valid s
    double GetLastValidSInUpstream(std::string roadId, OWL::OdId laneId, double initialDistance,
                                   double stepSizeLookingForValidS) ;

    //! Returns true if a lane at given distance with LaneType::Driving exists on specified side.
    //!
    //! @param laneId OpenDrive id of lane
    //! @param distance s-coordinate
    //! @param side side to check (left/right)
    bool ExistsDrivingLaneOnSide(std::string roadId, OWL::OdId laneId, double distance, Side side);

    //! Returns OSI Id of last lane in ReverseLaneStream.
    //! Returns -999 for invalid lane
    OWL::Id GetStreamId(OWL::CLane& lane) const;

    //! Returns start and end s-coordinate of lane stream
    std::pair<double, double> GetLimitingDistances(OWL::CLane& lane) const;

    //! Returns all TrafficSigns valid for the lanes in LaneStream within startDistance and startDistance + searchRange
    //!
    //! @param laneStream lane stream to search in
    //! @param startDistance s-coordinate of search start
    //! @param searchRange  range of search
    std::vector<std::pair<double, OWL::Interfaces::TrafficSign *>> GetTrafficSignsInRange(LaneStream laneStream, double startDistance, double searchRange) const;


    //! Retrieves all lane markings within the given range on the given side of the lane inside the range
    //!
    //! \param laneStream       lane stream to search in
    //! \param startDistance    s coordinate
    //! \param range            search range
    //! \param side             side of the lane
    std::vector<LaneMarking::Entity> GetLaneMarkings(const LaneStream &laneStream, double startDistance, double range, Side side) const;

    LaneQueryResult QueryLane(std::string roadId, int laneId, double distance) const;

    std::list<LaneQueryResult> QueryLanes(std::string roadId, double startDistance, double endDistance) const;

    LaneQueryResult BuildLaneQueryResult(OWL::CLane& lane) const;

    std::vector<JunctionConnection> GetConnectionsOnJunction(std::string junctionId, std::string incomingRoadId) const;

    //! Returns all intersections of the specified connector with other connectors in the junction
    //!
    //! \param connectingRoadId OpenDrive id of connector
    //! \return intersections of connecting road with other roads
    //!
    std::vector<IntersectingConnection> GetIntersectingConnections(std::string connectingRoadId) const;

    //! Returns all priorities between the connectors of a junction
    //!
    //! \param junctionId   OpenDrive id of the junction
    std::vector<JunctionConnectorPriority> GetPrioritiesOnJunction(std::string junctionId) const;
    
    RoadNetworkElement GetRoadSuccessor(std::string roadId) const;

    RoadNetworkElement GetRoadPredecessor(std::string roadId) const;

    //! Returns all moving objects on the specified connector and the lanes leading to this connector inside
    //! a certain range
    //!
    //! \param connectingRoadId OpenDrive id of the connector
    //! \param range            Search range measured backwards from the end of the connector
    //! \return moving objects in search range on connecting and incoming road
    std::vector<const OWL::Interfaces::WorldObject*> GetMovingObjectsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const;

    //! Returns the distance from the front of the specified to the first intersection point of its road with the specified connector.
    //! If the object is not yet on the junction all possible route the object can take are considered and the minimum distance is returned
    //!
    //! \param object           object to calculate the distance for
    //! \param connectingRoadId OpenDrive id of connector intersecting with the route of the object
    //!
    //! \return distance of object to first intersection with connecting road
    double GetDistanceUntilObjectEntersConnector(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const;

    //! Returns the distance from the rear of the specified to the last intersection point of its road with the specified connector.
    //! If the object is not yet on the junction all possible route the object can take are considered and the maximum distance is returned
    //!
    //! \param object           object to calculate the distance for
    //! \param connectingRoadId OpenDrive id of connector intersecting with the route of the object
    //!
    //! \return distance of rear of object to last intersection with connecting road
    double GetDistanceUntilObjectLeavesConnector(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const;

    //! Creates the longest possible laneStream along the given route that contains the given startLane
    //!
    //! \param route        OpenDrive ids of the roads along which the laneStream should flow
    //! \param startRoadId  OpenDrive id of the road of the startLane
    //! \param startLaneId  OpenDrive id of the startLane
    //! \param startDistance    s coordinate on the startLane
    //! \return
    //!
    std::shared_ptr<const LaneStream> CreateLaneStream(const std::vector<RouteElement>& route, std::string startRoadId, OWL::OdId startLaneId, double startDistance) const;

    //! \brief Creates a RoadStream from the given route
    //!
    //! \param route    Route containing the OpenDrive ids of the Roads along which the roadStream should flow
    //!
    //! \return a RoadStream across the Roads specified in route
    //!
    std::shared_ptr<const RoadStream> CreateRoadStream(const std::vector<RouteElement>& route) const;

    //! \brief GetDistanceBetweenObjects gets the distance between two ObjectPositions on a RoadStream
    //!
    //! \param roadStream the RoadStream in which to get the distance between two ObjectPositions
    //! \param object the first object from whom the distance is calculated
    //! \param targetObject the target object from whom the distance is calculated (if this object is behind, the distance is negative)
    //!
    //! \return the distance between object and targetObject on roadStream
    double GetDistanceBetweenObjects(const RoadStream& roadStream,
                                     const ObjectPosition& object,
                                     const ObjectPosition& targetObject) const;

    //! \brief GetNextJunctionIdOnRoute gets the OdId of the next Junction on route
    //! 
    //! \param route the Route on which to get the next Junction's OdId
    //! \param startPos the ObjectPosition from which to find the next Junction
    //! 
    //! \return the OdId of the first Junction after startPos on route
    std::string GetNextJunctionIdOnRoute(const Route& route, const ObjectPosition& startPos) const;

    //! \brief GetDistanceToJunction gets the distance to the Junction with junctionId from objectPos on route
    //!
    //! \param route the Route along which to calculate the distance
    //! \param objectPos the objectPos from which to find the distance to the Junction with junctionId
    //! \param junctionId the OdId of the Junction to whom the distance is calculated
    //!
    //! \return the distance from objectPos to the Junction with junctionId
    double GetDistanceToJunction(const Route& route, const ObjectPosition& objectPos, const std::string& junctionId) const;

    //! Calculates the obstruction with an object i.e. how far to left or the right the object is from my position
    //! For more information see the [markdown documentation](\ref dev_framework_modules_world_getobstruction)
    //!
    //! \param laneStream       own lane stream
    //! \param tCoordinate      own t coordinate
    //! \param otherPosition    position of the other object
    //! \param objectCorners    corners of the other object
    //! \return obstruction with other object
    Obstruction GetObstruction(const LaneStream& laneStream, double tCoordinate, const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners) const;

    //! Returns the world position that corresponds to a position on a lane
    //!
    //! \param lane             lane in the network
    //! \param distanceOnLane   s coordinate on the lane
    //! \param offset           t coordinate on the lane
    Position GetPositionByDistanceAndLane(const OWL::Interfaces::Lane& lane, double distanceOnLane, double offset) const;

    //! Returns the relative distances (start and end) and the connecting road id of all junctions on the road stream in range
    //!
    //! \param roadStream       road stream to search
    //! \param startPosition    start search position on the road stream
    //! \param range            range of search
    //! \return information about all junctions in range
    RelativeWorldView::Junctions GetRelativeJunctions (const RoadStream& roadStream, double startPosition, double range) const;

    //! Returns information about all lanes on the roadStream in range. These info are the relative distances (start and end),
    //! the laneId relative to the ego lane, the successors and predecessors if existing and the information wether the intended
    //! driving direction of the lane is the same as the direction of the roadStream. If the ego lane prematurely ends, then
    //! the further lane ids are relative to the middle of the road.
    //!
    //! \param roadStream       road stream to search
    //! \param startPosition    start search position on the road stream
    //! \param startLaneId      ego lane id
    //! \param range            range of search
    //! \return information about all lanes in range
    RelativeWorldView::Lanes GetRelativeLanes (const RoadStream& roadStream, double startPosition, int startLaneId, double range) const;

private:
    const OWL::Interfaces::WorldData& worldData;

    //! Returns the most upstream lane on the specified route such that there is a continous stream of lanes regarding successor/predecessor relation
    //! up to the start lane
    OWL::CLane* GetOriginatingRouteLane(std::vector<RouteElement> route, std::string startRoadId, OWL::OdId startLaneId, double startDistance) const;
    
    OWL::CRoad* GetOriginatingRouteRoad(const std::vector<std::string>& route, const std::string& startRoadId, const OWL::OdId startLaneId, const double startDistance) const;

    //! Returns the ids of the stream of roads leading the connecting road including the connecting road itself
    std::vector<RouteElement> GetRouteLeadingToConnector(std::string connectingRoadId) const;

    //! Returns the WorldPosition corresponding to the (s,t) position on the lane, if s is valid on the lane
    //! Otherwise the bool in the pair is false
    std::pair<bool, Position> CalculatePositionIfOnLane(double sCoordinate, double tCoordinate, const OWL::Interfaces::Lane& lane) const;

    int FindNextEgoLaneId(const OWL::Interfaces::Lanes& lanesOnSection, bool inStreamDirection, std::map<int, OWL::Id> previousSectionLaneIds) const;

    std::map<int, OWL::Id> AddLanesOfSection(const OWL::Interfaces::Lanes& lanesOnSection, bool inStreamDirection,
                                             int currentOwnLaneId, const std::map<int, OWL::Id>& previousSectionLaneIds,
                                             std::vector<RelativeWorldView::Lane>& previousSectionLanes, RelativeWorldView::LanesInterval& laneInterval) const;
};
