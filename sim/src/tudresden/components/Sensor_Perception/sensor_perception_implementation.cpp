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
            // FIXME GetDriverRoutePlanning() deprecated, need route
            // data = std::make_shared<structSignal<RouteElement> const>(GetAgent()->GetDriverRoutePlanning());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 3 = DriverRoutePlanning)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 4) {
        try {
            data = std::make_shared<ContainerSignal<std::vector<const MentalInfrastructure::TrafficSign *>> const>(
                sensorPerceptionLogic.GetTrafficSignPerception());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 4 = TrafficSigns)";
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
    sensorPerceptionLogic.Trigger(time, currentGazeState.ufovAngle, currentGazeState.viewDistance, currentGazeState.openingAngle);
}
