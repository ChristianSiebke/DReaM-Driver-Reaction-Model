#include "sensor_perception_implementation.h"

#include <OWL/DataTypes.h>

#include <qglobal.h>

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
    // delegating the trigger to the logic wrapper
    RouteElement target{"2", false}; // Waypoints openPASS
    SetNewRoute(target);
    sensorPerceptionLogic.Trigger(time, currentGazeState.ufovAngle, currentGazeState.viewDistance, currentGazeState.openingAngle);
}

void Sensor_Perception_Implementation::SetNewRoute(RouteElement target) {
    if (target == currentTarget)
        return;
    const auto &roadIds = GetAgent()->GetRoads(MeasurementPoint::Front);
    if (roadIds.empty()) {
        return;
    }
    auto rootRouteElement = CommonHelper::GetRoadWithLowestHeading(GetAgent()->GetObjectPosition().mainLocatePoint, *GetWorld());
    auto [roadGraph, rootVertex] = GetWorld()->GetRoadGraph(rootRouteElement, 20);

    auto vp = vertices(roadGraph);
    auto targetVertex = std::find_if(vp.first, vp.second, [=](auto element) { return get(RouteElement(), roadGraph, element) == target; });
    if (targetVertex == vp.second) {
        const std::string msg = "target vertex does not exist in graph";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
    GetAgent()->GetEgoAgent().SetRoadGraph(std::move(roadGraph), rootVertex, *targetVertex);
    currentTarget = target;
}