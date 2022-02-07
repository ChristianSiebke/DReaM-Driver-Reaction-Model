#include "sensor_perception_implementation.h"

#include <OWL/DataTypes.h>

#include <qglobal.h>

void Sensor_Perception_Implementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time) {
    Q_UNUSED(time);
    Q_UNUSED(localLinkId);
    Q_UNUSED(data)
}

void Sensor_Perception_Implementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time) {
    Q_UNUSED(time);

    if (localLinkId == 0) {
        try {
            data = std::make_shared<ContainerSignal<std::vector<std::shared_ptr<AgentPerception>>> const>(
                sensorPerceptionLogic.GetAgentPerception());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 0)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 1) {
        try {
            data = std::make_shared<structSignal<std::shared_ptr<EgoPerception>> const>(sensorPerceptionLogic.GetEgoPerception());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 1)";
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
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 2)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 3) {
        try {
            // FIXME GetDriverRoutePlanning() not declared
            // data = std::make_shared<structSignal<DriverRoutePlanning> const>(GetAgent()->GetDriverRoutePlanning());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 3)";
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
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 4)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    // TODO Add link for gazestate
    else {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void Sensor_Perception_Implementation::Trigger(int time) {
    // delegating the trigger to the logic wrapper
    sensorPerceptionLogic.Trigger(time);
}
