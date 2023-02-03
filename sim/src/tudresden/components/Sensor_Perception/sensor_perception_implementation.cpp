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
            data = std::make_shared<ContainerSignal<std::vector<int>> const>(sensorPerceptionLogic.GetVisibleAgents());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 0 = visible agents)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 1) {
        try {
            data = std::make_shared<ContainerSignal<std::vector<OdId>> const>(sensorPerceptionLogic.GetVisibleTrafficSignals());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 0 = visible traffic signals)";
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
    std::optional<Common::Vector2d> mPos;
    if (currentGazeState.mirrorGaze)
        mPos.emplace(currentGazeState.mirrorPos);
    else
        mPos.reset();

    sensorPerceptionLogic.Trigger(time, currentGazeState, mPos);
}
