#include "GlobalObserver_implementation.h"

#include <OWL/DataTypes.h>

#include <qglobal.h>

void GlobalObserver_Implementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time) {
    Q_UNUSED(time);
    if (localLinkId == 0) {
        std::shared_ptr<structSignal<void *> const> signal = std::dynamic_pointer_cast<structSignal<void *> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype (localLinkId 0 = GazeState)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        // TODO handle input signals
    }
    else {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void GlobalObserver_Implementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time) {
    Q_UNUSED(time);

    // if (localLinkId == 0) {
    //     try {
    //         data = std::make_shared<ContainerSignal<std::vector<std::shared_ptr<GeneralAgentPerception>>> const>(
    //             sensorPerceptionLogic.GetGeneralAgentPerception());
    //     }
    //     catch (const std::bad_alloc &) {
    //         const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 0 = AgentPerception)";
    //         LOG(CbkLogLevel::Debug, msg);
    //         throw std::runtime_error(msg);
    //     }
    // }
    // else if (localLinkId == 1) {
    //     try {
    //         data = std::make_shared<structSignal<std::shared_ptr<DetailedAgentPerception>>
    //         const>(sensorPerceptionLogic.GetEgoPerception());
    //     }
    //     catch (const std::bad_alloc &) {
    //         const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 1 = EgoPerception)";
    //         LOG(CbkLogLevel::Debug, msg);
    //         throw std::runtime_error(msg);
    //     }
    // }
    // else if (localLinkId == 2) {
    //     try {
    //         data =
    //             std::make_shared<structSignal<std::shared_ptr<InfrastructurePerception>>
    //             const>(sensorPerceptionLogic.GetInfrastructure());
    //     }
    //     catch (const std::bad_alloc &) {
    //         const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 2 = InfrastructurePerception)";
    //         LOG(CbkLogLevel::Debug, msg);
    //         throw std::runtime_error(msg);
    //     }
    // }
    // else if (localLinkId == 3) {
    //     try {
    //         data = std::make_shared<ContainerSignal<std::vector<const MentalInfrastructure::TrafficSignal *>> const>(
    //             sensorPerceptionLogic.GetTrafficSignalPerception());
    //     }
    //     catch (const std::bad_alloc &) {
    //         const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 3 = TrafficSignals)";
    //         LOG(CbkLogLevel::Debug, msg);
    //         throw std::runtime_error(msg);
    //     }
    // }
    // else {
    //     const std::string msg = COMPONENTNAME + " invalid link";
    //     LOG(CbkLogLevel::Debug, msg);
    //     throw std::runtime_error(msg);
    // }

    if (localLinkId == 0) {
        try {
            // TODO actually send data
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 0 = ...)"; // FIXME fill with correct error
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

void GlobalObserver_Implementation::Trigger(int time) {
    globalObserverMain->Trigger(time);
}
