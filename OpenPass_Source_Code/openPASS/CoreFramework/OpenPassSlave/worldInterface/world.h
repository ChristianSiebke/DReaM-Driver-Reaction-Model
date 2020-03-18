/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  World.h
//! @brief This file includes the static and dynamic objects used during a
//!        simulation run.
//-----------------------------------------------------------------------------

#pragma once

#include "Interfaces/worldInterface.h"
#include "worldBinding.h"

namespace SimulationSlave {

class World : public WorldInterface
{
public:
    World(WorldBinding* worldBinding) :
        worldBinding(worldBinding)
    {}

    World(const World&) = delete;
    World(World&&) = delete;
    World& operator=(const World&) = delete;
    World& operator=(World&&) = delete;

    ~World() = default;

    bool AddAgent(int id, AgentInterface* agent) override
    {
        return implementation->AddAgent(id, agent);
    }

    AgentInterface* GetAgent(int id) const override
    {
        return implementation->GetAgent(id);
    }

    AgentInterface* GetAgentByName(const std::string& scenarioName) override
    {
        return implementation->GetAgentByName(scenarioName);
    }

    const std::vector<const WorldObjectInterface*>& GetWorldObjects() const override
    {
        return implementation->GetWorldObjects();
    }

    const std::map<int, AgentInterface*>& GetAgents() const override
    {
        return implementation->GetAgents();
    }

    const std::vector<const TrafficObjectInterface*>& GetTrafficObjects() const override
    {
        return implementation->GetTrafficObjects();
    }

    const std::list<const AgentInterface*>& GetRemovedAgents() const override
    {
        return implementation->GetRemovedAgents();
    }

    // framework internal methods to access members without restrictions
    void ExtractParameter(ParameterInterface* parameters) override
    {
        return implementation->ExtractParameter(parameters);
    }

    void Reset() override
    {
        return implementation->Reset();
    }

    void Clear() override
    {
        return implementation->Clear();
    }

    std::string GetTimeOfDay() const override
    {
        return implementation->GetTimeOfDay();
    }

    double GetVisibilityDistance() const override
    {
        return implementation->GetVisibilityDistance();
    }

    double GetFriction() const override
    {
        return implementation->GetFriction();
    }

    void* GetOsiGroundTruth() override
    {
        return implementation->GetOsiGroundTruth();
    }

    void* GetWorldData() override
    {
        return implementation->GetWorldData();
    }

    void QueueAgentUpdate(std::function<void()> func) override
    {
        return implementation->QueueAgentUpdate(func);
    }

    void QueueAgentRemove(const AgentInterface* agent) override
    {
        return implementation->QueueAgentRemove(agent);
    }

    void SyncGlobalData() override
    {
        return implementation->SyncGlobalData();
    }

    bool CreateScenery(SceneryInterface* scenery) override
    {
        return implementation->CreateScenery(scenery);
    }

    AgentInterface* CreateAgentAdapterForAgent() override
    {
        return implementation->CreateAgentAdapterForAgent();
    }

    AgentInterface* GetEgoAgent() override
    {
        return implementation->GetEgoAgent();
    }

    std::vector<const AgentInterface*> GetAgentsInRange(Route route, std::string roadId, int laneId, double startDistance,
            double backwardRange, double forwardRange) const override
    {
        return implementation->GetAgentsInRange(route, roadId, laneId, startDistance, backwardRange, forwardRange);
    }

    std::vector<const WorldObjectInterface*> GetObjectsInRange(Route route, std::string roadId, int laneId, double startDistance,
            double backwardRange, double forwardRange) const override
    {
        return implementation->GetObjectsInRange(route, roadId, laneId, startDistance, backwardRange, forwardRange);
    }

    std::vector<const AgentInterface*> GetAgentsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const override
    {
        return implementation->GetAgentsInRangeOfJunctionConnection(connectingRoadId, range);
    }

    double GetDistanceToConnectorEntrance(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override
    {
        return implementation->GetDistanceToConnectorEntrance(position, intersectingConnectorId, intersectingLaneId, ownConnectorId);
    }

    double GetDistanceToConnectorDeparture(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override
    {
        return implementation->GetDistanceToConnectorDeparture(position, intersectingConnectorId, intersectingLaneId, ownConnectorId);
    }

    Position LaneCoord2WorldCoord(double distance, double offset, std::string roadId, int laneId) const override
    {
        return implementation->LaneCoord2WorldCoord(distance, offset, roadId, laneId);
    }

    bool IsSValidOnLane(std::string roadId, int laneId, double distance) override
    {
        return implementation->IsSValidOnLane(roadId, laneId, distance);
    }

    double GetLaneCurvature(Route route, std::string roadId, int laneId, double position, double distance = 0.0) const override
    {
        return implementation->GetLaneCurvature(route, roadId, laneId, position, distance);
    }

    double GetLaneWidth(Route route, std::string roadId, int laneId, double position, double distance = 0.0) const override
    {
        return implementation->GetLaneWidth(route, roadId, laneId, position, distance);
    }

    double GetLaneDirection(Route route, std::string roadId, int laneId, double position, double distance = 0.0) const override
    {
        return implementation->GetLaneDirection(route, roadId, laneId, position, distance);
    }

    double GetDistanceToEndOfLane(Route route, std::string roadId, int laneId, double initialSearchDistance,
                                  double maxSearchLength) override
    {
        return implementation->GetDistanceToEndOfLane(route, roadId, laneId, initialSearchDistance, maxSearchLength);
    }

    double GetDistanceToEndOfLane(Route route, std::string roadId, int laneId, double initialSearchDistance,
                                  double maxSearchLength, const LaneTypes& laneTypes) override
    {
        return implementation->GetDistanceToEndOfLane(route, roadId, laneId, initialSearchDistance, maxSearchLength, laneTypes);
    }

    bool IntersectsWithAgent(double x, double y, double rotation, double length, double width, double center) override
    {
        return implementation->IntersectsWithAgent(x, y, rotation, length, width, center);
    }

    Position RoadCoord2WorldCoord(RoadPosition roadCoord, std::string roadID = "") const override
    {
        return implementation->RoadCoord2WorldCoord(roadCoord, roadID);
    }

    //-----------------------------------------------------------------------------
    //! Instantiate the world by creating a WorldInterface out of a world library
    //! with given world parameters
    //! @param[in]  worldParameter  parameters of world inclusive library path
    //!
    //! @return                true if successful
    //-----------------------------------------------------------------------------
    bool Instantiate()
    {
        if (!worldBinding)
        {
            return false;
        }
        else
            if (!implementation)
            {
                implementation = worldBinding->Instantiate();
                if (!implementation)
                {
                    return false;
                }
            }
        return true;
    }

    //-----------------------------------------------------------------------------
    //! Returns true if world was already instantiated
    //!
    //! @return                true if world was already instantiated
    //-----------------------------------------------------------------------------
    bool isInstantiated() override
    {
        return implementation;
    }

    virtual std::vector<CommonTrafficSign::Entity> GetTrafficSignsInRange(const Route& route, std::string roadId, int laneId,
            double startDistance, double searchRange) const override
    {
        return implementation->GetTrafficSignsInRange(route, roadId, laneId, startDistance, searchRange);
    }

    virtual std::vector<CommonTrafficSign::Entity> GetRoadMarkingsInRange(const Route& route, std::string roadId, int laneId,
            double startDistance, double searchRange) const override
    {
        return implementation->GetRoadMarkingsInRange(route, roadId, laneId, startDistance, searchRange);
    }

    virtual std::vector<LaneMarking::Entity> GetLaneMarkings(const Route& route, std::string roadId, int laneId, double startDistance, double range, Side side) const override
    {
        return implementation->GetLaneMarkings(route, roadId, laneId, startDistance, range, side);
    }

    RelativeWorldView::Junctions GetRelativeJunctions (const Route& route, std::string roadId, double startDistance, double range) const override
    {
        return implementation->GetRelativeJunctions(route, roadId, startDistance, range);
    }

    RelativeWorldView::Lanes GetRelativeLanes(const Route& route, std::string roadId, int laneId, double distance, double range) const override
    {
        return implementation->GetRelativeLanes(route, roadId, laneId, distance, range);
    }

    std::vector<JunctionConnection> GetConnectionsOnJunction(std::string junctionId, std::string incomingRoadId) const override
    {
        return implementation->GetConnectionsOnJunction(junctionId, incomingRoadId);
    }

    std::vector<IntersectingConnection> GetIntersectingConnections(std::string connectingRoadId) const override
    {
        return implementation->GetIntersectingConnections(connectingRoadId);
    }

    std::vector<JunctionConnectorPriority> GetPrioritiesOnJunction(std::string junctionId) const override
    {
        return implementation->GetPrioritiesOnJunction(junctionId);
    }

    RoadNetworkElement GetRoadSuccessor(std::string roadId) const override
    {
        return implementation->GetRoadSuccessor(roadId) ;
    }

    RoadNetworkElement GetRoadPredecessor(std::string roadId) const override
    {
        return implementation->GetRoadPredecessor(roadId) ;
    }

    virtual Route GetRoute (GlobalRoadPosition start) const override
    {
        return implementation->GetRoute(start);
    }

    virtual Obstruction GetObstruction(const Route& route, const GlobalRoadPosition& ownPosition,
                                         const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners) const override
    {
        return implementation->GetObstruction(route, ownPosition, otherPosition, objectCorners);
    }

    virtual void *GetGlobalDrivingView() override
    {
        return implementation->GetGlobalDrivingView();
    }
    virtual void *GetGlobalObjects() override
    {
        return implementation->GetGlobalObjects();
    }
    virtual void SetTimeOfDay(int timeOfDay) override
    {
        return implementation->SetTimeOfDay(timeOfDay);
    }
    virtual void SetWeekday(Weekday weekday) override
    {
        return implementation->SetWeekday(weekday);
    }
    virtual Weekday GetWeekday() const override
    {
        return implementation->GetWeekday();
    }
    virtual void SetParameter(WorldParameter *worldParameter) override
    {
        return implementation->SetParameter(worldParameter);
    }
    virtual bool CreateGlobalDrivingView() override
    {
        return implementation->CreateGlobalDrivingView();
    }
    virtual const AgentInterface *GetSpecialAgent() override
    {
        return implementation->GetSpecialAgent();
    }
    virtual const AgentInterface *GetLastCarInlane(int laneNumber) override
    {
        return implementation->GetLastCarInlane(laneNumber);
    }
    virtual const AgentInterface *GetBicycle() const override
    {
        return implementation->GetBicycle();
    }
    virtual void QueueAgentUpdate(std::function<void(double)> func,
                                  double val) override
    {
        return implementation->QueueAgentUpdate(func, val);
    }
    virtual bool CreateWorldScenery(const std::string &sceneryFilename) override
    {
        return implementation->CreateWorldScenery(sceneryFilename);
    }
    virtual bool CreateWorldScenario(const std::string &scenarioFilename) override
    {
        return implementation->CreateWorldScenario(scenarioFilename);
    }
    virtual double GetDistanceBetweenObjects(const Route& route, const ObjectPosition& objectPos, const ObjectPosition& targetObjectPos) const override
    {
        return implementation->GetDistanceBetweenObjects(route, objectPos, targetObjectPos);
    }
    virtual std::string GetNextJunctionIdOnRoute(const Route& route, const ObjectPosition& objectPos) const override
    {
        return implementation->GetNextJunctionIdOnRoute(route, objectPos);
    }
    virtual double GetDistanceToJunction(const Route& route, const ObjectPosition& objectPos, const std::string& junctionId) const override
    {
        return implementation->GetDistanceToJunction(route, objectPos, junctionId);
    }

private:
    WorldBinding* worldBinding = nullptr;
    WorldInterface* implementation = nullptr;
};

} // namespace SimulationSlave


