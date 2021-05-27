/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
*               2020 HLRS, University of Stuttgart.
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "WorldData.h"
#include "WorldImplementation.h"
#include "common/RoutePlanning/RouteCalculation.h"
#include "EntityRepository.h"
#include "WorldEntities.h"

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

WorldImplementation::WorldImplementation(const CallbackInterface* callbacks, StochasticsInterface* stochastics, DataBufferWriteInterface* dataBuffer):
    agentNetwork(this, callbacks),
    callbacks(callbacks),
    dataBuffer(dataBuffer),
    repository(dataBuffer),
    worldData(callbacks)
{}

WorldImplementation::~WorldImplementation()
{
}

void WorldImplementation::RegisterAgent(AgentInterface* agent)
{
    agentNetwork.AddAgent(agent);
    worldObjects.push_back(agent);
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

const std::list<const AgentInterface*> WorldImplementation::GetRemovedAgentsInPreviousTimestep()
{
    return agentNetwork.GetRemovedAgentsInPreviousTimestep();
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
    repository.Reset();
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
}

void WorldImplementation::RemoveAgent(const AgentInterface* agent)
{
    agentNetwork.RemoveAgent(agent);

    auto it = std::find(worldObjects.begin(), worldObjects.end(), agent);
    if (it != worldObjects.end())
    {
        worldObjects.erase(it);
    }
}

void WorldImplementation::PublishGlobalData(int timestamp)
{
    agentNetwork.PublishGlobalData(
        [&](openpass::type::EntityId id, openpass::type::FlatParameterKey key, openpass::type::FlatParameterValue value)
        {
            dataBuffer->PutCyclic(id, key, value);
        });
}

void WorldImplementation::SyncGlobalData(int timestamp)
{
    auto lanes = worldData.GetLanes();
    for (auto lane : lanes)
    {
        lane.second->ClearMovingObjects();
    }
    agentNetwork.SyncGlobalData();
    trafficLightNetwork.UpdateStates(timestamp);
}

bool WorldImplementation::CreateScenery(SceneryInterface* scenery, const SceneryDynamicsInterface& sceneryDynamics)
{
    this->scenery = scenery;
    sceneryConverter = std::make_unique<SceneryConverter>(scenery,
                                                          repository,
                                                          worldData,
                                                          localizer,
                                                          callbacks);

    THROWIFFALSE(sceneryConverter->ConvertRoads(), "Unable to finish conversion process.")
    localizer.Init();
    sceneryConverter->ConvertObjects();
    InitTrafficObjects();

    RoadNetworkBuilder networkBuilder(*scenery);
    auto [roadGraph, vertexMapping] = networkBuilder.Build();
    worldData.SetRoadGraph(std::move(roadGraph), std::move(vertexMapping));
    worldData.SetEnvironment(sceneryDynamics.GetEnvironment());

    trafficLightNetwork = TrafficLightNetworkBuilder::Build(sceneryDynamics.GetTrafficSignalControllers(), worldData);

    return true;
}

std::unique_ptr<AgentInterface> WorldImplementation::CreateAgentAdapter(openpass::type::FlatParameter parameter)
{
    const auto id = repository.Register(openpass::entity::EntityType::MovingObject, openpass::utils::GetEntityInfo(parameter));
    return std::make_unique<AgentAdapter>(id, this, callbacks, &worldData, localizer);
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

RouteQueryResult<std::vector<CommonTrafficSign::Entity>> WorldImplementation::GetTrafficSignsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double searchRange) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetTrafficSignsInRange(*laneMultiStream, startDistanceOnStream, searchRange);
}

RouteQueryResult<std::vector<CommonTrafficSign::Entity> > WorldImplementation::GetRoadMarkingsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double searchRange) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetRoadMarkingsInRange(*laneMultiStream, startDistanceOnStream, searchRange);
}

RouteQueryResult<std::vector<CommonTrafficLight::Entity>> WorldImplementation::GetTrafficLightsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double searchRange) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetTrafficLightsInRange(*laneMultiStream, startDistanceOnStream, searchRange);
}

RouteQueryResult<std::vector<LaneMarking::Entity> > WorldImplementation::GetLaneMarkings(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double range, Side side) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetLaneMarkings(*laneMultiStream, startDistanceOnStream, range, side);
}

RouteQueryResult<RelativeWorldView::Junctions> WorldImplementation::GetRelativeJunctions(const RoadGraph& roadGraph, RoadGraphVertex startNode, double startDistance, double range) const
{
    const auto roadMultiStream = worldDataQuery.CreateRoadMultiStream(roadGraph, startNode);
    double startDistanceOnStream = roadMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetRelativeJunctions(*roadMultiStream, startDistanceOnStream, range);
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

std::pair<RoadGraph, RoadGraphVertex> WorldImplementation::GetRoadGraph(const RouteElement& start, int maxDepth) const
{
    auto startVertex = worldData.GetRoadGraphVertexMapping().at(start);
    return RouteCalculation::FilterRoadGraphByStartPosition(worldData.GetRoadGraph(), startVertex, maxDepth);
}

std::map<RoadGraphEdge, double> WorldImplementation::GetEdgeWeights(const RoadGraph& roadGraph) const
{
    std::map<RoadGraphEdge, double> weights;
    for (auto [edge, edgesEnd] = edges(roadGraph); edge != edgesEnd; ++edge)
    {
        weights[*edge] = 1;
    }
    return weights;
}

std::unique_ptr<RoadStreamInterface> WorldImplementation::GetRoadStream(const std::vector<RouteElement> &route) const
{
    return worldDataQuery.CreateRoadStream(route);
}

AgentInterface* WorldImplementation::GetEgoAgent()
{
    const std::map<int, AgentInterface*> agents = agentNetwork.GetAgents();

    for (auto iterator = agents.begin(); iterator != agents.end(); ++iterator)
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

RouteQueryResult<Obstruction> WorldImplementation::GetObstruction(const RoadGraph& roadGraph, RoadGraphVertex startNode, const GlobalRoadPosition& ownPosition,
                                                                  const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners, const Common::Vector2d& mainLaneLocator) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, ownPosition.laneId, ownPosition.roadPosition.s);
    return worldDataQuery.GetObstruction(*laneMultiStream, ownPosition.roadPosition.t, otherPosition, objectCorners, mainLaneLocator);
}

RouteQueryResult<RelativeWorldView::Lanes> WorldImplementation::GetRelativeLanes(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double distance, double range) const
{
    const auto roadMultiStream = worldDataQuery.CreateRoadMultiStream(roadGraph, startNode);
    double startDistanceOnStream = roadMultiStream->GetPositionByVertexAndS(startNode, distance);

    return worldDataQuery.GetRelativeLanes(*roadMultiStream, startDistanceOnStream, laneId, range);
}

RouteQueryResult<std::vector<const AgentInterface*> > WorldImplementation::GetAgentsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double backwardRange, double forwardRange) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    const auto queryResult = worldDataQuery.GetObjectsOfTypeInRange<OWL::Interfaces::MovingObject>(*laneMultiStream, startDistanceOnStream - backwardRange, startDistanceOnStream + forwardRange);
    RouteQueryResult<std::vector<const AgentInterface*>> result;
    for (const auto& [node, objects]: queryResult)
    {
        result[node] = get_transformed<AgentInterface>(objects);
    }
    return result;
}

RouteQueryResult<std::vector<const WorldObjectInterface*>> WorldImplementation::GetObjectsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double backwardRange, double forwardRange) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    const auto queryResult = worldDataQuery.GetObjectsOfTypeInRange<OWL::Interfaces::WorldObject>(*laneMultiStream, startDistanceOnStream - backwardRange, startDistanceOnStream + forwardRange);
    RouteQueryResult<std::vector<const WorldObjectInterface*>> result;
    for (const auto& [node, objects]: queryResult)
    {
        result[node] = get_transformed<WorldObjectInterface>(objects);
    }
    return result;
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

std::map<std::string, GlobalRoadPosition> WorldImplementation::WorldCoord2LaneCoord(double x, double y, double heading) const
{
    return localizer.Locate({x,y}, heading);
}

bool WorldImplementation::IsSValidOnLane(std::string roadId, int laneId,
        double distance) //when necessary optional parameter with reference to get point
{
    return worldDataQuery.IsSValidOnLane(roadId, laneId, distance);
}

bool WorldImplementation::IsDirectionalRoadExisting(const std::string &roadId, bool inOdDirection) const
{
    return worldData.GetRoadGraphVertexMapping().find(RouteElement {roadId, inOdDirection}) != worldData.GetRoadGraphVertexMapping().end();
}

bool WorldImplementation::IsLaneTypeValid(const std::string &roadId, const int laneId, const double distanceOnLane, const LaneTypes& validLaneTypes)
{
    const auto& laneType = worldDataQuery.GetLaneByOdId(roadId, laneId, distanceOnLane).GetLaneType();

    if (std::find(validLaneTypes.begin(), validLaneTypes.end(), laneType) == validLaneTypes.end())
    {
        return false;
    }

    return true;
}

double WorldImplementation::GetLaneCurvature(std::string roadId, int laneId, double position) const
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, position);
    if (!lane.Exists())
    {
        return 0.0;
    }
    return lane.GetCurvature(position);
}

RouteQueryResult<std::optional<double> > WorldImplementation::GetLaneCurvature(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, position);
    double positionOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, position);
    return worldDataQuery.GetLaneCurvature(*laneMultiStream, positionOnStream + distance);
}

double WorldImplementation::GetLaneWidth(std::string roadId, int laneId, double position) const
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, position);
    if (!lane.Exists())
    {
        return 0.0;
    }
    return lane.GetWidth(position);
}

RouteQueryResult<std::optional<double>> WorldImplementation::GetLaneWidth(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, position);
    double positionOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, position);
    return worldDataQuery.GetLaneWidth(*laneMultiStream, positionOnStream + distance);
}

double WorldImplementation::GetLaneDirection(std::string roadId, int laneId, double position) const
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, position);
    if (!lane.Exists())
    {
        return 0.0;
    }
    return lane.GetDirection(position);
}

RouteQueryResult<std::optional<double> > WorldImplementation::GetLaneDirection(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, position);
    double positionOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, position);
    return worldDataQuery.GetLaneDirection(*laneMultiStream, positionOnStream + distance);
}

RouteQueryResult<double> WorldImplementation::GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance, double maximumSearchLength) const
{
    return GetDistanceToEndOfLane(roadGraph, startNode, laneId, initialSearchDistance, maximumSearchLength,
                                  {LaneType::Driving, LaneType::Exit, LaneType::OnRamp, LaneType::OffRamp, LaneType::Stop});
}

RouteQueryResult<double> WorldImplementation::GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance, double maximumSearchLength, const LaneTypes& laneTypes) const
{
    auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, initialSearchDistance);
    auto initialPositionOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, initialSearchDistance);
    return worldDataQuery.GetDistanceToEndOfLane(*laneMultiStream, initialPositionOnStream, maximumSearchLength, laneTypes);
}

LaneSections WorldImplementation::GetLaneSections(const std::string& roadId) const
{
    LaneSections result;

    const auto& road = worldDataQuery.GetRoadByOdId(roadId);
    for (const auto &section : road->GetSections())
    {
        LaneSection laneSection;
        laneSection.startS = section->GetSOffset();
        laneSection.endS = laneSection.startS + section->GetLength();



        for (const auto& lane : section->GetLanes())
        {
            laneSection.laneIds.push_back(lane->GetOdId());
        }

        result.push_back(laneSection);
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

Position WorldImplementation::RoadCoord2WorldCoord(RoadPosition roadCoord, std::string roadID) const
{
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

    return SceneryConverter::RoadCoord2WorldCoord(road, roadCoord.s, roadCoord.t, roadCoord.hdg);
}

double WorldImplementation::GetRoadLength(const std::string& roadId) const
{
    return worldDataQuery.GetRoadByOdId(roadId)->GetLength();
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

RouteQueryResult<LongitudinalDistance> WorldImplementation::GetDistanceBetweenObjects(const RoadGraph& roadGraph, RoadGraphVertex startNode,
                                                      const ObjectPosition& objectPos, const std::optional<double> objectReferenceS, const ObjectPosition& targetObjectPos) const
{
    const auto roadStream = worldDataQuery.CreateRoadMultiStream(roadGraph, startNode);
    return worldDataQuery.GetDistanceBetweenObjects(*roadStream, objectPos, objectReferenceS, targetObjectPos);
}
