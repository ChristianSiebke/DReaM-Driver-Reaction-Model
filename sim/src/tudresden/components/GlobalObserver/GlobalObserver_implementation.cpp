#include "GlobalObserver_implementation.h"

#include <OWL/DataTypes.h>

#include <qglobal.h>

#include "Common/TimeMeasurement.hpp"
TimeMeasurement timeMeasure("GlobalObserver.cpp");

void GlobalObserver_Implementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time) {
    Q_UNUSED(time);

    if (localLinkId == 0) {
        std::shared_ptr<ContainerSignal<std::vector<int>> const> signal =
            std::dynamic_pointer_cast<ContainerSignal<std::vector<int>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype (localLinkId 0 = visible agents)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        visibleAgents = signal->value;
    }
    else if (localLinkId == 1) {
        std::shared_ptr<ContainerSignal<std::vector<OdId>> const> signal =
            std::dynamic_pointer_cast<ContainerSignal<std::vector<OdId>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype (localLinkId 0 = visible traffic signals)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        visibleTrafficSignals = signal->value;
    }
    else {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void GlobalObserver_Implementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time) {
    Q_UNUSED(time);

    if (localLinkId == 0) {
        try {
            data = std::make_shared<ContainerSignal<std::vector<std::shared_ptr<GeneralAgentPerception>>> const>(
                globalObserverMain->GetGeneralAgentPerception(visibleAgents));
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 0 = visible agents)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 1) {
        try {
            data = std::make_shared<structSignal<std::shared_ptr<DetailedAgentPerception>> const>(
                globalObserverMain->GetDetailedAgentPerception(GetAgent()->GetId()));
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 1 = detailed (ego) perception)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 2) {
        try {
            data = std::make_shared<structSignal<std::shared_ptr<InfrastructurePerception>> const>(
                globalObserverMain->GetInfrastructurePerception());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 2 = infrastructure perception)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 3) {
        try {
            data = std::make_shared<ContainerSignal<std::vector<const MentalInfrastructure::TrafficSignal *>> const>(
                globalObserverMain->GetVisibleTrafficSignals(visibleTrafficSignals));
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 3 = traffic signals)";
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
    timeMeasure.StartTimePoint("Trigger GlobalObserver");
    globalObserverMain->Trigger(time);
    timeMeasure.EndTimePoint();
}
