/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
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
    if (converter.ConvertRoads())
    {
        localizer.Init();
        converter.ConvertObjects();
        InitTrafficObjects();
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

std::vector<CommonTrafficSign::Entity> WorldImplementation::GetRoadMarkingsInRange(const Route& route, std::string roadId, int laneId, double startDistance, double searchRange) const
{
    std::vector<CommonTrafficSign::Entity> foundMarkings {};
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

    const auto& roadMarkings = worldDataQuery.GetRoadMarkingsInRange(laneStream,
                                                       startDistance,
                                                       searchRange);
    for (auto& roadMarking : roadMarkings)
    {
        foundMarkings.push_back(roadMarking.second->GetSpecification(roadMarking.first));
    }

    return foundMarkings;
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

bool WorldImplementation::IsSValidOnLane(std::string roadId, int laneId,
        double distance) //when necessary optional parameter with reference to get point
{
    return worldDataQuery.IsSValidOnLane(roadId, laneId, distance);
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
    return GetDistanceToEndOfLane(route, roadId, laneId, initialSearchDistance, maximumSearchLength,
    {LaneType::Driving, LaneType::Exit, LaneType::OnRamp, LaneType::OffRamp, LaneType::Stop});
}

double WorldImplementation::GetDistanceToEndOfLane(Route route, std::string roadId, int laneId, double initialSearchDistance, double maximumSearchLength, const LaneTypes& laneTypes)
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, initialSearchDistance);
    if (!lane.Exists())
    {
        return 0.0;
    }
    auto laneStream = navigation.GetLaneStream(route, roadId, laneId, initialSearchDistance);
    double positionOnLaneStream = laneStream.GetPositionByElementAndS(lane, initialSearchDistance);
    return worldDataQuery.GetDistanceToEndOfLane(laneStream, positionOnLaneStream,
                                                 maximumSearchLength, laneTypes);
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
            Common::Vector2d coord = roadGeometry->GetCoord(localS, roadCoord.t);
            double dir = roadGeometry->GetDir(localS);
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
