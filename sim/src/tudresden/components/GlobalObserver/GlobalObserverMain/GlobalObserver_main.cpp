#include "GlobalObserver_main.h"

#include "core/opSimulation/framework/sampler.h"
#include "egoAgent.h"
namespace GlobalObserver {

std::shared_ptr<Main> Main::instance = nullptr;
int Main::runId = 0;
ProfilesInterface *Main::profile = nullptr;
std::string Main::scenarioConfigPath = "";
std::unordered_map<DReaMDefinitions::AgentVehicleType,
                   std::unordered_map<OdId, std::vector<std::pair<std::vector<Routes::InternWaypoint>, double>>>>
    Main::profileCatalogRouteDistributions{};
std::unordered_map<std::string, std::vector<Routes::InternWaypoint>> Main::scenarioRoutes{};

std::unordered_map<std::string, std::vector<Routes::InternWaypoint>> Main::ScenarioRouteConverter(std::string scenarioConfigPath) {
    std::unordered_map<std::string, std::vector<Routes::InternWaypoint>> routes;
    Routes::RouteImporter routeImporter(scenarioConfigPath);
    auto routeImport = routeImporter.GetDReaMRoute();
    std::transform(routeImport.begin(), routeImport.end(), std::inserter(routes, routes.begin()), [this](auto element) {
        std::pair<std::string, std::vector<Routes::InternWaypoint>> result;
        auto wayPoints = this->routeConverter.Convert(element.second);
        return std::make_pair(element.first, wayPoints);
    });
    return routes;
}

std::unordered_map<DReaMDefinitions::AgentVehicleType,
                   std::unordered_map<OdId, std::vector<std::pair<std::vector<Routes::InternWaypoint>, double>>>>
Main::ProfilesRouteConverter(ProfilesInterface *profile) {
    try {
        std::unordered_map<DReaMDefinitions::AgentVehicleType,
                           std::unordered_map<OdId, std::vector<std::pair<std::vector<Routes::InternWaypoint>, double>>>>
            routesMap;
        auto spawnerProfiles = profile->GetProfileGroups().at("Spawner");
        for (const auto &[profileName, profileList] : spawnerProfiles) {
            DReaMDefinitions::AgentVehicleType agentType = DReaMDefinitions::AgentVehicleType::NONE;
            OdId startRoad;
            for (const auto &[listName, routesVec] : profileList) {
                if (listName == "Routes") {
                    for (const auto &route : std::get<openpass::parameter::internal::ParameterListLevel1>(routesVec)) {
                        std::vector<Routes::Import::Position> routeImport{};
                        std::vector<OdId> roads{};
                        std::vector<int> lanes{};
                        std::vector<double> s{};
                        double weight;
                        for (const auto &[key, value] : route) {
                            const auto &routeElement = std::get<openpass::parameter::internal::ParameterValue>(value);
                            if (key == "AgentType") {
                                auto type = std::get<std::string>(routeElement);
                                if (type == "Car" || type == "Truck" || type == "Bus" || type == "Motorbike") {
                                    agentType = DReaMDefinitions::AgentVehicleType::Car;
                                }
                                else if (type == "Cyclist") {
                                    agentType = DReaMDefinitions::AgentVehicleType::Bicycle;
                                }
                                else if (type == "Pedestrian") {
                                    agentType = DReaMDefinitions::AgentVehicleType::Pedestrian;
                                }
                                else {
                                    agentType = DReaMDefinitions::AgentVehicleType::Undefined;
                                }
                            }
                            if (key == "openDriveRoadIDs") {
                                startRoad = std::get<std::vector<std::string>>(routeElement).at(0);
                                for (auto id : std::get<std::vector<std::string>>(routeElement)) {
                                    roads.push_back(id);
                                }
                            }
                            if (key == "openDriveLaneID") {
                                for (auto id : std::get<std::vector<double>>(routeElement)) {
                                    lanes.push_back(id);
                                }
                            }
                            if (key == "SCoordinates") {
                                for (auto sCoordinates : std::get<std::vector<double>>(routeElement)) {
                                    s.push_back(sCoordinates);
                                }
                            }
                            if (key == "Weight") {
                                weight = std::get<double>(routeElement);
                            }
                        }

                        if (s.size() != lanes.size() || s.size() != roads.size()) {
                            const std::string msg =
                                __FILE__ " " + std::to_string(__LINE__) +
                                " error in ProfilesCatalo: elementes of (Roads |SCoordinates | tCoordinates) in Routes " +
                                "list have not the same size";
                            throw std::logic_error(msg);
                        }

                        for (unsigned int i = 0; i < roads.size(); i++) {
                            Routes::Import::LanePosition lanePosition;
                            lanePosition.roadId = roads.at(i);
                            lanePosition.s = s.at(i);
                            lanePosition.laneId = lanes.at(i);
                            routeImport.push_back(lanePosition);
                        }
                        auto [it, success] =
                            routesMap.insert({agentType, {{startRoad, {std::make_pair(routeConverter.Convert(routeImport), weight)}}}});

                        if (!success) {
                            auto [it2, success2] =
                                it->second.insert({startRoad, {std::make_pair(routeConverter.Convert(routeImport), weight)}});
                            if (!success2) {
                                it2->second.push_back({routeConverter.Convert(routeImport), weight});
                            }
                        }
                    }
                }
            }
        }
        return routesMap;
    }
    catch (const std::logic_error &error) {
        throw std::runtime_error(error.what());
    }
}

void Main::SetInitialRoute(AgentInterface *agent) {
    auto it = std::find_if(scenarioRoutes.begin(), scenarioRoutes.end(),
                           [agent](auto element) { return element.first == agent->GetScenarioName(); });
    if (it != scenarioRoutes.end()) {
        apConverter.SetInitialRoute(agent, it->second);
        return;
    }
    else {
        const auto &egoAgent = const_cast<AgentInterface *>(agent)->GetEgoAgent();
        DReaMDefinitions::AgentVehicleType vehicleType;
        auto agentType = agent->GetVehicleModelParameters().vehicleType;

        if (agentType == AgentVehicleType::Car || agentType == AgentVehicleType::Motorbike || agentType == AgentVehicleType::Truck) {
            vehicleType = DReaMDefinitions::AgentVehicleType::Car;
        }
        else if (agentType == AgentVehicleType::Bicycle) {
            vehicleType = DReaMDefinitions::AgentVehicleType::Bicycle;
        }
        else if (agentType == AgentVehicleType::Pedestrian) {
            vehicleType = DReaMDefinitions::AgentVehicleType::Pedestrian;
        }
        else {
            vehicleType = DReaMDefinitions::AgentVehicleType::Undefined;
        }
        auto vehicleRoutes = std::find_if(profileCatalogRouteDistributions.begin(), profileCatalogRouteDistributions.end(),
                                          [vehicleType](auto element) { return element.first == vehicleType; });
        if (vehicleRoutes != profileCatalogRouteDistributions.end()) {
            auto routes = std::find_if(vehicleRoutes->second.begin(), vehicleRoutes->second.end(),
                                       [&egoAgent](auto element) { return element.first == egoAgent.GetMainLocatePosition().roadId; });
            if (routes != vehicleRoutes->second.end()) {
                auto route = Sampler::Sample(routes->second, stochastics);
                apConverter.SetInitialRoute(agent, route);
                return;
            }
        }
    }
    apConverter.SetInitialRoute(agent, {});
}

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
