/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
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

class LaneQueryResult
{
public:
    static LaneQueryResult InvalidResult()
    {
        return LaneQueryResult(false);
    }

    LaneQueryResult(uint64_t streamId, double startDistance, double endDistance, LaneCategory laneCategory,
                    bool isDrivingLane) :
        valid{true},
        streamId{streamId},
        startDistance{startDistance},
        endDistance{endDistance},
        laneCategory{laneCategory},
        isDrivingLane{isDrivingLane}
    {}

    bool valid { false };
    uint64_t streamId { 0 };
    double startDistance { 0.0 };
    double endDistance { 0.0 };
    LaneCategory laneCategory { LaneCategory::Undefined };
    bool isDrivingLane { false };

private:
    LaneQueryResult(bool valid) : valid{valid}
    {
    }
};

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


    // Agent functions
    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the first agent found.
    //! Returns nullptr if no agent is found.
    //! Internal OWL::EVENTHORIZON is used as maximum search distance.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @return First agent found along the route
    //-----------------------------------------------------------------------------
    virtual AgentInterface* GetNextAgentInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the first agent found.
    //! Returns nullptr if no agent is found.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @param[in] searchDistance   maximum distance to search
    //! @return First agent found along the route
    //-----------------------------------------------------------------------------
    virtual AgentInterface* GetNextAgentInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection, double searchDistance) const = 0;

    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the last agent found.
    //! Returns nullptr if no agent is found.
    //! Internal OWL::EVENTHORIZON is used as maximum search distance.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @return Last agent found along the route
    //-----------------------------------------------------------------------------
    virtual AgentInterface* GetLastAgentInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the last agent found.
    //! Returns nullptr if no agent is found.
    //! Internal OWL::EVENTHORIZON is used as maximum search distance.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @param[in] searchDistance   maximum distance to search
    //! @return Last agent found along the route
    //-----------------------------------------------------------------------------
    virtual AgentInterface* GetLastAgentInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection, double searchDistance) const = 0;

    // Obstacle functions
    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the first traffic object found.
    //! Returns nullptr if no traffic object is found.
    //! Internal OWL::EVENTHORIZON is used as maximum search distance.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @return First traffic object found along the route
    //-----------------------------------------------------------------------------
    virtual TrafficObjectInterface* GetNextTrafficObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the first traffic object found.
    //! Returns nullptr if no traffic object is found.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @param[in] searchDistance   maximum distance to search
    //! @return First traffic object found along the route
    //-----------------------------------------------------------------------------
    virtual TrafficObjectInterface* GetNextTrafficObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection, double searchDistance) const = 0;

    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the last traffic object found.
    //! Returns nullptr if no traffic object is found.
    //! Internal OWL::EVENTHORIZON is used as maximum search distance.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @return Last traffic object found along the route
    //-----------------------------------------------------------------------------
    virtual TrafficObjectInterface* GetLastTrafficObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the last traffic object found.
    //! Returns nullptr if no traffic object is found.
    //! Internal OWL::EVENTHORIZON is used as maximum search distance.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @param[in] searchDistance   maximum distance to search
    //! @return Last traffic object found along the route
    //-----------------------------------------------------------------------------
    virtual TrafficObjectInterface* GetLastTrafficObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection, double searchDistance) const = 0;

    // Generic functions
    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the first object found.
    //! Returns nullptr if no object is found.
    //! Internal OWL::EVENTHORIZON is used as maximum search distance.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @return First object found along the route
    //-----------------------------------------------------------------------------
    virtual WorldObjectInterface* GetNextObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the first object found.
    //! Returns nullptr if no object is found.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @param[in] searchDistance   maximum distance to search
    //! @return First object found along the route
    //-----------------------------------------------------------------------------
    virtual WorldObjectInterface* GetNextObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection,
            double searchDistance) const = 0;

    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the traffic object found.
    //! Returns nullptr if no object is found.
    //! Internal OWL::EVENTHORIZON is used as maximum search distance.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @return Last object found along the route
    //-----------------------------------------------------------------------------
    virtual WorldObjectInterface* GetLastObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Searches the lane starting at the given distance and returns the last object found.
    //! Returns nullptr if no object is found.
    //! Internal OWL::EVENTHORIZON is used as maximum search distance.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] currentDistance  start s coordinate
    //! @param[in] searchInForwardDirection specifies wether to search in or against the route direction
    //! @param[in] searchDistance   maximum distance to search
    //! @return Last object found along the route
    //-----------------------------------------------------------------------------
    virtual WorldObjectInterface* GetLastObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection,
            double searchDistance) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns all agents in specified range (also agents partially in search interval).
    //! Returns empty list otherwise.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId           OpenDriveId of the road to search in
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] startDistance    start of search (s coordinate)
    //! @param[in] backwardRange    search range against route direction from measured from startDistance
    //! @param[in] forwardRange     search range in route direction from measured from startDistance
    //!
    //! @return All agents in specified range
    //-----------------------------------------------------------------------------
    virtual std::vector<const AgentInterface*> GetAgentsInRange(Route route, std::string roadId, int laneId, double startDistance,
                                                                double backwardRange, double forwardRange) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns all objects in specified range (also objects partially in search interval).
    //! Returns empty list otherwise.
    //!
    //! @param[in] route            Route along which to search
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneId OpenDriveId of lane to search in
    //! @param[in] startDistance    start of search (s coordinate)
    //! @param[in] backwardRange    search range against route direction from measured from startDistance
    //! @param[in] forwardRange     search range in route direction from measured from startDistance
    //!
    //! @return All objects in specified range
    //-----------------------------------------------------------------------------
    virtual std::vector<const WorldObjectInterface*> GetObjectsInRange(Route route, std::string roadId, int laneId, double startDistance,
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
    //! Returns ids of driving lanes at given distance, empty vector otherwise.
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] distance  s coordinate
    //! @return Ids of lanes with type driving at given distance
    //-----------------------------------------------------------------------------
    virtual std::vector<int> GetDrivingLanesAtDistance(std::string roadId, double distance) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns ids of driving lanes at given distance, empty vector otherwise.
    //!
    //! @param[in] roadId  OpenDriveId of the road to look in
    //! @param[in] distance  s coordinate
    //! @return Ids of lanes with type stop at given distance
    //-----------------------------------------------------------------------------
    virtual std::vector<int> GetStopLanesAtDistance(std::string roadId, double distance) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns ids of exit lanes at given distance, empty vector otherwise.
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] distance  s coordinate
    //! @return Ids of lanes with type stop at given distance
    //-----------------------------------------------------------------------------
    virtual std::vector<int> GetExitLanesAtDistance(std::string roadId, double distance) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns ids of onRamps and offRamps at given distance, empty vector otherwise.
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] distance  s coordinate
    //! @return Ids of lanes with type stop at given distance
    //-----------------------------------------------------------------------------
    virtual std::vector<int> GetRampsAtDistance(std::string roadId, double distance) const = 0;
    //-----------------------------------------------------------------------------
    //! Retrieve a valid s coordinate on given lane (downstream)
    //! Internal a step-size is used which is set to 100
    //! Internal a max. search distance is used (OWL::Eventhorizon)
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneId OpenDriveId of lane to search in
    //! @param[in] distance  start s coordinate
    //! @param[out] next next valid s coordinate, INFINIT if valid s is not found
    //! @return true if valid s is found in search range, false otherwise
    //-----------------------------------------------------------------------------
    virtual bool GetNextValidSOnLane(std::string roadId, int laneId, double distance, double& next) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve a valid s coordinate on given lane (upstream)
    //! Internal a step-size is used which is set to 100
    //! Internal a max. search distance is used (OWL::Eventhorizon)
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneId OpenDriveId of lane to search in
    //! @param[in] distance  start s coordinate
    //! @param[out] last last valid s coordinate, -INFINIT if valid s is not found
    //! @return true if valid s is found in search range, false otherwise
    //-----------------------------------------------------------------------------
    virtual bool GetLastValidSOnLane(std::string roadId, int laneId, double distance, double& last) = 0;

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
    //! Check if a left lane at given distance exists
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneId OpenDriveId of lane to search in
    //! @param[in] distance  s coordinate
    //! @return true if left lane exists at given distance, false otherwise
    //-----------------------------------------------------------------------------
    virtual bool ExistsLaneLeft(std::string roadId, int laneId, double distance) = 0;

    //-----------------------------------------------------------------------------
    //! Check if a right lane at given distance exists
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneId OpenDriveId of lane to search in
    //! @param[in] distance  s coordinate
    //! @return true if right lane exists at given distance, false otherwise
    //-----------------------------------------------------------------------------
    virtual bool ExistsLaneRight(std::string roadId, int laneId, double distance) = 0;

    //-----------------------------------------------------------------------------
    //! Returns number of lanes at given distance
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] distance  s coordinate
    //! @return number of lanes at distance.
    //-----------------------------------------------------------------------------
    virtual int GetNumberOfLanes(std::string roadId, double distance) = 0;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the curvature of the lane at distance from the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position or the lane stream ends before the distance
    //!
    //! @param[in] route    Route along which to search
    //! @param[in] roadId   OpenDriveId of the road to search in
    //! @param[in] laneId   OpenDriveId of lane to search in
    //! @param[in] position s coordinate of search start
    //! @param[in] distance s coordinate difference from position to the point of interst
    //! @return curvature at distance from start position
    //-----------------------------------------------------------------------------
    virtual double GetLaneCurvature(Route route, std::string roadId, int laneId, double position, double distance = 0.0) const = 0 ;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the width of the lane at distance from the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position or the lane stream ends before the distance
    //!
    //! @param[in] route    Route along which to search
    //! @param[in] roadId   OpenDriveId of the road to search in
    //! @param[in] laneId   OpenDriveId of lane to search in
    //! @param[in] position s coordinate of search start
    //! @param[in] distance s coordinate difference from position to the point of interst
    //! @return width at distance from start position
    //-----------------------------------------------------------------------------
    virtual double GetLaneWidth(Route route, std::string roadId, int laneId, double position, double distance = 0.0) const = 0 ;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the direction (heading) of the lane at distance from the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position or the lane stream ends before the distance
    //!
    //! @param[in] route    Route along which to search
    //! @param[in] roadId   OpenDriveId of the road to search in
    //! @param[in] laneId   OpenDriveId of lane to search in
    //! @param[in] position s coordinate of search start
    //! @param[in] distance s coordinate difference from position to the point of interst
    //! @return direction at distance from start position
    //-----------------------------------------------------------------------------
    virtual double GetLaneDirection(Route route, std::string roadId, int laneId, double position, double distance = 0.0) const = 0 ;

    //-----------------------------------------------------------------------------
    //! Returns remaining distance to end of lane stream (along given route) or until next lane which has non of the following types:
    //! Driving, Exit, OnRamp, OffRamp or Stop
    //!
    //! @param[in] route    Route along which to search
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneNumber OpenDriveId of lane to search in
    //! @param[in] distance  s coordinate
    //! @param[in] maxSearchLength maximum search length
    //! @return remaining distance
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToEndOfLane(Route route, std::string roadId, int laneNumber, double initialSearchDistance,
                                          double maxSearchLength) = 0;

    //-----------------------------------------------------------------------------
    //! Returns remaining distance to end of lane stream (along given route) or until next non driving lane (0 if this lane is no driving lane)
    //!
    //! @param[in] route    Route along which to search
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneNumber OpenDriveId of lane to search in
    //! @param[in] distance  s coordinate
    //! @param[in] maxSearchLength maximum search length
    //! @return remaining distance
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToEndOfDrivingLane(Route route, std::string roadId, int laneNumber, double initialSearchDistance,
            double maxSearchLength) = 0;

    //-----------------------------------------------------------------------------
    //! Returns remaining distance to end of lane stream (along given route) or until next non driving or stop lane (0 if this lane is no driving or stop lane)
    //!
    //! @param[in] route    Route along which to search
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneNumber OpenDriveId of lane to search in
    //! @param[in] distance  s coordinate
    //! @param[in] maxSearchLength maximum search length
    //! @return remaining distance
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToEndOfDrivingOrStopLane(Route route, std::string roadId, int laneNumber, double initialSearchDistance,
            double maxSearchLength) = 0;

    //-----------------------------------------------------------------------------
    //! Returns remaining distance to end of lane stream (along given route) or until next non ramp lane (0 if this lane is no ramp lane)
    //!
    //! @param[in] route    Route along which to search
    //! @param[in] laneId OpenDriveId of lane to search in
    //! @param[in] distance  s coordinate
    //! @param[in] maxSearchLength maximum search length
    //! @return remaining distance
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToEndOfRamp(Route route, std::string roadId, int laneId, double initialSearchDistance,
                                          double maxSearchLength) = 0;

    //-----------------------------------------------------------------------------
    //! Returns remaining distance to end of lane stream (along given route) or until next non exit lane (0 if this lane is no exit lane)
    //!
    //! @param[in] route    Route along which to search
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneId OpenDriveId of lane to search in
    //! @param[in] distance  s coordinate
    //! @param[in] maxSearchLength maximum search length
    //! @return remaining distance
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToEndOfExit(Route route, std::string roadId, int laneId, double initialSearchDistance,
                                          double maxSearchLength) = 0;

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
    virtual double GetDistanceBetweenObjects(const Route& route,
                                             const ObjectPosition& objectPos,
                                             const ObjectPosition& targetObjectPos) const = 0;
    //-----------------------------------------------------------------------------
    //! Retrieve whether a new agent intersects with an existing agent
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool IntersectsWithAgent(double x, double y, double rotation, double length, double width, double center) = 0;

    virtual polygon_t GetBoundingBoxAroundAgent(AgentInterface* agent, double width, double length) = 0;

    virtual Position RoadCoord2WorldCoord(RoadPosition roadCoord, std::string roadID = "") const = 0;

    //! Calculates the obstruction of an agent with another object i.e. how far to left or the right the object is from my position
    //! For more information see the [markdown documentation](\ref dev_framework_modules_world_getobstruction)
    //!
    //! \param route            route of the agent
    //! \param ownPosition      position of the agent
    //! \param otherPosition    position of the other object
    //! \param objectCorners    corners of the other object
    //! \return obstruction with the other object
    virtual Obstruction GetObstruction(const Route& route, const GlobalRoadPosition& ownPosition, const ObjectPosition& otherPosition,
                                                 const std::vector<Common::Vector2d>& objectCorners) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves all traffic signs in front
    //!
    //! @return                TrafficSigns
    //-----------------------------------------------------------------------------
    virtual std::vector<CommonTrafficSign::Entity> GetTrafficSignsInRange(const Route& route, std::string roadId, int laneId,
            double startDistance, double searchRange) const = 0;

    //! Retrieves all lane markings on the given position on the given side of the lane inside the range
    //!
    //! \param roadId           OpenDrive Id of the road
    //! \param laneId           OpenDrive Id of the lane
    //! \param startDistance    s coordinate
    //! \param range            search range
    //! \param side             side of the lane
    virtual std::vector<LaneMarking::Entity> GetLaneMarkings(const Route& route, std::string roadId, int laneId, double startDistance, double range, Side side) const = 0;

    //! Returns the relative distances (start and end) and the connecting road id of all junctions on the route in range
    //!
    //! \param route            route of the agent
    //! \param roadId           OpenDrive Id of the road
    //! \param startDistance    start s coordinate on the road
    //! \param range            range of search
    //! \return information about all junctions in range
    virtual RelativeWorldView::Junctions GetRelativeJunctions (const Route& route, std::string roadId, double startDistance, double range) const = 0;

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
    virtual RelativeWorldView::Lanes GetRelativeLanes(const Route& route, std::string roadId, int laneId, double distance, double range) const = 0;

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

    //! Returns a randomized route starting at the given position
    //!
    //! \param start    start Position
    //! \return randomized route
    virtual Route GetRoute (GlobalRoadPosition start) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the friction
    //!
    //! @return                friction
    //-----------------------------------------------------------------------------
    virtual double GetFriction() const = 0;

    virtual LaneQueryResult QueryLane(std::string roadId, int laneId, double distance) const = 0;

    virtual std::list<LaneQueryResult> QueryLanes(std::string roadId, double startDistance, double endDistance) const = 0;

    //-----------------------------------------------------------------------------
    //! Converts stream Id for lane at given distance into  OpenDriveID
    //! Returns -999 if stream Id is invalid for given distance
    //!
    //! @param[in] streamId   stream id of lane
    //! @param[in] endDistance s coordinate
    //! @return OpenDriveId of lane at distance
    //-----------------------------------------------------------------------------
    virtual int GetLaneId(uint64_t streamId, double endDistance) const = 0;

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

    //-----------------------------------------------------------------------------
    //! Returns a list with all agents n the specified group
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual std::vector<AgentInterface*> GetAgentsByGroupType(const AgentCategory& agentCategory) = 0;

    //-----------------------------------------------------------------------------
    //! \brief GetNextConnectingRoadIdOnRoute gets the next connecting Road (Road
    //!        within a Junction) OdId on Route shared with ObjectPosition
    //!        Returns empty string if no such Road found
    //!
    //! \param route the Route on which to find the next connecting Road OdId
    //! \param objectPos the ObjectPosition from which to begin the search
    //!
    //! \return the OdId of the first connecting Road on route after objectPos
    //-----------------------------------------------------------------------------
    virtual std::string GetNextJunctionIdOnRoute(const Route& route, const ObjectPosition& objectPos) const = 0;

    //-----------------------------------------------------------------------------
    //! \brief GetDistanceToJunction gets the distance from objectPos to the
    //!        Junction with junctionId on Route route
    //!
    //! \param route the Route on which to calculate the distance from objectPos to
    //!              Junction with junctionId
    //! \param objectPos the ObjectPosition from which to get the distance to the
    //!                  junction
    //! \param connectingRoadId the Road OdId of the connecting road on the
    //!                         Junction
    //!
    //! \return the distance from objectPos to the junction
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToJunction(const Route& route, const ObjectPosition& objectPos, const std::string& junctionId) const = 0;
};
