#include "sensor_perception_implementation.h"

#include <OWL/DataTypes.h>

#include <qglobal.h>

#include "RouteCalculation.h"

void Sensor_Perception_Implementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time) {
    Q_UNUSED(time);
    if (localLinkId == 0) {
        std::shared_ptr<structSignal<GazeState> const> signal = std::dynamic_pointer_cast<structSignal<GazeState> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype (localLinkId 0 = GazeState)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        currentGazeState = signal->value;
    }
    else {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void Sensor_Perception_Implementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time) {
    Q_UNUSED(time);

    if (localLinkId == 0) {
        try {
            data = std::make_shared<ContainerSignal<std::vector<std::shared_ptr<AgentPerception>>> const>(
                sensorPerceptionLogic.GetAgentPerception());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 0 = AgentPerception)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 1) {
        try {
            data = std::make_shared<structSignal<std::shared_ptr<EgoPerception>> const>(sensorPerceptionLogic.GetEgoPerception());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 1 = EgoPerception)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 2) {
        try {
            data =
                std::make_shared<structSignal<std::shared_ptr<InfrastructurePerception>> const>(sensorPerceptionLogic.GetInfrastructure());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 2 = InfrastructurePerception)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 3) {
        try {
            data = std::make_shared<ContainerSignal<std::vector<const MentalInfrastructure::TrafficSign *>> const>(
                sensorPerceptionLogic.GetTrafficSignPerception());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 3 = TrafficSigns)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void Sensor_Perception_Implementation::Trigger(int time) {
    UpdateGraphPosition();
    sensorPerceptionLogic.Trigger(time, currentGazeState.ufovAngle, currentGazeState.viewDistance, currentGazeState.openingAngle, route);
}

void Sensor_Perception_Implementation::UpdateGraphPosition() {
    auto worldData = static_cast<OWL::WorldData *>(GetWorld()->GetWorldData());
    WorldDataQuery helper(*worldData);

    auto referenceLane = &helper.GetLaneByOdId(GetAgent()->GetEgoAgent().GetMainLocatePosition().roadId,
                                               GetAgent()->GetEgoAgent().GetMainLocatePosition().laneId,
                                               GetAgent()->GetEgoAgent().GetMainLocatePosition().roadPosition.s);

    auto normRad = std::fabs(std::fmod(GetAgent()->GetYaw() - GetAgent()->GetEgoAgent().GetLaneDirection(), (2 * M_PI)));
    auto absDiffDeg = std::min((2 * M_PI) - normRad, normRad);
    bool direction = M_PI_2 >= absDiffDeg ? true : false;
    direction = GetAgent()->GetVelocity() >= 0 ? direction : !direction;
    auto nextLanes = direction ? referenceLane->GetNext() : referenceLane->GetPrevious();

    if ((!GetAgent()->GetEgoAgent().HasValidRoute()) ||
        (GetAgent()->GetEgoAgent().GetRootOfWayToTargetGraph() == 0 && nextLanes.size() > 0)) {
        GetNewRoute();
    }
}

void Sensor_Perception_Implementation::GetNewRoute() {
    constexpr int maxDepth = 10;
    const auto &roadIds = GetAgent()->GetRoads(MeasurementPoint::Front);
    if (roadIds.empty()) {
        return;
    }
    auto [branchedGraph, root] = GetWorld()->GetRoadGraph(
        CommonHelper::GetRoadWithLowestHeading(GetAgent()->GetObjectPosition().referencePoint, *GetWorld()), maxDepth);
    std::map<RoadGraph::edge_descriptor, double> weights = GetWorld()->GetEdgeWeights(branchedGraph);
    auto targetVertex = RouteCalculation::SampleRoute(branchedGraph, root, weights, *GetStochastics());

    auto straightRouteGraph = RoadGraph{};
    RoadGraphVertex wayPoint = targetVertex;
    auto vertex1 = add_vertex(get(RouteElement(), branchedGraph, wayPoint), straightRouteGraph);
    auto newTarget = vertex1;
    RoadGraphVertex vertex2;
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
    auto newRoot = vertex2;

    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    auto roads = worldData->GetRoads();
    WorldDataQuery helper(*worldData);

    auto [begin, end] = vertices(straightRouteGraph);
    auto rbegin = std::make_reverse_iterator(end);
    auto rend = std::make_reverse_iterator(begin);

    std::vector<InternWaypoint> newRoute;
    auto currentLaneId = GetAgent()->GetEgoAgent().GetReferencePointPosition()->laneId;

    std::transform(rbegin, rend, std::back_inserter(newRoute),
                   [&straightRouteGraph = straightRouteGraph, roads, &currentLaneId](auto element) {
                       InternWaypoint newRoute;
                       newRoute.roadId = get(RouteElement(), straightRouteGraph, element).roadId;
                       newRoute.s = 0;
                       auto road = roads.at(get(RouteElement(), straightRouteGraph, element).roadId);
                       auto section = road->GetSections().front();
                       auto laneId = 0;
                       if (get(RouteElement(), straightRouteGraph, element).inOdDirection) {
                           laneId = currentLaneId < 0 ? -1 : 1;
                           auto iter = std::find_if(section->GetLanes().begin(), section->GetLanes().end(),
                                                    [laneId](auto element) { return element->GetOdId() == laneId; });
                           newRoute.lane = (*iter)->GetId();
                       }
                       else {
                           laneId = currentLaneId < 0 ? 1 : -1;
                           auto iter = std::find_if(section->GetLanes().begin(), section->GetLanes().end(),
                                                    [laneId](auto element) { return element->GetOdId() == laneId; });
                           newRoute.lane = (*iter)->GetId();
                       }
                       currentLaneId = laneId;
                       return newRoute;
                   });
    route = newRoute;
    GetAgent()->GetEgoAgent().SetRoadGraph(std::move(straightRouteGraph), newRoot, newTarget);
}
