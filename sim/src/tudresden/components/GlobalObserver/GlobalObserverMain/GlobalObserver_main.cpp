#include "GlobalObserver_main.h"

namespace GlobalObserver {

std::shared_ptr<Main> Main::instance = nullptr;
int Main::runId = 0;
ProfilesInterface *Main::profile = nullptr;
std::string Main::scenarioConfigPath = "";

void Main::Trigger(int time) {
    if (time == lastConversionTime)
        return;
    // tracking the timestamp to avoid doubling of conversion
    lastConversionTime = time;

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
    apConverter.Populate();
}

void Main::TriggerRoadNetworkConversion() {
    rnConverter.Populate();
}

void Main::SetInitialRoute(AgentInterface *agent) {
    // TODO: waypoints/Route must be passed via the openPASS framework.
    // So far the openPASS framework (AgentInterface/egoAgent) has no function to get the waypoints/route --> redundant import
    Routes::RouteImporter routeImporter(scenarioConfigPath);
    auto routeImport = routeImporter.GetDReaMRoute(agent->GetScenarioName());
    auto route = routeConverter.Convert(routeImport);
    auto a = profile->GetProfileGroups();
    std::for_each(a.begin(), a.end(), [](auto element) { std::cout << "Profilegroup: " << element.first << std::endl; });
    apConverter.SetInitialRoute(agent, route);
}

std::shared_ptr<DetailedAgentPerception> Main::GetDetailedAgentPerception(int agentId) {
    return agentPerceptions.at(agentId);
}

std::vector<std::shared_ptr<GeneralAgentPerception>> Main::GetGeneralAgentPerception(std::vector<int> agentIds) {
    std::vector<std::shared_ptr<GeneralAgentPerception>> toReturn;
    for (const auto &id : agentIds) {
        if (agentPerceptions.find(id) != agentPerceptions.end()) {
            toReturn.push_back(std::static_pointer_cast<GeneralAgentPerception>(agentPerceptions.at(id)));
        }
    }
    return toReturn;
}

std::shared_ptr<InfrastructurePerception> Main::GetInfrastructurePerception() {
    return infrastructurePerception;
}

std::vector<const MentalInfrastructure::TrafficSignal *> Main::GetVisibleTrafficSignals(std::vector<OdId> ids) {
    std::vector<const MentalInfrastructure::TrafficSignal *> toReturn;
    for (const auto &id : ids) {
        if (infrastructurePerception->lookupTableRoadNetwork.trafficSigns.find(id) !=
            infrastructurePerception->lookupTableRoadNetwork.trafficSigns.end()) {
            toReturn.push_back(infrastructurePerception->lookupTableRoadNetwork.trafficSigns.at(id));
            continue;
        }
        if (infrastructurePerception->lookupTableRoadNetwork.trafficLights.find(id) !=
            infrastructurePerception->lookupTableRoadNetwork.trafficLights.end()) {
            toReturn.push_back(infrastructurePerception->lookupTableRoadNetwork.trafficLights.at(id));
        }
    }
    return toReturn;
}
} // namespace GlobalObserver