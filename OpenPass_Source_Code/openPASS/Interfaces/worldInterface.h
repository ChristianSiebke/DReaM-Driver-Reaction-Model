/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  worldInterface.h
//! @brief This file provides the interface of the scenery and dynamic objects
//!        to custom libraries.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <functional>
#include <list>

#include "Common/boostGeometryCommon.h"
#include "Common/globalDefinitions.h"
#include "Common/worldDefinitions.h"
#include "Common/vector2d.h"

class AgentInterface;
class ParameterInterface;
class SceneryInterface;
class TrafficObjectInterface;
class WorldObjectInterface;

using LaneTypes = std::vector<LaneType>;

//-----------------------------------------------------------------------------
//! Provides access to world representation
//-----------------------------------------------------------------------------
class WorldInterface
{
public:
    WorldInterface() = default;
    WorldInterface(const WorldInterface &) = delete;
    WorldInterface(WorldInterface &&) = delete;
    WorldInterface &operator=(const WorldInterface &) = delete;
    WorldInterface &operator=(WorldInterface &&) = delete;
    virtual ~WorldInterface() = default;


    virtual bool Instantiate() {return false;}

    virtual bool isInstantiated() {return false;}

    //-----------------------------------------------------------------------------
    //! Retrieves the OSI ground truth
    //!
    //! @return                global view
    //-----------------------------------------------------------------------------
    virtual void* GetOsiGroundTruth() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves global view on complete world (from which a sensor can retrieve
    //! arbitrary information)
    //!
    //! @return                global view
    //-----------------------------------------------------------------------------
    virtual void *GetGlobalDrivingView() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves static objects (roads and junctions)
    //!
    //! @return                reference to scenery
    //-----------------------------------------------------------------------------
    virtual void *GetGlobalObjects() = 0;

    //-----------------------------------------------------------------------------
    //! Sets time of day (hour) for the simulation run
    //!
    //! @return                time of day [0-23]
    //-----------------------------------------------------------------------------
    virtual void SetTimeOfDay(int timeOfDay) = 0;

    //-----------------------------------------------------------------------------
    //! Sets weekday for the simulation run
    //!
    //! @return                weekday
    //-----------------------------------------------------------------------------
    virtual void SetWeekday(Weekday weekday) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the OSI world data
    //!
    //! @return                Pointer OSI world data structure
    //-----------------------------------------------------------------------------
    virtual void* GetWorldData() = 0;
    
    //-----------------------------------------------------------------------------
    //! Retrieves time of day (hour)
    //!
    //! @return                time of day [0, 23]
    //-----------------------------------------------------------------------------
    virtual std::string GetTimeOfDay() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves weekday
    //!
    //! @return                weekday
    //-----------------------------------------------------------------------------
    virtual Weekday GetWeekday() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the visibility distance (meter)
    //!
    //! @return                visibility distance
    //-----------------------------------------------------------------------------
    virtual double GetVisibilityDistance() const = 0;

    //-----------------------------------------------------------------------------
    //! Sets the world parameters like weekday, library
    //! @param[in]     worldParamter  parameter to setup world
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetParameter(WorldParameter *worldParameter) = 0;

    //-----------------------------------------------------------------------------
    //! Sets the world parameters like weekday, library
    //! @param[in]     worldParamter  parameter to setup world
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void ExtractParameter(ParameterInterface* parameters) = 0;

    //-----------------------------------------------------------------------------
    //! Reset the world for the next run
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void Reset() = 0;

    //-----------------------------------------------------------------------------
    //! Clear the world, reset internal states
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void Clear() = 0;

    //-----------------------------------------------------------------------------
    //! Create internal global driving view
    //!
    //! @return                weekday
    //-----------------------------------------------------------------------------
    virtual bool CreateGlobalDrivingView() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves specific agent
    //! @param[in]  id         id of agent
    //!
    //! @return                Agent reference
    //-----------------------------------------------------------------------------
    virtual AgentInterface *GetAgent(int id) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves all agents
    //!
    //! @return                Mapping of ids to agents
    //-----------------------------------------------------------------------------
    virtual const std::map<int, AgentInterface *> &GetAgents() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves all worldObjects that currently exist
    //!
    //! @return                Vector of all worldObjects
    //-----------------------------------------------------------------------------
    virtual const std::vector<const WorldObjectInterface*>& GetWorldObjects() const = 0;

    //-----------------------------------------------------------------------------
    //! Add agent to world
    //! @param[in]  id         id of agent
    //! @param[in]  agent      agent reference
    //!
    //! @return                true if successful
    //-----------------------------------------------------------------------------
    virtual bool AddAgent(int id, AgentInterface *agent) = 0;

    //-----------------------------------------------------------------------------
    //! queue functions and values to update agent when SyncGlobalData is called
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void QueueAgentUpdate(std::function<void(double)> func,
                                  double val) = 0;

    //-----------------------------------------------------------------------------
    //! queue functions and values to update agent when SyncGlobalData is called
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void QueueAgentUpdate(std::function<void()> func) = 0;

    //-----------------------------------------------------------------------------
    //! Add agent to list to be removed when SyncGlobalData is called
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void QueueAgentRemove(const AgentInterface *agent) = 0;

    //-----------------------------------------------------------------------------
    //! Wirte all global date into the datesstore
    //!
    //! @return timestamp under which the data should be published
    //-----------------------------------------------------------------------------
    virtual void PublishGlobalData(int timestamp) = 0;

    //-----------------------------------------------------------------------------
    //! Update all agents / Remove agents and sync data of all agents within world
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SyncGlobalData() = 0;

    //-----------------------------------------------------------------------------
    //! Create a scenery in world.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool CreateScenery(SceneryInterface *scenery) = 0;

    //-----------------------------------------------------------------------------
    //! Create an agentAdapter for an agent to communicate between the agent of the
    //! framework and the world.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual AgentInterface *CreateAgentAdapterForAgent() = 0;

    //-----------------------------------------------------------------------------
    //! Returns one agent which is set to be special.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const AgentInterface *GetSpecialAgent() = 0;

    //-----------------------------------------------------------------------------
    //! Returns the last car in lane. Return nullptr if no car is found.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const AgentInterface *GetLastCarInlane(int laneNumber) = 0;

    //-----------------------------------------------------------------------------
    //! Returns an agentInterface which is a bicycle. Returns nullptr if no bicyle
    //! found.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const AgentInterface *GetBicycle() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the world postion by the distance and the lane.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual Position LaneCoord2WorldCoord(double distanceOnLane, double offset, std::string roadId,
            int laneId) const = 0;

    //-----------------------------------------------------------------------------
    //! Tries to create an internal scenery from a given file.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool CreateWorldScenery(const  std::string &sceneryFilename) = 0;

    //-----------------------------------------------------------------------------
    //! Tries to create an internal scenario from a given file.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool CreateWorldScenario(const  std::string &scenarioFilename) = 0;

    //-----------------------------------------------------------------------------
    //! Returns all agents in specified range (also agents partially in search interval).
    //! Returns empty list otherwise.
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] startDistance    start of search (s coordinate)
    //! @param[in] backwardRange    search range against route direction from measured from startDistance
    //! @param[in] forwardRange     search range in route direction from measured from startDistance
    //!
    //! @return All agents in specified range
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::vector<const AgentInterface*>> GetAgentsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance,
                                                                       double backwardRange, double forwardRange) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns all objects in specified range (also objects partially in search interval).
    //! Returns empty list otherwise.
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] startDistance    start of search (s coordinate)
    //! @param[in] backwardRange    search range against route direction from measured from startDistance
    //! @param[in] forwardRange     search range in route direction from measured from startDistance
    //!
    //! @return All objects in specified range
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::vector<const WorldObjectInterface*>> GetObjectsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance,
                                                                       double backwardRange, double forwardRange) const = 0;

    //! Returns all agents on the specified connectingRoad of a junction and on the incoming lanes that lead to this connecting road
    //! inside a certain range. The range is measured backwards from the end of the connecting road.
    //!
    //! \param connectingRoadId OpenDrive id of the connecting road
    //! \param range            Distance of the search start to the end of connecting road
    //! 
    //! \return  All agents in specified range
    virtual std::vector<const AgentInterface*> GetAgentsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const = 0;

    //! Returns the s coordinate distance from the front of the agent to the first point where his lane intersects another.
    //! As the agent may not yet be on the junction, it has to be specified which connecting road he will take in the junction
    //!
    //! \param position                 position of the agent
    //! \param intersectingConnectorId  OpenDrive id of the connecting road that intersects with the agent
    //! \param intersectingLaneId       OpenDrive id of the lane on the intersecting connecting road
    //! \param ownConnectorId           OpenDrive id of the connecting road that this agent is assumed to drive on
    //!
    //! \return distance of front of agent to the intersecting lane
    virtual double GetDistanceToConnectorEntrance(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const = 0;

    //! Returns the s coordinate distance from the rear of the agent to the furthest point where his lane intersects another.
    //! As the agent may not yet be on the junction, it has to be specified which connecting road he will take in the junction
    //!
    //! \param position                 position of the agent
    //! \param intersectingConnectorId  OpenDrive id of the connecting road that intersects with the agent
    //! \param intersectingLaneId       OpenDrive id of the lane on the intersecting connecting road
    //! \param ownConnectorId           OpenDrive id of the connecting road that this agent is assumed to drive on
    //!
    //! \return distance of rear of agent to the farther side of the intersecting lane
    virtual double GetDistanceToConnectorDeparture(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve whether s coordinate is valid on given lane.
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneId OpenDriveId of lane to search in
    //! @param[in] distance  s coordinate
    //! @return true if s is valid at given distance, false otherwise
    //-----------------------------------------------------------------------------
    virtual bool IsSValidOnLane(std::string roadId, int laneId, double distance) = 0;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the curvature of the lane at the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position.
    //!
    //! @param[in] roadId   OpenDriveId of the road to search in
    //! @param[in] laneId   OpenDriveId of lane to search in
    //! @param[in] position s coordinate of search start
    //! @return curvature at position
    //-----------------------------------------------------------------------------
    virtual double GetLaneCurvature(std::string roadId, int laneId, double position) const = 0 ;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the curvature of the lane at distance from the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position or the lane stream ends before the distance
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] position s       coordinate of search start
    //! @param[in] distance s       coordinate difference from position to the point of interst
    //! @return curvature at position
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::optional<double> > GetLaneCurvature(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the width of the lane the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position.
    //!
    //! @param[in] roadId   OpenDriveId of the road to search in
    //! @param[in] laneId   OpenDriveId of lane to search in
    //! @param[in] position s coordinate of search start
    //! @return width at position
    //-----------------------------------------------------------------------------
    virtual double GetLaneWidth(std::string roadId, int laneId, double position) const = 0 ;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the width of the lane at distance from the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position or the lane stream ends before the distance
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] position s       coordinate of search start
    //! @param[in] distance s       coordinate difference from position to the point of interst
    //! @return width at position
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::optional<double> > GetLaneWidth(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns value for the direction (i.e. heading) of the lane at the given position.
    //! The heading is constant between two joints
    //! Returns 0 if there is no lane at the given position.
    //!
    //! @param[in] roadId   OpenDriveId of the road to search in
    //! @param[in] laneId   OpenDriveId of lane to search in
    //! @param[in] position s coordinate of search start
    //! @return direction at position
    //-----------------------------------------------------------------------------
    virtual double GetLaneDirection(std::string roadId, int laneId, double position) const = 0 ;

    //-----------------------------------------------------------------------------
    //! Returns value for the curvature (i.e. heading) of the lane at distance from the given position.
    //! The heading is constant between two joints
    //! Returns 0 if there is no lane at the given position or the lane stream ends before the distance
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] position s       coordinate of search start
    //! @param[in] distance s       coordinate difference from position to the point of interst
    //! @return direction at position
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::optional<double> > GetLaneDirection(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns remaining distance to end of lane stream (along given route) or until next lane which has non of the following types:
    //! Driving, Exit, OnRamp, OffRamp or Stop
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneNumber       OpenDriveId of lane to search in
    //! @param[in] distance         s coordinate
    //! @param[in] maxSearchLength  maximum search length
    //! @return remaining distance
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<double> GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance,
                                                                             double maximumSearchLength) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns remaining distance to end of lane stream (along given route) or until next lane which has non of the specified types:
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneNumber       OpenDriveId of lane to search in
    //! @param[in] distance         s coordinate
    //! @param[in] maxSearchLength  maximum search length
    //! @param[in] laneTypes        filter for lane types
    //! @return remaining distance
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<double> GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance,
                                                                             double maximumSearchLength, const LaneTypes& laneTypes) const = 0;

    //-----------------------------------------------------------------------------
    //! \brief GetDistanceBetweenObjects gets the distance between two
    //!        ObjectPositions on the specified Route. Returns
    //!        std::numeric_limits<double>::max() if Route does not include both
    //!        ObjectPositions
    //!
    //! \param route the Route on which to calculate the distance between two
    //!        ObjectPositions
    //! \param objectPos the ObjectPosition of the first object
    //! \param targetObjectPos the ObjectPosition of the reference object
    //! \return the distance between the ObjectPositions on the Route
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::optional<LongitudinalDistance>> GetDistanceBetweenObjects(const RoadGraph& roadGraph,
                                                                                            RoadGraphVertex startNode,
                                                                                            const ObjectPosition& objectPos,
                                                                                            const ObjectPosition& targetObjectPos) const = 0;
    //-----------------------------------------------------------------------------
    //! Retrieve whether a new agent intersects with an existing agent
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool IntersectsWithAgent(double x, double y, double rotation, double length, double width, double center) = 0;

    virtual Position RoadCoord2WorldCoord(RoadPosition roadCoord, std::string roadID = "") const = 0;

    //! Calculates the obstruction of an agent with another object i.e. how far to left or the right the object is from my position
    //! For more information see the [markdown documentation](\ref dev_framework_modules_world_getobstruction)
    //!
    //! \param route            route of the agent
    //! \param ownPosition      position of the agent
    //! \param otherPosition    position of the other object
    //! \param objectCorners    corners of the other object
    //! \return obstruction with the other object
    virtual RouteQueryResult<Obstruction> GetObstruction(const RoadGraph& roadGraph, RoadGraphVertex startNode, const GlobalRoadPosition& ownPosition,
                                                         const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners, const Common::Vector2d& mainLaneLocator) const = 0;

    //! Returns all traffic signs valid for a lane inside the range
    //!
    //! \param route            route to search along
    //! \param roadId           OpenDrive Id of the road
    //! \param laneId           OpenDrive Id of the lane
    //! \param startDistance    s coordinate
    //! \param searchRange      range of search (can also be negative)
    //! \return traffic signs in range
    virtual RouteQueryResult<std::vector<CommonTrafficSign::Entity>> GetTrafficSignsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId,
                                                                                            double startDistance, double searchRange) const = 0;

    //! Returns all road markings valid for a lane inside the range
    //!
    //! \param route            route to search along
    //! \param roadId           OpenDrive Id of the road
    //! \param laneId           OpenDrive Id of the lane
    //! \param startDistance    s coordinate
    //! \param searchRange      range of search
    //! \return road markings in range (can also be negative)
    virtual RouteQueryResult<std::vector<CommonTrafficSign::Entity>> GetRoadMarkingsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId,
                                                                                            double startDistance, double searchRange) const = 0;

    //! Retrieves all lane markings on the given position on the given side of the lane inside the range
    //!
    //! \param roadId           OpenDrive Id of the road
    //! \param laneId           OpenDrive Id of the lane
    //! \param startDistance    s coordinate
    //! \param range            search range
    //! \param side             side of the lane
    virtual RouteQueryResult<std::vector<LaneMarking::Entity>> GetLaneMarkings(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double range, Side side) const = 0;

    //! Returns the relative distances (start and end) and the connecting road id of all junctions on the route in range
    //!
    //! \param route            route of the agent
    //! \param roadId           OpenDrive Id of the road
    //! \param startDistance    start s coordinate on the road
    //! \param range            range of search
    //! \return information about all junctions in range
    virtual RouteQueryResult<RelativeWorldView::Junctions> GetRelativeJunctions (const RoadGraph& roadGraph, RoadGraphVertex startNode, double startDistance, double range) const = 0;

    //! Returns information about all lanes on the route in range. These info are the relative distances (start and end),
    //! the laneId relative to the ego lane, the successors and predecessors if existing and the information wether the intended
    //! driving direction of the lane is the same as the direction of the route. If the ego lane prematurely ends, then
    //! the further lane ids are relative to the middle of the road.
    //!
    //! \param route            route of the agent
    //! \param roadId           OpenDrive Id of the road
    //! \param laneId           OpenDrive Id of the lane
    //! \param distance         start s coordinate on the road
    //! \param range            range of search
    //! \return information about all lanes in range
    virtual RouteQueryResult<RelativeWorldView::Lanes> GetRelativeLanes(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double distance, double range) const = 0;

    //! Returns all possible connections on the junction, that an agent has when coming from the specified road
    //!
    //! \param junctionId       OpenDrive Id of the junction
    //! \param incomingRoadId   OpenDrive Id of the incoming road
    //! \return all connections on junctions with specified incoming road
    virtual std::vector<JunctionConnection> GetConnectionsOnJunction(std::string junctionId, std::string incomingRoadId) const = 0;

    //! Returns all priorities between the connectors of a junction
    //!
    //! \param junctionId   OpenDrive id of the junction
    virtual std::vector<JunctionConnectorPriority> GetPrioritiesOnJunction(std::string junctionId) const = 0;

    //! Returns the id of all roads on a junction that intersect with the given road and their rank compared to this road.
    //! Note: The rank is the rank of the intersecting road, i.e. "Higher" means that the intersecting road has higher priority
    //! then the given connectingRoad
    //!
    //! \param connectingRoadId     OpenDrive id of the road, for which intersections should be retrieved
    //! \return id and rank of all roads intersecting the connecting road
    virtual std::vector<IntersectingConnection> GetIntersectingConnections(std::string connectingRoadId) const = 0;

    //! Returns the Id and type (road or junction) if the next element of the road in stream direction.
    //! Returns tpye None if there is no successor.
    //!
    //! \param roadId   OpenDrive Id of road
    //! \return type and OpenDrive Id of next downstream element
    virtual RoadNetworkElement GetRoadSuccessor(std::string roadId) const = 0;

    //! Returns the Id and type (road or junction) if the next element of the road against stream direction.
    //! Returns tpye None if there is no predecessor.
    //!
    //! \param roadId   OpenDrive Id of road
    //! \return type and OpenDrive Id of next upstream element
    virtual RoadNetworkElement GetRoadPredecessor(std::string roadId) const = 0;

    virtual std::pair<RoadGraph, RoadGraphVertex> GetRoadGraph (const RouteElement& start, int maxDepth) const = 0;

    virtual std::map<RoadGraphEdge, double> GetEdgeWeights (const RoadGraph& roadGraph) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the friction
    //!
    //! @return                friction
    //-----------------------------------------------------------------------------
    virtual double GetFriction() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns one agent which is set to be special.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual AgentInterface* GetEgoAgent() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves agents that were removed from the world during the simulation run
    //!
    //! @return                List of agent references
    //-----------------------------------------------------------------------------
    virtual const std::list<const AgentInterface*>& GetRemovedAgents() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves all traffic objects
    //!
    //! @return                Traffic objects
    //-----------------------------------------------------------------------------
    virtual const std::vector<const TrafficObjectInterface*>& GetTrafficObjects() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns one agent with the specified scenarioName
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual AgentInterface* GetAgentByName(const std::string& scenarioName) = 0;
};
