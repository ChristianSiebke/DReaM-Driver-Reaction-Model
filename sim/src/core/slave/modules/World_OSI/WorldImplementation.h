/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
*               2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <algorithm>
#include <qglobal.h>
#include "include/worldInterface.h"
#include "AgentNetwork.h"
#include "SceneryConverter.h"
#include "include/parameterInterface.h"
#include "Localization.h"
#include "include/dataStoreInterface.h"

#include "WorldData.h"
#include "WorldDataQuery.h"

namespace osi3
{
class SensorView;
class SensorViewConfiguration;
}

struct WorldParameterOSI
{
    void Reset()
    {
        timeOfDay = "";
        visibilityDistance = 0;
        friction = 0.0;
        weather = "";
    }

    std::string timeOfDay {""};
    int visibilityDistance {0};
    double friction {0.0};
    std::string weather {""};
};

#include "osi3/osi_groundtruth.pb.h"

/** \addtogroup World
* @{
* \brief implementation of a world interface
*
* The World module implements a WorldInterface which is used
* by the framework and the agents.
* It is used to set up a basic simulated world with static objects like
* roads, intersection and curves and dynamic objects like cars, bicycles and
* pedastrians.
*
* The World handles all simulated objects.
*
* \section world_inputs Inputs
* name | meaning
* -----|---------
* callbacks | CallbackInterface to send log information to the framework
*
* \section world_outputs Outputs
 name | meaning
* -----|---------
* *WoldInterface | Provides a reference to an implementation of a WorldInterface to the framework.
*
* \section world_configParameters Parameters to be specified in runConfiguration.xml
* tag | meaning
* -----|---------
* World | Name of World library. "World" by default.
*
* @see WorldInterface
*
* @} */

/*!
 * \brief Implementation of a WorldInterface
 *
 * This class implements all function of the WorldInterface. It is responsible for all
 * dynamic and static objects in a given scenery.
 *
 * \ingroup World
 */
class WorldImplementation : public WorldInterface
{
public:
    const std::string MODULENAME = "WORLD";

    WorldImplementation(const CallbackInterface* callbacks, StochasticsInterface* stochastics, DataStoreWriteInterface* dataStore);
    WorldImplementation(const WorldImplementation&) = delete;
    WorldImplementation(WorldImplementation&&) = delete;
    WorldImplementation& operator=(const WorldImplementation&) = delete;
    WorldImplementation& operator=(WorldImplementation&&) = delete;

    virtual ~WorldImplementation() override;

    bool AddAgent(int id, AgentInterface* agent) override;
    AgentInterface* GetAgent(int id) const override;
    const std::vector<const WorldObjectInterface*>& GetWorldObjects() const override;
    const std::map<int, AgentInterface *> &GetAgents() const override;
    const std::list<const AgentInterface*>& GetRemovedAgents() const override;
    const std::list<const AgentInterface*> GetRemovedAgentsInPreviousTimestep() override;

    const std::vector<const TrafficObjectInterface*>& GetTrafficObjects() const override;

    // framework internal methods to access members without restrictions
    void ExtractParameter(ParameterInterface* parameters) override;

    void Reset() override;
    void Clear() override;

    // model callbacks
    std::string GetTimeOfDay() const override;

    void* GetWorldData() override;
    void* GetOsiGroundTruth() override;

    void QueueAgentUpdate(std::function<void()> func) override;
    void QueueAgentRemove(const AgentInterface* agent) override;
    void RemoveAgent(const AgentInterface* agent);

    void PublishGlobalData(int timestamp) override;
    void SyncGlobalData() override;

    bool CreateScenery(SceneryInterface* scenery, const openScenario::EnvironmentAction& environment) override;

    AgentInterface* CreateAgentAdapterForAgent() override;

    AgentInterface* GetEgoAgent() override;

    AgentInterface* GetAgentByName(const std::string& scenarioName) override;

    RouteQueryResult<std::vector<const AgentInterface*>> GetAgentsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance,
                                                                          double backwardRange, double forwardRange) const override;

    RouteQueryResult<std::vector<const WorldObjectInterface*>> GetObjectsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance,
                                                                                 double backwardRange, double forwardRange) const override;
    std::vector<const AgentInterface*> GetAgentsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const override;
    
    double GetDistanceToConnectorEntrance(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override;
    double GetDistanceToConnectorDeparture(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override;
    
    Position LaneCoord2WorldCoord(double distanceOnLane, double offset, std::string roadId,
                                          int laneId) const override;

    bool IsSValidOnLane(std::string roadId, int laneId, double distance) override;

    bool IsDirectionalRoadExisting(const std::string& roadId, bool inOdDirection) override;

    double GetLaneCurvature(std::string roadId, int laneId, double position) const override;
    RouteQueryResult<std::optional<double> > GetLaneCurvature(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const override;

    double GetLaneWidth(std::string roadId, int laneId, double position) const override;
    RouteQueryResult<std::optional<double> > GetLaneWidth(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const override;

    double GetLaneDirection(std::string roadId, int laneId, double position) const override;
    RouteQueryResult<std::optional<double> > GetLaneDirection(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const override;

    RouteQueryResult<double> GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance,
                                  double maximumSearchLength) const override;

    RouteQueryResult<double> GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance,
                                  double maximumSearchLength, const LaneTypes& laneTypes) const override;

    RouteQueryResult<LongitudinalDistance> GetDistanceBetweenObjects(const RoadGraph& roadGraph, RoadGraphVertex startNode, const ObjectPosition& objectPos, const std::optional<double> objectReferenceS, const ObjectPosition& targetObjectPos) const override;

    bool IntersectsWithAgent(double x, double y, double rotation, double length, double width, double center) override;

    Position RoadCoord2WorldCoord(RoadPosition roadCoord, std::string roadID) const override;

    double GetRoadLength(const std::string& roadId) const override;

    double GetVisibilityDistance() const override;

    RouteQueryResult<Obstruction> GetObstruction(const RoadGraph& roadGraph, RoadGraphVertex startNode, const GlobalRoadPosition& ownPosition,
                                                 const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners, const Common::Vector2d& mainLaneLocator) const override;

    RouteQueryResult<std::vector<CommonTrafficSign::Entity>> GetTrafficSignsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId,
                                                                                    double startDistance, double searchRange) const override;

    RouteQueryResult<std::vector<CommonTrafficSign::Entity>> GetRoadMarkingsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId,
                                                                                    double startDistance, double searchRange) const override;

    RouteQueryResult<std::vector<LaneMarking::Entity>> GetLaneMarkings(const RoadGraph& roadGraph, RoadGraphVertex startNode,
                                                                       int laneId, double startDistance, double range, Side side) const override;

    RouteQueryResult<RelativeWorldView::Junctions> GetRelativeJunctions (const RoadGraph& roadGraph, RoadGraphVertex startNode, double startDistance, double range) const override;

    RouteQueryResult<RelativeWorldView::Lanes> GetRelativeLanes(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double distance, double range) const override;

    std::vector<JunctionConnection> GetConnectionsOnJunction(std::string junctionId, std::string incomingRoadId) const override;

    std::vector<IntersectingConnection> GetIntersectingConnections(std::string connectingRoadId) const override;

    std::vector<JunctionConnectorPriority> GetPrioritiesOnJunction(std::string junctionId) const override;
    
    RoadNetworkElement GetRoadSuccessor(std::string roadId) const override;

    RoadNetworkElement GetRoadPredecessor(std::string roadId) const override;

    std::pair<RoadGraph, RoadGraphVertex> GetRoadGraph (const RouteElement& start, int maxDepth) const override;

    std::map<RoadGraphEdge, double> GetEdgeWeights (const RoadGraph& roadGraph) const override;

    double GetFriction() const override;

    virtual void *GetGlobalDrivingView() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void *GetGlobalObjects() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void SetTimeOfDay(int timeOfDay) override
    {
        Q_UNUSED(timeOfDay);

        throw std::runtime_error("not implemented");
    }
    virtual void SetWeekday(Weekday weekday) override
    {
        Q_UNUSED(weekday);

        throw std::runtime_error("not implemented");
    }
    virtual Weekday GetWeekday() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void SetParameter(WorldParameter *worldParameter) override
    {
        Q_UNUSED(worldParameter);

        throw std::runtime_error("not implemented");
    }
    virtual bool CreateGlobalDrivingView() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual const AgentInterface *GetSpecialAgent() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual const AgentInterface *GetLastCarInlane(int laneNumber) override
    {
        Q_UNUSED(laneNumber);

        throw std::runtime_error("not implemented");
    }
    virtual const AgentInterface *GetBicycle() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void QueueAgentUpdate(std::function<void(double)> func,
                                  double val) override
    {
        Q_UNUSED(func);
        Q_UNUSED(val);

        throw std::runtime_error("not implemented");
    }
    virtual bool CreateWorldScenery(const  std::string &sceneryFilename) override
    {
        Q_UNUSED(sceneryFilename);

        throw std::runtime_error("not implemented");
    }
    virtual bool CreateWorldScenario(const  std::string &scenarioFilename) override
    {
        Q_UNUSED(scenarioFilename);

        throw std::runtime_error("not implemented");
    }
protected:
    //-----------------------------------------------------------------------------
    //! Provides callback to LOG() macro
    //!
    //! @param[in]     logLevel    Importance of log
    //! @param[in]     file        Name of file where log is called
    //! @param[in]     line        Line within file where log is called
    //! @param[in]     message     Message to log
    //-----------------------------------------------------------------------------
    void Log(CbkLogLevel logLevel,
             const char* file,
             int line,
             const std::string& message)
    {
        if (callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

private:
    void InitTrafficObjects();

    OWL::WorldData worldData;
    WorldDataQuery worldDataQuery{worldData};
    World::Localization::Localizer localizer{worldData};

    std::vector<const TrafficObjectInterface*> trafficObjects;

    // world parameters
    WorldParameterOSI worldParameter;

    AgentNetwork agentNetwork;

    const CallbackInterface* callbacks;

    mutable std::vector<const WorldObjectInterface*> worldObjects;

    const int stepSizeLookingForValidS = 100;
    SceneryInterface* scenery;

    std::unordered_map<const OWL::Interfaces::MovingObject*, AgentInterface*> movingObjectMapping{{nullptr, nullptr}};
    std::unordered_map<const OWL::Interfaces::MovingObject*, TrafficObjectInterface*> stationaryObjectMapping{{nullptr, nullptr}};

    DataStoreWriteInterface* dataStore;
};
