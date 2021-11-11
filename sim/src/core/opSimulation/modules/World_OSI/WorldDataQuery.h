/********************************************************************************
 * Copyright (c) 2018-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include <numeric>
#include <utility>
#include <tuple>
#include "OWL/DataTypes.h"
#include "WorldData.h"
#include "RoadStream.h"
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

    StreamInfo () = default;

    StreamInfo (const T* element, double sOffset, bool inStreamDirection) :
        element(element),
        sOffset(sOffset),
        inStreamDirection(inStreamDirection)
    {}

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

    double GetElementPosition (double streamPosition) const
    {
        return inStreamDirection ? streamPosition - sOffset : sOffset - streamPosition;
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
using RoadStreamInfo = StreamInfo<OWL::Interfaces::Road>;

//! This class represent a directed tree of elements.
//! It is used by various queries to search along consecutive elements within a range starting at some start point
//! The elements supported by this class are:
//! OWL::Lane*
//! OWL::Road*
template<typename T>
class MultiStream
{
public:
    template<typename Result, typename ... Intermediary>
    using TraversedFunction = std::function<std::tuple<Result, Intermediary ...> (const StreamInfo<T>&, const Result&, const Intermediary& ...)>;

    struct Node
    {
        std::optional<StreamInfo<T>> element;
        std::vector<Node> next;
        RoadGraphVertex roadGraphVertex;

        template<typename Result, typename ... Intermediary>
        void Traverse (TraversedFunction<Result, Intermediary...> function,
                       const Result& previousResult,
                       const Intermediary& ... intermediaryResults,
                       RouteQueryResult<Result>& queryResult,
                       const OWL::Interfaces::WorldData& worldData) const
        {
            if (element.has_value())
            {
                auto result = function(element.value(), previousResult, intermediaryResults...);
                queryResult[roadGraphVertex] = std::get<Result>(result);
                for (const auto& successor : next)
                {
                    successor.template Traverse<Result, Intermediary...>(function, std::get<Result>(result), std::get<Intermediary>(result)..., queryResult, worldData);
                }
            }
            else
            {
                queryResult[roadGraphVertex] = previousResult;
                for (const auto& successor : next)
                {
                    successor.template Traverse<Result, Intermediary...>(function, previousResult, intermediaryResults..., queryResult, worldData);
                }
            }
        }

        const Node* FindVertex (const RoadGraphVertex& vertex) const
        {
            if (roadGraphVertex == vertex)
            {
                return this;
            }
            for (const auto& successor : next)
            {
                if (auto foundNode = successor.FindVertex(vertex))
                {
                    return foundNode;
                }
            }
            return nullptr;
        }
    };

    MultiStream() = default;
    explicit MultiStream(const Node& root) :
        root(root)
    {}

   const Node& GetRoot() const
   {
       return root;
   }

   template<typename Result, typename ... Intermediary>
   RouteQueryResult<Result> Traverse (TraversedFunction<Result, Intermediary...> function,
                                      const Result& zeroResult,
                                      const Intermediary&... defaultIntermediaryResults,
                                      const OWL::Interfaces::WorldData& worldData) const
   {
       RouteQueryResult<Result> result;
       root.template Traverse<Result, Intermediary...>(function, zeroResult, defaultIntermediaryResults..., result, worldData);
       return result;
   }

   double GetPositionByVertexAndS(const RoadGraphVertex& vertex, double sCoordinate) const
   {
       const Node* node = root.FindVertex(vertex);
       if (!node)
       {
           throw std::runtime_error("Cannot find vertex in multistream");
       }
       if (!node->element.has_value())
       {
           return std::numeric_limits<double>::lowest();
       }
       const auto& element = node->element.value();
       auto distance = element.element->GetDistance(OWL::MeasurementPoint::RoadStart);
       return element.GetStreamPosition(sCoordinate - distance);
   }

private:
    Node root;
};

using LaneMultiStream = MultiStream<OWL::Interfaces::Lane>;
using RoadMultiStream = MultiStream<OWL::Interfaces::Road>;

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
        const auto& roadId = lane.element->GetRoad().GetId();
        return (worldObject->Is<T>() &&
                lane.GetStreamPosition(worldObject->GetDistance(OWL::MeasurementPoint::RoadEnd, roadId) - lane.element->GetDistance(OWL::MeasurementPoint::RoadStart)) > minS &&
                lane.GetStreamPosition(worldObject->GetDistance(OWL::MeasurementPoint::RoadStart, roadId) - lane.element->GetDistance(OWL::MeasurementPoint::RoadStart)) < maxS);
    }

    //! Searches along a lane multi stream starting at startDistance and returns the all objects
    //! found until endDistance that are of type T.
    //! Returns an empty vector if there is no such object within the range
    //! The result is returned for every node.
    //! T can be one of WorldObject, MovingObject or StationaryObject
    template<typename T>
    RouteQueryResult<std::vector<const OWL::Interfaces::WorldObject*>> GetObjectsOfTypeInRange(const LaneMultiStream& laneStream,
                                                                                               const double startDistance,
                                                                                               const double endDistance) const
    {
        return laneStream.Traverse<std::vector<const OWL::Interfaces::WorldObject*>>(
            LaneMultiStream::TraversedFunction<std::vector<const OWL::Interfaces::WorldObject*>>(
                [&](const LaneStreamInfo& laneStreamElement, const std::vector<const OWL::Interfaces::WorldObject*>& previousOjects)
                {
                    std::vector<const OWL::Interfaces::WorldObject*> foundObjects{previousOjects};

                    if (laneStreamElement.EndS() < startDistance)
                    {
                        return foundObjects;
                    }

                    if (laneStreamElement.StartS() > endDistance)
                    {
                        return foundObjects;
                    }

                    const auto& roadId = laneStreamElement.element->GetRoad().GetId();

                    const auto streamDirection = laneStreamElement.inStreamDirection;
                    const auto s_lanestart = laneStreamElement.element->GetDistance(OWL::MeasurementPoint::RoadStart);

                    for (const auto& [laneOverlap, object] : laneStreamElement.element->GetWorldObjects(streamDirection))
                    {
                        const auto s_min = streamDirection ? laneOverlap.s_min : laneOverlap.s_max;
                        const auto s_max = streamDirection ? laneOverlap.s_max : laneOverlap.s_min;

                        auto streamPositionStart = laneStreamElement.GetStreamPosition(s_min - s_lanestart);
                        if (streamPositionStart > endDistance)
                        {
                            break;
                        }

                        auto streamPositionEnd = laneStreamElement.GetStreamPosition(s_max - s_lanestart);
                        if (dynamic_cast<const T*>(object) && streamPositionEnd >= startDistance)
                        {
                            if (std::find(foundObjects.crbegin(), foundObjects.crend(), object) == foundObjects.crend())
                            {
                                foundObjects.push_back(object);
                            }
                        }
                    }

                    return foundObjects;
                }),
                {},
                worldData);
    }

    //! Iterates over a LaneMultStream until either the type of the next lane does not match one of the specified LaneTypes
    //! or maxSearchLength is reached. Returns the relative distance to the end of last matching lane.
    //! The result is returned for every node.
    //! Returns INFINITY if end of lane is outside maxSearchLength.
    //! Returns 0 if lane does not exist or LaneType does not match at initialSearchDistance
    //!
    //! @param laneStream               lane stream to search along
    //! @param initialSearchPosition    start position in stream
    //! @param maxSearchLength          maxmium look ahead distance
    //! @param requestedLaneTypes       filter of LaneTypes
    RouteQueryResult<double> GetDistanceToEndOfLane(const LaneMultiStream& laneStream, double initialSearchPosition, double maxSearchLength,
                                                    const std::vector<LaneType>& requestedLaneTypes) const;

    //! Checks if given s-coordinate is valid for specified laneId.
    //!
    //! @param roadId OpenDrive id of road
    //! @param laneId OpenDrive Id of lane
    //! @param distance s-coordinate
    bool IsSValidOnLane(std::string roadId, OWL::OdId laneId, double distance);

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

    //! Returns all TrafficSigns valid for the lanes in the LaneMultiStream within startDistance and startDistance + searchRange
    //! The result is returned for every node.
    //!
    //! @param laneStream       lane stream to search in
    //! @param startDistance    start position in stream
    //! @param searchRange      range of search (positive)
    RouteQueryResult<std::vector<CommonTrafficSign::Entity>> GetTrafficSignsInRange(LaneMultiStream laneStream, double startDistance, double searchRange) const;

    //! Returns all RoadMarkings valid for the lanes in the LaneMultiStream within startDistance and startDistance + searchRange
    //! The result is returned for every node.
    //!
    //! @param laneStream       lane stream to search in
    //! @param startDistance    start position in stream
    //! @param searchRange      range of search (positive)
    RouteQueryResult<std::vector<CommonTrafficSign::Entity>> GetRoadMarkingsInRange(LaneMultiStream laneStream, double startDistance, double searchRange) const;

    //! Returns all TrafficLights valid for the lanes in the LaneMultiStream within startDistance and startDistance + searchRange
    //! The result is returned for every node.
    //!
    //! @param laneStream       lane stream to search in
    //! @param startDistance    start position in stream
    //! @param searchRange      range of search (positive)
    RouteQueryResult<std::vector<CommonTrafficLight::Entity>> GetTrafficLightsInRange(LaneMultiStream laneStream, double startDistance, double searchRange) const;

    //! Retrieves all lane markings within the given range on the given side of the lane inside the range
    //! The result is returned for every node.
    //!
    //! \param laneStream       lane stream to search in
    //! \param startDistance    start position in stream
    //! \param range            search range
    //! \param side             side of the lane
    RouteQueryResult<std::vector<LaneMarking::Entity>> GetLaneMarkings(const LaneMultiStream &laneStream, double startDistance, double range, Side side) const;

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

    //! Creates a LaneMultiStream that contains the OWL lanes out of a roadGraph.
    //!
    //! \param roadGraph        road graph to convert, must be a tree
    //! \param start            root of the tree
    //! \param startLaneId      OpenDrive id of the lane at the root, where the lane stream should start
    //! \param startDistance    s coordinate at the root, where the lane stream should start
    //! \return     LaneMultiStream starting with the specified lane and containing all consecutive lanes that are also contained in the given road graph
    std::shared_ptr<const LaneMultiStream> CreateLaneMultiStream(const RoadGraph& roadGraph, RoadGraphVertex start, OWL::OdId startLaneId, double startDistance) const;

    //! \brief Creates a RoadStream from the given route
    //!
    //! \param route    Route containing the OpenDrive ids of the Roads along which the roadStream should flow
    //!
    //! \return a RoadStream across the Roads specified in route
    //!
    std::unique_ptr<RoadStream> CreateRoadStream(const std::vector<RouteElement>& route) const;

    //! Creates a RoadMultiStream that contains the OWL roads out of a roadGraph.
    //!
    //! \param roadGraph        road graph to convert, must be a tree
    //! \param start            root of the tree
    //! \return     LaneMultiStream starting with the specified road and containing all consecutive roads that are also contained in the given road graph
    std::shared_ptr<const RoadMultiStream> CreateRoadMultiStream(const RoadGraph& roadGraph, RoadGraphVertex start) const;

    //! \brief GetDistanceBetweenObjects gets the distance between two ObjectPositions on a RoadStream
    //!
    //! \param[in]    roadStream         RoadStream in which to get the distance between two ObjectPositions
    //! \param[in]    objectPos          Object from whom the distance is calculated
    //! \param[in]    objectReferenceS   Reference point offset of the target object
    //! \param[in]    targetObject       Target object from whom the distance is calculated (if this object is behind, the distance is negative)
    //!
    //! \return The distance between object and targetObject on roadStream
    RouteQueryResult<LongitudinalDistance> GetDistanceBetweenObjects(const RoadMultiStream& roadStream, const ObjectPosition& objectPos, const std::optional<double> objectReferenceS, const ObjectPosition& targetObjectPos) const;

    //! Calculates the obstruction with an object i.e. how far to left or the right the object is from my position
    //! For more information see the [markdown documentation](\ref dev_framework_modules_world_getobstruction)
    //!
    //! \param laneStream       own lane stream
    //! \param tCoordinate      own t coordinate
    //! \param otherPosition    position of the other object
    //! \param objectCorners    corners of the other object
    //! \return obstruction with other object
    RouteQueryResult<Obstruction> GetObstruction(const LaneMultiStream& laneStream, double tCoordinate,
                                                 const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners, const Common::Vector2d& mainLaneLocator) const;

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
    RouteQueryResult<RelativeWorldView::Junctions> GetRelativeJunctions(const RoadMultiStream& roadStream, double startPosition, double range) const;

    //! Returns information about all lanes on the roadStream in range. These info are the relative distances (start and end),
    //! the laneId relative to the ego lane, the successors and predecessors if existing and the information wether the intended
    //! driving direction of the lane is the same as the direction of the roadStream. If the ego lane prematurely ends, then
    //! the further lane ids are relative to the middle of the road.
    //!
    //! \param roadStream       road stream to search
    //! \param startPosition    start search position on the road stream
    //! \param startLaneId      ego lane id
    //! \param range            range of search
    //! \param includeOncoming  indicating whether oncoming lanes should be included
    //! \return information about all lanes in range
    RouteQueryResult<RelativeWorldView::Lanes> GetRelativeLanes(const RoadMultiStream& roadStream, double startPosition, int startLaneId, double range, bool includeOncoming) const;

    //! Returns the relative lane id of the located position of a point relative to the given position
    //!
    //! \param roadStream       road stream to search
    //! \param startPosition    start search position on the road stream
    //! \param ownPosition      own position on stream
    //! \param ownLaneId        id of own lane
    //! \param targetPosition   position of queried point
    //! \return lane id relative to own position
    RouteQueryResult<std::optional<int>> GetRelativeLaneId(const RoadMultiStream& roadStream, double ownPosition, int ownLaneId, GlobalRoadPositions targetPosition) const;

    RouteQueryResult<std::optional<double>> GetLaneCurvature (const LaneMultiStream& laneStream, double position) const;

    RouteQueryResult<std::optional<double>> GetLaneWidth (const LaneMultiStream& laneStream, double position) const;

    RouteQueryResult<std::optional<double>> GetLaneDirection (const LaneMultiStream& laneStream, double position) const;
private:
    const OWL::Interfaces::WorldData& worldData;

    //! Returns the most upstream lane on the specified route such that there is a continous stream of lanes regarding successor/predecessor relation
    //! up to the start lane
    OWL::CLane* GetOriginatingRouteLane(std::vector<RouteElement> route, std::string startRoadId, OWL::OdId startLaneId, double startDistance) const;
    
    OWL::CRoad* GetOriginatingRouteRoad(const std::vector<std::string>& route, const std::string& startRoadId, const OWL::OdId startLaneId, const double startDistance) const;

    //! Returns the ids of the stream of roads leading the connecting road including the connecting road itself
    std::tuple<RoadGraph, RoadGraphVertex, RoadGraphVertex> GetRouteLeadingToConnector(std::string connectingRoadId) const;

    //! Returns the WorldPosition corresponding to the (s,t) position on the lane, if s is valid on the lane
    //! Otherwise the bool in the pair is false
    std::optional<Position> CalculatePositionIfOnLane(double sCoordinate, double tCoordinate, const OWL::Interfaces::Lane& lane) const;

    int FindNextEgoLaneId(const OWL::Interfaces::Lanes& lanesOnSection, bool inStreamDirection, std::map<int, OWL::Id> previousSectionLaneIds) const;

    std::map<int, OWL::Id> AddLanesOfSection(const OWL::Interfaces::Lanes& lanesOnSection, bool inStreamDirection,
                                             int currentOwnLaneId, bool includeOncoming, const std::map<int, OWL::Id>& previousSectionLaneIds,
                                             std::vector<RelativeWorldView::Lane>& previousSectionLanes, RelativeWorldView::LanesInterval& laneInterval) const;

    RoadMultiStream::Node CreateRoadMultiStreamRecursive(const RoadGraph& roadGraph, const RoadGraphVertex& current, double sOffset) const;

    LaneMultiStream::Node CreateLaneMultiStreamRecursive(const RoadGraph& roadGraph, const RoadGraphVertex& current, double sOffset, const OWL::Lane* lane) const;
};
