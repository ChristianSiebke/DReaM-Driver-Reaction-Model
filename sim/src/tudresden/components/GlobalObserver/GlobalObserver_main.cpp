#include "GlobalObserver_main.h"

namespace GlobalObserver {

std::shared_ptr<Main> Main::instance = nullptr;

void Main::Trigger(int time) {
    if (time == lastConversionTime)
        return;

    // converting the infrastructure
    rnConverter.Populate();

    // generating additional infrastructure data (conflict areas, stopping points, roadmap graph)
    if (!staticInfrastructureCreated) {
        rgCalculator.Populate();
        spCalculator.Populate();
        caCalculator.Populate();
        staticInfrastructureCreated = true;
    }

    // converting all agents
    if (!agentPerceptionsCreated) {
        agentPerceptionsCreated = true;
    }

    // tracking the timestamp to avoid doubling of conversion
    lastConversionTime = time;
}

void Main::SetInitialRoute(int agentId, std::vector<GlobalObserver::Routes::InternWaypoint> route) {
    apConverter.SetInitialRoute(agentId, route);
}

std::shared_ptr<DetailedAgentPerception> Main::GetDetailedAgentPerception(int agentId) {
    return agentPerceptions[agentId];
}

std::vector<std::shared_ptr<GeneralAgentPerception>> Main::GetGeneralAgentPerception(std::vector<int> agentIds) {
    std::vector<std::shared_ptr<GeneralAgentPerception>> toReturn;
    for (const auto &id : agentIds) {
        toReturn.push_back(std::static_pointer_cast<GeneralAgentPerception>(agentPerceptions[id]));
    }
    return toReturn;
}

std::shared_ptr<InfrastructurePerception> Main::GetInfrastructurePerception() {
    return infrastructurePerception;
}

}