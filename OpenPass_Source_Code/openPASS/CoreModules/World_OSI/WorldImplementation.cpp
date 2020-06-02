/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
*               2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "WorldData.h"
#include "WorldImplementation.h"

#include "osi3/osi_sensorview.pb.h"
#include "osi3/osi_sensorviewconfiguration.pb.h"

namespace {
    template <typename T>
    static std::vector<const T*> get_transformed(const std::vector<const OWL::Interfaces::WorldObject*>& worldObjects)
    {
        std::vector<const T*> transformedContainer;
        std::transform(worldObjects.begin(), worldObjects.end(), std::back_inserter(transformedContainer),
                       [](const OWL::Interfaces::WorldObject* object){ return object->GetLink<T>(); });
        return transformedContainer;
    }
}

WorldImplementation::WorldImplementation(const CallbackInterface* callbacks, StochasticsInterface* stochastics):
    agentNetwork(this, callbacks),
    callbacks(callbacks),
    navigation(*stochastics, worldData)
{}

WorldImplementation::~WorldImplementation()
{
}

bool WorldImplementation::AddAgent(int id, AgentInterface* agent)
{
    if (agentNetwork.AddAgent(id, agent))
    {
        worldObjects.push_back(agent);
        return true;
    }

    return false;
}

AgentInterface *WorldImplementation::GetAgent(int id) const
{
    return agentNetwork.GetAgent(id);
}

const std::vector<const WorldObjectInterface*>& WorldImplementation::GetWorldObjects() const
{
    return worldObjects;
}

const std::map<int, AgentInterface *> &WorldImplementation::GetAgents() const
{
    return agentNetwork.GetAgents();
}

const std::list<const AgentInterface*>& WorldImplementation::GetRemovedAgents() const
{
    return agentNetwork.GetRemovedAgents();
}

const std::vector<const TrafficObjectInterface*>& WorldImplementation::GetTrafficObjects() const
{
    return trafficObjects;
}

void WorldImplementation::ExtractParameter(ParameterInterface* parameters)
{
    auto intParameter = parameters->GetParametersInt();
    auto doubleParameter = parameters->GetParametersDouble();
    auto stringParameter = parameters->GetParametersString();

    worldParameter.timeOfDay = stringParameter.at("TimeOfDay");
    worldParameter.visibilityDistance = intParameter.at("VisibilityDistance");
    worldParameter.friction = doubleParameter.at("Friction");
    worldParameter.weather = stringParameter.at("Weather");
}

void WorldImplementation::Reset()
{
    auto lanes = worldData.GetLanes();
    for (auto lane : lanes)
    {
        lane.second->ClearMovingObjects();
    }
    worldData.Reset();
    worldParameter.Reset();
    agentNetwork.Clear();
    worldObjects.clear();
    worldObjects.insert(worldObjects.end(), trafficObjects.begin(), trafficObjects.end());
}

void WorldImplementation::Clear()
{
    for (auto worldObject : worldObjects)
    {
        delete worldObject;
    }
}

void* WorldImplementation::GetWorldData()
{
    return &worldData;
}

void* WorldImplementation::GetOsiGroundTruth()
{
    //    return &groundTruth;
    return nullptr;
}

void WorldImplementation::QueueAgentUpdate(std::function<void()> func)
{
    agentNetwork.QueueAgentUpdate(func);
}

void WorldImplementation::QueueAgentRemove(const AgentInterface* agent)
{
    agentNetwork.QueueAgentRemove(agent);

    auto it = std::find(worldObjects.begin(), worldObjects.end(), agent);
    if (it != worldObjects.end())
    {
        worldObjects.erase(it);
    }
}

void WorldImplementation::SyncGlobalData()
{
    auto lanes = worldData.GetLanes();
    for (auto lane : lanes)
    {
        lane.second->ClearMovingObjects();
    }
    agentNetwork.SyncGlobalData();
}

bool WorldImplementation::CreateScenery(SceneryInterface* scenery)
{
    this->scenery = scenery;

    SceneryConverter converter(scenery,
                               worldData,
                               localizer,
                               callbacks);
    if (converter.Convert())
    {
        InitTrafficObjects();
        localizer.Init();
        return true;
    }

    return false;
}

AgentInterface* WorldImplementation::CreateAgentAdapterForAgent()
{
    AgentInterface* agentAdapter = new AgentAdapter(this, callbacks, &worldData, localizer);

    return agentAdapter;
}

std::string WorldImplementation::GetTimeOfDay() const
{
    return worldParameter.timeOfDay;
}

double WorldImplementation::GetVisibilityDistance() const
{
    return worldParameter.visibilityDistance;
}

double WorldImplementation::GetFriction() const
{
    return worldParameter.friction;
}

std::vector<CommonTrafficSign::Entity> WorldImplementation::GetTrafficSignsInRange(const Route& route, std::string roadId, int laneId,
        double startDistance, double searchRange) const
{
    std::vector<CommonTrafficSign::Entity> foundSigns {};
    auto& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, startDistance);
    if (!lane.Exists())
    {
        return {};
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, startDistance);
    if (searchRange < 0.0)
    {
        laneStream = laneStream.Reverse();
        searchRange = -searchRange;
    }
    startDistance = laneStream.GetPositionByElementAndS(lane, startDistance);

    const auto& signs = worldDataQuery.GetTrafficSignsInRange(laneStream,
                                                       startDistance,
                                                       searchRange);
    for (auto& sign : signs)
    {
        foundSigns.push_back(sign.second->GetSpecification(sign.first));
    }

    return foundSigns;
}

std::vector<LaneMarking::Entity> WorldImplementation::GetLaneMarkings(const Route& route, std::string roadId, int laneId, double startDistance, double range, Side side) const
{
     auto& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, startDistance);
     if (!lane.Exists())
     {
         return {};
     }
     auto laneStream = navigation.GetLaneStream(route, roadId, laneId, startDistance);
     if (range < 0.0)
     {
         laneStream = laneStream.Reverse();
         range = -range;
     }
     startDistance = laneStream.GetPositionByElementAndS(lane, startDistance);

     return worldDataQuery.GetLaneMarkings(laneStream,
                                           startDistance,
                                           range,
                                           side);
}

RelativeWorldView::Junctions WorldImplementation::GetRelativeJunctions(const Route& route, std::string roadId, double startDistance, double range) const
{
    const auto road = worldDataQuery.GetRoadByOdId(roadId);
    const auto& roadStream = navigation.GetRoadStream(route);
    const auto& positionOnStream = roadStream.GetPositionByElementAndS(*road, startDistance);
    return worldDataQuery.GetRelativeJunctions(roadStream, positionOnStream, range);
}

std::vector<JunctionConnection> WorldImplementation::GetConnectionsOnJunction(std::string junctionId, std::string incomingRoadId) const
{
    return worldDataQuery.GetConnectionsOnJunction(junctionId, incomingRoadId);
}

std::vector<IntersectingConnection> WorldImplementation::GetIntersectingConnections(std::string connectingRoadId) const
{
    return worldDataQuery.GetIntersectingConnections(connectingRoadId);
}

std::vector<JunctionConnectorPriority> WorldImplementation::GetPrioritiesOnJunction(std::string junctionId) const
{
    return worldDataQuery.GetPrioritiesOnJunction(junctionId);
}

RoadNetworkElement WorldImplementation::GetRoadSuccessor(std::string roadId) const
{
    return worldDataQuery.GetRoadSuccessor(roadId);
}

RoadNetworkElement WorldImplementation::GetRoadPredecessor(std::string roadId) const
{
    return worldDataQuery.GetRoadPredecessor(roadId);
}

Route WorldImplementation::GetRoute(GlobalRoadPosition start) const
{
    return navigation.GetRoute(start);
}

AgentInterface* WorldImplementation::GetEgoAgent()
{
    const std::map<int, AgentInterface*> agents = agentNetwork.GetAgents();

    for (auto iterator = agents.begin(); iterator != agents.end(); iterator++)
    {
        AgentInterface* agent = iterator->second;
        if (agent->IsEgoAgent())
        {
            return agent;
        }
    }

    return nullptr;
}

AgentInterface* WorldImplementation::GetAgentByName(const std::string& scenarioName)
{
    const std::map<int, AgentInterface*> agents = agentNetwork.GetAgents();

    for (auto iterator = agents.begin(); iterator != agents.end(); iterator++)
    {
        AgentInterface* agent = iterator->second;
        if (agent->GetScenarioName() == (scenarioName))
        {
            return agent;
        }
    }

    return nullptr;
}

// TODO JT check and remove this method
std::vector<AgentInterface*> WorldImplementation::GetAgentsByGroupType(const AgentCategory& agentCategory)
{
    const std::map<int, AgentInterface*> agents = agentNetwork.GetAgents();

    std::vector<AgentInterface*> agentsWithGroupType;

    for (auto iterator = agents.begin(); iterator != agents.end(); iterator++)
    {
        AgentInterface* agent = iterator->second;
        if (agentCategory == AgentCategory::Any  || agent->GetAgentCategory() == agentCategory)
        {
            agentsWithGroupType.push_back(agent);
        }
    }

    return agentsWithGroupType;
}

//// Agent functions
AgentInterface* WorldImplementation::GetNextAgentInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const
{
    return GetNextAgentInLane(route, roadId, laneId, currentDistance, searchInForwardDirection, OWL::EVENTHORIZON);
}

AgentInterface* WorldImplementation::GetNextAgentInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection, double searchDistance) const
{
    auto& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, currentDistance);
    if (!lane.Exists())
    {
        return nullptr;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, currentDistance);
    if (!searchInForwardDirection)
    {
        laneStream = laneStream.Reverse();
    }
    double startDistance = laneStream.GetPositionByElementAndS(lane, currentDistance);

    auto worldObject = worldDataQuery.GetNextObjectInLane<OWL::Interfaces::MovingObject>(laneStream,
                                                                                        startDistance,
                                                                                        searchDistance);
    return worldObject ? worldObject->GetLink<AgentInterface>() : nullptr;
}

AgentInterface* WorldImplementation::GetLastAgentInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const
{
    return GetLastAgentInLane(route, roadId, laneId, currentDistance, searchInForwardDirection, OWL::EVENTHORIZON);
}

AgentInterface* WorldImplementation::GetLastAgentInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection, double searchDistance) const
{
    auto& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, currentDistance);
    if (!lane.Exists())
    {
        return nullptr;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, currentDistance);
    if (!searchInForwardDirection)
    {
        laneStream = laneStream.Reverse();
    }
    double startDistance = laneStream.GetPositionByElementAndS(lane, currentDistance);

    auto worldObject = worldDataQuery.GetLastObjectInLane<OWL::Interfaces::MovingObject>(laneStream,
                                                                                        startDistance,
                                                                                        searchDistance);

    return worldObject ? worldObject->GetLink<AgentInterface>() : nullptr;
}

// Obstacle functions
TrafficObjectInterface* WorldImplementation::GetNextTrafficObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const
{
    return GetNextTrafficObjectInLane(route, roadId, laneId, currentDistance, searchInForwardDirection, OWL::EVENTHORIZON);
}

TrafficObjectInterface* WorldImplementation::GetNextTrafficObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection, double searchDistance) const
{
    auto& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, currentDistance);
    if (!lane.Exists())
    {
        return nullptr;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, currentDistance);
    if (!searchInForwardDirection)
    {
        laneStream = laneStream.Reverse();
    }
    double startDistance = laneStream.GetPositionByElementAndS(lane, currentDistance);

    auto worldObject = worldDataQuery.GetNextObjectInLane<OWL::Interfaces::StationaryObject>(laneStream,
                                                                                        startDistance,
                                                                                        searchDistance);

    return worldObject ? worldObject->GetLink<TrafficObjectInterface>() : nullptr;
}

TrafficObjectInterface* WorldImplementation::GetLastTrafficObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const
{
    return GetLastTrafficObjectInLane(route, roadId, laneId, currentDistance, searchInForwardDirection, OWL::EVENTHORIZON);
}

TrafficObjectInterface* WorldImplementation::GetLastTrafficObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection, double searchDistance) const
{
    auto& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, currentDistance);
    if (!lane.Exists())
    {
        return nullptr;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, currentDistance);
    if (!searchInForwardDirection)
    {
        laneStream = laneStream.Reverse();
    }
    double startDistance = laneStream.GetPositionByElementAndS(lane, currentDistance);

    auto worldObject = worldDataQuery.GetLastObjectInLane<OWL::Interfaces::StationaryObject>(laneStream,
                                                                                        startDistance,
                                                                                        searchDistance);

    return worldObject ? worldObject->GetLink<TrafficObjectInterface>() : nullptr;
}

// Generic functions
WorldObjectInterface *WorldImplementation::GetNextObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection) const
{
    return GetNextObjectInLane(route, roadId, laneId, currentDistance, searchInForwardDirection, OWL::EVENTHORIZON);
}

WorldObjectInterface *WorldImplementation::GetNextObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection, double searchDistance) const
{
    auto& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, currentDistance);
    if (!lane.Exists())
    {
        return nullptr;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, currentDistance);
    if (!searchInForwardDirection)
    {
        laneStream = laneStream.Reverse();
    }
    double startDistance = laneStream.GetPositionByElementAndS(lane, currentDistance);

    auto worldObject = worldDataQuery.GetNextObjectInLane<OWL::Interfaces::WorldObject>(laneStream,
                                                                                        startDistance,
                                                                                        searchDistance);
    return worldObject ? worldObject->GetLink<WorldObjectInterface>() : nullptr;
}

WorldObjectInterface* WorldImplementation::GetLastObjectInLane(Route route, std::string roadId, int laneId,
        double currentDistance, bool searchInForwardDirection) const
{
    return GetLastObjectInLane(route, roadId, laneId, currentDistance, searchInForwardDirection, OWL::EVENTHORIZON);
}

WorldObjectInterface* WorldImplementation::GetLastObjectInLane(Route route, std::string roadId, int laneId, double currentDistance, bool searchInForwardDirection,
        double searchDistance) const
{
    auto& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, currentDistance);
    if (!lane.Exists())
    {
        return nullptr;
    }

    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, currentDistance);
    if (!searchInForwardDirection)
    {
        laneStream = laneStream.Reverse();
    }

    double startDistance = laneStream.GetPositionByElementAndS(lane, currentDistance);
    auto worldObject = worldDataQuery.GetLastObjectInLane<OWL::Interfaces::WorldObject>(laneStream,
                                                                                        startDistance,
                                                                                        searchDistance);

    return worldObject ? worldObject->GetLink<WorldObjectInterface>() : nullptr;
}

Obstruction WorldImplementation::GetObstruction(const Route& route, const GlobalRoadPosition& ownPosition, const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners) const
{
    auto& lane = worldDataQuery.GetLaneByOdId(ownPosition.roadId, ownPosition.laneId, ownPosition.roadPosition.s);
    if (!lane.Exists())
    {
        return Obstruction::Invalid();
    }
    auto laneStream = navigation.GetLaneStream(route, ownPosition);
    return worldDataQuery.GetObstruction(laneStream, ownPosition.roadPosition.t, otherPosition, objectCorners);
}

RelativeWorldView::Lanes WorldImplementation::GetRelativeLanes(const Route& route, std::string roadId, int laneId, double distance, double range) const
{
    auto road = worldDataQuery.GetRoadByOdId(roadId);
    if (!road)
    {
        return {};
    }
    auto roadStream = navigation.GetRoadStream(route);
    double startDistanceOnStream = roadStream.GetPositionByElementAndS(*road, distance);

    return worldDataQuery.GetRelativeLanes(roadStream, startDistanceOnStream, laneId, range);
}

std::vector<const AgentInterface*> WorldImplementation::GetAgentsInRange(Route route, std::string roadId, int laneId, double startDistance,
                                                                         double backwardRange, double forwardRange) const
{
    auto& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, startDistance);
    if (!lane.Exists())
    {
        return {};
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, startDistance);
    double startDistanceOnStream = laneStream.GetPositionByElementAndS(lane, startDistance);

    auto movingObjects = worldDataQuery.GetObjectsOfTypeInRange<OWL::Interfaces::MovingObject>(laneStream, startDistanceOnStream - backwardRange, startDistanceOnStream + forwardRange);
    return get_transformed<AgentInterface>(movingObjects);
}

std::vector<const WorldObjectInterface*> WorldImplementation::GetObjectsInRange(Route route, std::string roadId, int laneId, double startDistance,
                                                                                double backwardRange, double forwardRange) const
{
    auto& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, startDistance);
    if (!lane.Exists())
    {
        return {};
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, startDistance);
    double startDistanceOnStream = laneStream.GetPositionByElementAndS(lane, startDistance);

    auto objects = worldDataQuery.GetObjectsOfTypeInRange<OWL::Interfaces::WorldObject>(laneStream, startDistanceOnStream - backwardRange, startDistanceOnStream + forwardRange);
    return get_transformed<WorldObjectInterface>(objects);
}

std::vector<const AgentInterface *> WorldImplementation::GetAgentsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const
{
    auto movingObjects = worldDataQuery.GetMovingObjectsInRangeOfJunctionConnection(connectingRoadId, range);
    return get_transformed<AgentInterface>(movingObjects);
}

double WorldImplementation::GetDistanceToConnectorEntrance(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return worldDataQuery.GetDistanceUntilObjectEntersConnector(position, intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

double WorldImplementation::GetDistanceToConnectorDeparture(const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return worldDataQuery.GetDistanceUntilObjectLeavesConnector(position, intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

Position WorldImplementation::LaneCoord2WorldCoord(double distanceOnLane, double offset, std::string roadId,
        int laneId) const
{
    OWL::CLane& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, distanceOnLane);
    return worldDataQuery.GetPositionByDistanceAndLane(lane, distanceOnLane, offset);
}

bool WorldImplementation::GetNextValidSOnLane(std::string roadId, int laneId, double distance, double& nextValidS)
{
    nextValidS = worldDataQuery.GetNextValidSOnLaneInDownstream(roadId, laneId, distance, stepSizeLookingForValidS);

    if (nextValidS == INFINITY)
    {
        return false;
    }

    return true;
}

bool WorldImplementation::GetLastValidSOnLane(std::string roadId, int laneId, double distance, double& nextValidS)
{
    nextValidS = worldDataQuery.GetLastValidSInUpstream(roadId, laneId, distance, stepSizeLookingForValidS);

    if (nextValidS == -INFINITY)
    {
        return false;
    }

    return true;
}

bool WorldImplementation::IsSValidOnLane(std::string roadId, int laneId,
        double distance) //when necessary optional parameter with reference to get point
{
    return worldDataQuery.IsSValidOnLane(roadId, laneId, distance);
}

bool WorldImplementation::ExistsLaneLeft(std::string roadId, int laneId, double distance)
{
    return worldDataQuery.ExistsDrivingLaneOnSide(roadId, laneId, distance, Side::Left);
}

bool WorldImplementation::ExistsLaneRight(std::string roadId, int laneId, double distance)
{
    return worldDataQuery.ExistsDrivingLaneOnSide(roadId, laneId, distance, Side::Right);
}

int WorldImplementation::GetNumberOfLanes(std::string roadId, double distance)
{
    return worldDataQuery.GetNumberOfLanes(roadId, distance);
}

double WorldImplementation::GetLaneCurvature(Route route, std::string roadId, int laneId, double position, double distance) const
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, position);
    if (!lane.Exists())
    {
        return 0.0;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, position);
    double positionOnLaneStream = laneStream.GetPositionByElementAndS(lane, position);
    auto [relativePositionOnLane, laneInGivenDistance] = laneStream.GetElementAndSByPosition(positionOnLaneStream + distance);
    if (!laneInGivenDistance)
    {
        return 0.0;
    }
    return laneInGivenDistance->GetCurvature(laneInGivenDistance->GetDistance(OWL::MeasurementPoint::RoadStart) + relativePositionOnLane);
}

double WorldImplementation::GetLaneWidth(Route route, std::string roadId, int laneId, double position, double distance) const
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, position);
    if (!lane.Exists())
    {
        return 0.0;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, position);
    double positionOnLaneStream = laneStream.GetPositionByElementAndS(lane, position);
    auto [relativePositionOnLane, laneInGivenDistance] = laneStream.GetElementAndSByPosition( positionOnLaneStream + distance);
    if (!laneInGivenDistance)
    {
        return 0.0;
    }
    return laneInGivenDistance->GetWidth(laneInGivenDistance->GetDistance(OWL::MeasurementPoint::RoadStart) + relativePositionOnLane);
}

double WorldImplementation::GetLaneDirection(Route route, std::string roadId, int laneId, double position, double distance) const
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, position);
    if (!lane.Exists())
    {
        return 0.0;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, position);
    double positionOnLaneStream = laneStream.GetPositionByElementAndS(lane, position);
    auto [relativePositionOnLane, laneInGivenDistance] = laneStream.GetElementAndSByPosition( positionOnLaneStream + distance);
    if (!laneInGivenDistance)
    {
        return 0.0;
    }
    return laneInGivenDistance->GetDirection(laneInGivenDistance->GetDistance(OWL::MeasurementPoint::RoadStart) + relativePositionOnLane);
}

double WorldImplementation::GetDistanceToEndOfLane(Route route, std::string roadId, int laneId, double initialSearchDistance,
        double maximumSearchLength)
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, initialSearchDistance);
    if (!lane.Exists())
    {
        return 0.0;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, initialSearchDistance);
    double positionOnLaneStream = laneStream.GetPositionByElementAndS(lane, initialSearchDistance);
    return worldDataQuery.GetDistanceToEndOfLane(laneStream, positionOnLaneStream,
            maximumSearchLength, {LaneType::Driving, LaneType::Exit, LaneType::OnRamp, LaneType::OffRamp, LaneType::Stop});
}

double WorldImplementation::GetDistanceToEndOfDrivingLane(Route route, std::string roadId, int laneId, double initialSearchDistance,
        double maximumSearchLength)
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, initialSearchDistance);
    if (!lane.Exists())
    {
        return 0.0;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, initialSearchDistance);
    double positionOnLaneStream = laneStream.GetPositionByElementAndS(lane, initialSearchDistance);
    return worldDataQuery.GetDistanceToEndOfLane(laneStream, positionOnLaneStream,
            maximumSearchLength, {LaneType::Driving});
}

double WorldImplementation::GetDistanceToEndOfDrivingOrStopLane(Route route, std::string roadId, int laneId,
        double initialSearchDistance, double maximumSearchLength)
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, initialSearchDistance);
    if (!lane.Exists())
    {
        return 0.0;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, initialSearchDistance);
    double positionOnLaneStream = laneStream.GetPositionByElementAndS(lane, initialSearchDistance);
    return worldDataQuery.GetDistanceToEndOfLane(laneStream, positionOnLaneStream,
            maximumSearchLength, {LaneType::Driving, LaneType::Stop});
}

double WorldImplementation::GetDistanceToEndOfRamp(Route route, std::string roadId, int laneId, double initialSearchDistance,
        double maximumSearchLength)
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, initialSearchDistance);
    if (!lane.Exists())
    {
        return 0.0;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, initialSearchDistance);
    double positionOnLaneStream = laneStream.GetPositionByElementAndS(lane, initialSearchDistance);
    return worldDataQuery.GetDistanceToEndOfLane(laneStream, positionOnLaneStream,
            maximumSearchLength, {LaneType::OnRamp, LaneType::OffRamp});
}

double WorldImplementation::GetDistanceToEndOfExit(Route route, std::string roadId, int laneId, double initialSearchDistance,
        double maximumSearchLength)
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, initialSearchDistance);
    if (!lane.Exists())
    {
        return 0.0;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, initialSearchDistance);
    double positionOnLaneStream = laneStream.GetPositionByElementAndS(lane, initialSearchDistance);
    return worldDataQuery.GetDistanceToEndOfLane(laneStream, positionOnLaneStream,
            maximumSearchLength, {LaneType::Exit});
}

std::vector<int> WorldImplementation::GetDrivingLanesAtDistance(std::string roadId, double distance) const
{
    std::vector<int> result;
    const auto drivingLanes = worldDataQuery.GetLanesOfLaneTypeAtDistance(roadId, distance, {LaneType::Driving});

    for (const auto lane : drivingLanes)
    {
        OWL::OdId laneId = worldData.GetLaneIdMapping().at(lane->GetId());
        result.push_back(static_cast<int>(laneId));
    }

    return result;
}

std::vector<int> WorldImplementation::GetStopLanesAtDistance(std::string roadId, double distance) const
{
    std::vector<int> result;
    const auto stopLanes = worldDataQuery.GetLanesOfLaneTypeAtDistance(roadId, distance, {LaneType::Stop});

    for (const auto lane : stopLanes)
    {
        OWL::OdId laneId = worldData.GetLaneIdMapping().at(lane->GetId());
        result.push_back(static_cast<int>(laneId));
    }

    return result;
}

std::vector<int> WorldImplementation::GetExitLanesAtDistance(std::string roadId, double distance) const
{
    std::vector<int> result;
    const auto stopLanes = worldDataQuery.GetLanesOfLaneTypeAtDistance(roadId, distance, {LaneType::Exit});

    for (const auto lane : stopLanes)
    {
        OWL::OdId laneId = worldData.GetLaneIdMapping().at(lane->GetId());
        result.push_back(static_cast<int>(laneId));
    }

    return result;
}

std::vector<int> WorldImplementation::GetRampsAtDistance(std::string roadId, double distance) const
{
    std::vector<int> result;
    const auto stopLanes = worldDataQuery.GetLanesOfLaneTypeAtDistance(roadId, distance, {LaneType::OnRamp, LaneType::OffRamp});

    for (const auto lane : stopLanes)
    {
        OWL::OdId laneId = worldData.GetLaneIdMapping().at(lane->GetId());
        result.push_back(static_cast<int>(laneId));
    }

    return result;
}

bool WorldImplementation::IntersectsWithAgent(double x, double y, double rotation, double length, double width,
        double center)
{

    polygon_t polyNewAgent = World::Localization::GetBoundingBox(x, y, length, width, rotation, center);

    for (std::pair<int, AgentInterface*> agent : GetAgents())
    {
        polygon_t polyAgent = agent.second->GetBoundingBox2D();


        bool intersects = bg::intersects(polyNewAgent, polyAgent);
        if (intersects)
        {
            return true;
        }
    }

    return false;
}

polygon_t WorldImplementation::GetBoundingBoxAroundAgent(AgentInterface* agent, double width, double length)
{
    double x = agent->GetPositionX();
    double y = agent->GetPositionY();
    double rotation =  agent->GetYaw();
    double center = length * 0.5;
    polygon_t poly = World::Localization::GetBoundingBox(x, y, length, width, rotation, center);
    return poly;
}

Position WorldImplementation::RoadCoord2WorldCoord(RoadPosition roadCoord, std::string roadID) const
{
    Position worldCoord;
    worldCoord.xPos = 0;
    worldCoord.yPos = 0;
    worldCoord.yawAngle = 0;

    RoadInterface* road = nullptr;

    if (roadID.empty())
    {
        if (!scenery->GetRoads().empty())
        {
            road = scenery->GetRoads().begin()->second;
        }
    }
    else
    {
        for (auto& item : scenery->GetRoads())
        {
            RoadInterface* itemRoad = item.second;
            if (roadID == itemRoad->GetId())
            {
                road = itemRoad;
                break;
            }
        }
    }

    if (!road)
    {
        const std::string msg = "Could not find road with ID " + roadID;
        throw std::runtime_error(msg);
    }

    std::list<RoadGeometryInterface*> roadGeometries = road->GetGeometries();

    for (RoadGeometryInterface* roadGeometry : roadGeometries)
    {
        double roadGeometryLength = roadGeometry->GetLength();
        double roadGeometryStart = roadGeometry->GetS();
        double roadGeometryEnd = roadGeometryStart + roadGeometryLength;

        if (roadCoord.s > roadGeometryStart && roadCoord.s < roadGeometryEnd)
        {
            double localS = roadCoord.s - roadGeometryStart;
            Common::Vector2d coord = roadGeometry->GetCoord(0, localS, 0, roadCoord.t, 0, 0);
            double dir = roadGeometry->GetDir(0, localS, 0, roadCoord.t, 0);
            worldCoord.xPos = coord.x;
            worldCoord.yPos = coord.y;
            worldCoord.yawAngle = dir + roadCoord.hdg;
            break;
        }
    }

    return worldCoord;
}

void WorldImplementation::InitTrafficObjects()
{
    assert(trafficObjects.size() == 0);

    for (auto& baseTrafficObject : worldData.GetStationaryObjects())
    {
        TrafficObjectInterface* trafficObject = baseTrafficObject.second->GetLink<TrafficObjectInterface>();
        trafficObjects.push_back(trafficObject);
        worldObjects.push_back(trafficObject);
    }
}

LaneQueryResult WorldImplementation::QueryLane(std::string roadId, int laneId, double distance) const
{
    return worldDataQuery.QueryLane(roadId, laneId, distance);
}

std::list<LaneQueryResult> WorldImplementation::QueryLanes(std::string roadId, double startDistance,
        double endDistance) const
{
    return worldDataQuery.QueryLanes(roadId, startDistance, endDistance);
}

int WorldImplementation::GetLaneId(uint64_t streamId, double endDistance) const
{
    Q_UNUSED(endDistance);
    return static_cast<int>(worldData.GetLaneIdMapping().at(streamId));//Hotfix
}

double WorldImplementation::GetDistanceBetweenObjects(const Route& route, const ObjectPosition& objectPos, const ObjectPosition& targetObjectPos) const
{
    const auto roadStream = navigation.GetRoadStream(route);
    return worldDataQuery.GetDistanceBetweenObjects(roadStream, objectPos, targetObjectPos);
}

std::string WorldImplementation::GetNextJunctionIdOnRoute(const Route& route, const ObjectPosition& objectPos) const
{
    return worldDataQuery.GetNextJunctionIdOnRoute(route, objectPos);
}

double WorldImplementation::GetDistanceToJunction(const Route& route, const ObjectPosition& objectPos, const std::string& junctionId) const
{
    return worldDataQuery.GetDistanceToJunction(route, objectPos, junctionId);
}
