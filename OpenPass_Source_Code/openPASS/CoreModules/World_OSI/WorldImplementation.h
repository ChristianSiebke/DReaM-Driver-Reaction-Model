/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
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
#include "Interfaces/worldInterface.h"
#include "AgentNetwork.h"
#include "SceneryConverter.h"
#include "Interfaces/parameterInterface.h"
#include "RoutePlanning/StochasticNavigation.h"
#include "Localization.h"

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

#include "osi/osi_groundtruth.pb.h"

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

    WorldImplementation(const CallbackInterface* callbacks, StochasticsInterface* stochastics);
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
    void SyncGlobalData() override;

    bool CreateScenery(SceneryInterface* scenery) override;

    AgentInterface* CreateAgentAdapterForAgent() override;

    AgentInterface* GetEgoAgent() override;

    AgentInterface* GetAgentByName(const std::string& scenarioName) override;

    std::vector<const AgentInterface*> GetAgentsInRange(Route route, std::string roadId, int laneId, double startDistance,
                                                        double backwardRange, double forwardRange) const override;
    std::vector<const WorldObjectInterface*> GetObjectsInRange(Route route, std::string roadId, int laneId, double startDistance,
                                                               double backwardRange, double forwardRange) const override;
    std::vector<const AgentInterface*> GetAgentsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const override;
    
    double GetDistanceToConnectorEntrance(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override;
    double GetDistanceToConnectorDeparture(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override;
    
    Position LaneCoord2WorldCoord(double distanceOnLane, double offset, std::string roadId,
                                          int laneId) const override;

    bool IsSValidOnLane(std::string roadId, int laneId, double distance) override;


    double GetLaneCurvature(Route route, std::string roadId, int laneId, double position, double distance = 0.0) const override;
    double GetLaneWidth(Route route, std::string roadId, int laneId, double position, double distance = 0.0) const override;
    double GetLaneDirection(Route route, std::string roadId, int laneId, double position, double distance = 0.0) const override;

    double GetDistanceToEndOfLane(Route route, std::string roadId, int laneId, double initialSearchDistance,
                                  double maximumSearchLength) override;
    double GetDistanceToEndOfLane(Route route, std::string roadId, int laneId, double initialSearchDistance,
                                  double maximumSearchLength, const LaneTypes& laneTypes) override;

    double GetDistanceBetweenObjects(const Route& route, const ObjectPosition& objectPos, const ObjectPosition& targetObjectPos) const override;

    bool IntersectsWithAgent(double x, double y, double rotation, double length, double width, double center) override;

    Position RoadCoord2WorldCoord(RoadPosition roadCoord, std::string roadID) const override;

    double GetVisibilityDistance() const override;

    Obstruction GetObstruction(const Route& route, const GlobalRoadPosition& ownPosition,
                                         const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners) const override;

    std::vector<CommonTrafficSign::Entity> GetTrafficSignsInRange(const Route& route, std::string roadId, int laneId, double startDistance,
            double searchRange) const override;

    virtual std::vector<LaneMarking::Entity> GetLaneMarkings(const Route& route, std::string roadId, int laneId, double startDistance, double range, Side side) const override;

    RelativeWorldView::Junctions GetRelativeJunctions (const Route& route, std::string roadId, double startDistance, double range) const override;

    RelativeWorldView::Lanes GetRelativeLanes(const Route& route, std::string roadId, int laneId, double distance, double range) const override;

    std::vector<JunctionConnection> GetConnectionsOnJunction(std::string junctionId, std::string incomingRoadId) const override;

    std::vector<IntersectingConnection> GetIntersectingConnections(std::string connectingRoadId) const override;

    std::vector<JunctionConnectorPriority> GetPrioritiesOnJunction(std::string junctionId) const override;
    
    RoadNetworkElement GetRoadSuccessor(std::string roadId) const override;

    RoadNetworkElement GetRoadPredecessor(std::string roadId) const override;

    virtual Route GetRoute (GlobalRoadPosition start) const override;

    double GetFriction() const override;

    std::string GetNextJunctionIdOnRoute(const Route& route, const ObjectPosition& objectPos) const override;

    double GetDistanceToJunction(const Route& route, const ObjectPosition& objectPos, const std::string& junctionId) const override;

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

    StochasticNavigation navigation;
};
