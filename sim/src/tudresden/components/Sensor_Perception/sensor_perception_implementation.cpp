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
    try {
        sensorPerceptionLogic.Trigger(time, currentGazeState);
    }
catch (const char *error) {
    const std::string msg = COMPONENTNAME + " " + error;
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}
catch (const std::string &error) {
    const std::string msg = COMPONENTNAME + " " + error;
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}
catch (const std::out_of_range &error) {
    const std::string msg = COMPONENTNAME + " " + error.what();
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}
catch (const std::runtime_error &error) {
    const std::string msg = COMPONENTNAME + " " + error.what();
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}
catch (const std::logic_error &error) {
    const std::string msg = COMPONENTNAME + " " + error.what();
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}
catch (const std::bad_typeid &error) {
    const std::string msg = COMPONENTNAME + " " + error.what();
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}
catch (const std::bad_cast &error) {
    const std::string msg = COMPONENTNAME + " " + error.what();
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}
catch (const std::bad_alloc &error) {
    const std::string msg = COMPONENTNAME + " " + error.what();
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}
}
