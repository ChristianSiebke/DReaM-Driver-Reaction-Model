#include "AgentPerceptionConverter.h"

#include <cassert>

#include "WorldDataQuery.h"
#include "egoAgent.h"

namespace GlobalObserver::Converters {
void AgentPerceptionConverter::Populate() {
    agentPerceptions.clear();
    for (const auto &[id, agent] : world->GetAgents()) {
        auto converted = ConvertAgent(agent);
        agentPerceptions.insert({converted->id, converted});
    }
}

void AgentPerceptionConverter::SetInitialRoute(AgentInterface *agent, std::vector<GlobalObserver::Routes::InternWaypoint> route) {
    if (route.empty()) {
        auto newRouteOptional = RouteUpdate(agent);
        if (newRouteOptional.has_value()) {
            auto dreamRoute = ConvertRoute(*newRouteOptional);
            routeMapping.insert_or_assign(agent->GetId(), dreamRoute);
        }
    }
    else {
        routeMapping.insert_or_assign(agent->GetId(), ConvertRoute(route));
        auto roadGraph = RoadGraph{};
        auto vertex1 = add_vertex(
            RouteElement{route.back().roadId,
                         this->infrastructurePerception->lookupTableRoadNetwork.lanes.at(route.back().lane)->IsInRoadDirection()},
            roadGraph);
        auto target = vertex1;
        RoadGraphVertex vertex2 = 0;
        for (auto iter = route.rbegin(); iter != route.rend(); iter++) {
            if (route.back().roadId == iter->roadId) {
                continue;
            }
            vertex2 =
                add_vertex(RouteElement{iter->roadId,
                                        this->infrastructurePerception->lookupTableRoadNetwork.lanes.at(iter->lane)->IsInRoadDirection()},
                           roadGraph);
            add_edge(vertex2, vertex1, roadGraph);
            vertex1 = vertex2;
        }
        const_cast<AgentInterface *>(agent)->GetEgoAgent().SetRoadGraph(std::move(roadGraph), vertex2, target);
    }
}

DReaMRoute::Waypoints AgentPerceptionConverter::ConvertRoute(std::vector<GlobalObserver::Routes::InternWaypoint> route) {
    DReaMRoute::Waypoints dreamRoute;
    std::transform(route.begin(), route.end(), std::back_inserter(dreamRoute), [this](auto element) {
        DReaMRoute::Waypoint result;
        result.roadId = element.roadId;
        result.lane = this->infrastructurePerception->lookupTableRoadNetwork.lanes.at(element.lane);
        result.s = element.s;
        return result;
    });
    return dreamRoute;
}

bool AgentPerceptionConverter::CheckForRouteUpdate(const AgentInterface *agent) const {
    const auto &egoAgent = const_cast<AgentInterface *>(agent)->GetEgoAgent();
    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    WorldDataQuery helper(*worldData);

    auto referenceLane = &helper.GetLaneByOdId(egoAgent.GetMainLocatePosition().roadId, egoAgent.GetMainLocatePosition().laneId,
                                               egoAgent.GetMainLocatePosition().roadPosition.s);

    auto normRad = std::fabs(std::fmod(agent->GetYaw() - egoAgent.GetLaneDirection(), (2 * M_PI)));
    auto absDiffDeg = std::min((2 * M_PI) - normRad, normRad);
    bool direction = M_PI_2 >= absDiffDeg ? true : false;
    direction = agent->GetVelocity() >= 0 ? direction : !direction;
    auto nextLanes = direction ? referenceLane->GetNext() : referenceLane->GetPrevious();

    return (!egoAgent.HasValidRoute()) || !egoAgent.GetReferencePointPosition() ||
           (routeMapping.find(agent->GetId()) == routeMapping.end()) || (egoAgent.GetRootOfWayToTargetGraph() == 0 && nextLanes.size() > 0);
}

std::optional<std::vector<GlobalObserver::Routes::InternWaypoint>>
AgentPerceptionConverter::RouteUpdate(const AgentInterface *agent) const {
    const auto &egoAgent = const_cast<AgentInterface *>(agent)->GetEgoAgent();
    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    WorldDataQuery helper(*worldData);
    constexpr int maxDepth = 10;
    const auto &roadIds = agent->GetRoads(MeasurementPoint::Front);
    if (roadIds.empty()) {
        return std::nullopt;
    }
    auto [branchedGraph, root] =
        world->GetRoadGraph(CommonHelper::GetRoadWithLowestHeading(agent->GetObjectPosition().referencePoint, *world), maxDepth);
    std::map<RoadGraph::edge_descriptor, double> weights = world->GetEdgeWeights(branchedGraph);

    RoadGraphVertex vertex2;
    RoadGraphVertex newTarget;
    bool sampleRoute = true;
    auto straightRouteGraph = RoadGraph{};
    auto referenceLane = &helper.GetLaneByOdId(egoAgent.GetMainLocatePosition().roadId, egoAgent.GetMainLocatePosition().laneId,
                                               egoAgent.GetMainLocatePosition().roadPosition.s);
    while (sampleRoute) {
        straightRouteGraph = RoadGraph{};
        RoadGraphVertex targetVertex = RouteCalculation::SampleRoute(branchedGraph, root, weights, *stochastics);
        RoadGraphVertex wayPoint = targetVertex;
        auto vertex1 = add_vertex(get(RouteElement(), branchedGraph, wayPoint), straightRouteGraph);

        newTarget = vertex1;
        while (wayPoint != root) {
            for (auto [edge, edgesEnd] = edges(branchedGraph); edge != edgesEnd; ++edge) {
                if (target(*edge, branchedGraph) == wayPoint) {
                    wayPoint = source(*edge, branchedGraph);
                    vertex2 = add_vertex(get(RouteElement(), branchedGraph, wayPoint), straightRouteGraph);
                    add_edge(vertex2, vertex1, straightRouteGraph);
                    vertex1 = vertex2;
                    break;
                }
            }
      
        }
        auto [begin, end] = vertices(straightRouteGraph);
        auto referenceLaneDReaM = infrastructurePerception->lookupTableRoadNetwork.lanes.at(referenceLane->GetId());
        auto nextLane = referenceLaneDReaM->NextLane(egoAgent.GetAgent()->GetIndicatorState(), true);
          if (nextLane->GetRoad()->GetOpenDriveId() == get(RouteElement(), straightRouteGraph, *std::prev(end, 1)).roadId ||
            nextLane->GetRoad()->GetOpenDriveId() == get(RouteElement(), straightRouteGraph, *std::prev(end, 2)).roadId)
            sampleRoute = false;
    }
    auto newRoot = vertex2;

    auto [begin, end] = vertices(straightRouteGraph);
    auto rbegin = std::make_reverse_iterator(end);
    auto rend = std::make_reverse_iterator(begin);

    std::vector<GlobalObserver::Routes::InternWaypoint> newRoute{};
    auto owlRefLaneId = referenceLane->GetId();
    auto nextElement = rbegin;
    std::transform(rbegin, rend, std::back_inserter(newRoute),
                   [&nextElement, rend, &straightRouteGraph = straightRouteGraph, &owlRefLaneId, this](auto element) {
                       GlobalObserver::Routes::InternWaypoint newRoute;
                       newRoute.s = 0;
                       newRoute.roadId = get(RouteElement(), straightRouteGraph, element).roadId;
                       auto referenceLaneDReaM = infrastructurePerception->lookupTableRoadNetwork.lanes.at(owlRefLaneId);
                       auto lanes = referenceLaneDReaM->NextLanes(true);
                       newRoute.lane = owlRefLaneId;
                       nextElement++;
                       if (nextElement != rend) {
                           auto nextRoadId = get(RouteElement(), straightRouteGraph, *nextElement).roadId;

                           if (auto a = std::find_if(lanes->leftLanes.begin(), lanes->leftLanes.end(),
                                                     [nextRoadId](const MentalInfrastructure::Lane *element) {
                                                         return element->GetRoad()->GetOpenDriveId() == nextRoadId;
                                                     });
                               a != lanes->leftLanes.end()) {
                               owlRefLaneId = (*a)->GetOwlId();
                           }
                           if (auto a = std::find_if(lanes->rightLanes.begin(), lanes->rightLanes.end(),
                                                     [nextRoadId](const MentalInfrastructure::Lane *element) {
                                                         return element->GetRoad()->GetOpenDriveId() == nextRoadId;
                                                     });
                               a != lanes->rightLanes.end()) {
                               owlRefLaneId = (*a)->GetOwlId();
                           }
                           if (auto a = std::find_if(lanes->straightLanes.begin(), lanes->straightLanes.end(),
                                                     [nextRoadId](const MentalInfrastructure::Lane *element) {
                                                         return element->GetRoad()->GetOpenDriveId() == nextRoadId;
                                                     });
                               a != lanes->straightLanes.end()) {
                               owlRefLaneId = (*a)->GetOwlId();
                           }
                       }
                       return newRoute;
                   });

    const_cast<AgentInterface *>(agent)->GetEgoAgent().SetRoadGraph(std::move(straightRouteGraph), newRoot, newTarget);
    return newRoute;
}

Common::Vector2d AgentPerceptionConverter::GetDriverPosition(const AgentInterface *agent) const {
    // driver is located in the center of agent
    double xDistanceRefPointToDriver = agent->GetDistanceReferencePointToLeadingEdge() - (agent->GetLength() / 2);
    Common::Vector2d distanceRefPointToDriver(xDistanceRefPointToDriver, 0);
    distanceRefPointToDriver.Rotate(agent->GetYaw());

    Common::Vector2d driverPosition{agent->GetPositionX(), agent->GetPositionY()};
    driverPosition.Add(distanceRefPointToDriver);
    return driverPosition;
}

std::shared_ptr<DetailedAgentPerception> AgentPerceptionConverter::ConvertAgent(const AgentInterface *agent) {
    DReaMRoute::Waypoints dreamRoute;
    if (CheckForRouteUpdate(agent)) {
        auto newRouteOptional = RouteUpdate(agent);
        if (newRouteOptional.has_value()) {
            dreamRoute = ConvertRoute(*newRouteOptional);
            routeMapping.insert_or_assign(agent->GetId(), dreamRoute);
        }
        else {
            dreamRoute = routeMapping.at(agent->GetId());
        }
    }
    else {
        dreamRoute = routeMapping.at(agent->GetId());
    }
    const auto &actualEgoAgent = const_cast<AgentInterface *>(agent)->GetEgoAgent();

    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());

    WorldDataQuery helper(*worldData);
    auto referenceLane =
        &helper.GetLaneByOdId(actualEgoAgent.GetReferencePointPosition()->roadId, actualEgoAgent.GetReferencePointPosition()->laneId,
                              actualEgoAgent.GetReferencePointPosition()->roadPosition.s);
    if (!referenceLane || !referenceLane->Exists()) {
        auto msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " invalid lane ";
        throw std::runtime_error(msg);
    }

    auto referenceLaneDReaM = infrastructurePerception->lookupTableRoadNetwork.lanes.at(referenceLane->GetId());

    auto mainLocatorLane =
        &helper.GetLaneByOdId(actualEgoAgent.GetMainLocatePosition().roadId, actualEgoAgent.GetMainLocatePosition().laneId,
                              actualEgoAgent.GetMainLocatePosition().roadPosition.s);

    if (!mainLocatorLane || !mainLocatorLane->Exists()) {
        auto msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " invalid lane ";
        throw std::runtime_error(msg);
    }
    auto mainLocatorLaneDReaM = infrastructurePerception->lookupTableRoadNetwork.lanes.at(mainLocatorLane->GetId());

    DetailedAgentPerception perceptionData;

    // object information
    perceptionData.id = agent->GetId();
    perceptionData.yaw = agent->GetYaw();
    perceptionData.width = agent->GetWidth();
    perceptionData.length = agent->GetLength();
    perceptionData.refPosition = Common::Vector2d(agent->GetPositionX(), agent->GetPositionY());
    perceptionData.touchedRoads = actualEgoAgent.GetAgent()->GetObjectPosition().touchedRoads;
    if (perceptionData.touchedRoads.empty()) {
        auto msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " error ";
        throw std::runtime_error(msg);
    }
    // general agent information
    perceptionData.vehicleType = (DReaMDefinitions::AgentVehicleType)agent->GetVehicleModelParameters().vehicleType;
    perceptionData.distanceReferencePointToLeadingEdge = agent->GetDistanceReferencePointToLeadingEdge();
    perceptionData.acceleration = agent->GetAcceleration();
    perceptionData.velocity = agent->GetVelocity(VelocityScope::Absolute);
    perceptionData.lanePosition = {referenceLaneDReaM,
                                   referenceLaneDReaM->IsInRoadDirection()
                                       ? actualEgoAgent.GetReferencePointPosition()->roadPosition.s
                                       : referenceLaneDReaM->GetLength() - actualEgoAgent.GetReferencePointPosition()->roadPosition.s};
    perceptionData.movingInLaneDirection =
        GeneralAgentPerception::IsMovingInLaneDirection(perceptionData.lanePosition.lane, perceptionData.yaw,
                                                        perceptionData.lanePosition.sCoordinate, perceptionData.velocity);
    perceptionData.brakeLight = agent->GetBrakeLight();
    perceptionData.indicatorState = agent->GetIndicatorState();
    perceptionData.nextLane =
        perceptionData.lanePosition.lane->NextLane(perceptionData.indicatorState, perceptionData.movingInLaneDirection);
    perceptionData.junctionDistance = GeneralAgentPerception::CalculateJunctionDistance(
        perceptionData, perceptionData.lanePosition.lane->GetRoad(), perceptionData.lanePosition.lane);

    // detailed agent information
    perceptionData.globalDriverPosition = GetDriverPosition(agent);
    perceptionData.steeringWheelAngle = agent->GetSteeringWheelAngle();
    perceptionData.laneWidth = actualEgoAgent.GetLaneWidth();
    perceptionData.mainLocatorInformation = {
        mainLocatorLaneDReaM, // lane ptr
        perceptionData.movingInLaneDirection ? actualEgoAgent.GetPositionLateral()
                                             : -actualEgoAgent.GetPositionLateral(),                              // lateralDisplacement
        actualEgoAgent.GetLaneCurvature(),                                                                        // curvature
        perceptionData.movingInLaneDirection ? actualEgoAgent.GetRelativeYaw() : -actualEgoAgent.GetRelativeYaw() // heading
    };

    perceptionData.route = dreamRoute;

    return std::make_shared<DetailedAgentPerception>(perceptionData);
}
} // namespace GlobalObserver::Converters
