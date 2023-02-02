#pragma once

#include <map>
#include <optional>
#include <vector>

#include "Routes/RouteConverter.h"
#include "common/PerceptionData.h"
#include "common/RoutePlanning/RouteCalculation.h"

namespace GlobalObserver::Converters {
class AgentPerceptionConverter {
public:
    AgentPerceptionConverter(WorldInterface *world, StochasticsInterface *stochastics,
                             std::shared_ptr<InfrastructurePerception> infrastructurePerception,
                             std::vector<std::shared_ptr<DetailedAgentPerception>> &agentPerceptions) :
        world(world), stochastics(stochastics), infrastructurePerception(infrastructurePerception), agentPerceptions(agentPerceptions) {
    }

    void Populate();

    void SetInitialRoute(int agentId, std::vector<GlobalObserver::Routes::InternWaypoint> route);

private:
    std::shared_ptr<DetailedAgentPerception> ConvertAgent(const AgentInterface *agent);
    Common::Vector2d GetDriverPosition(const AgentInterface *agent) const;
    std::optional<std::vector<GlobalObserver::Routes::InternWaypoint>> RouteUpdate(const AgentInterface *agent) const;
    bool CheckForRouteUpdate(const AgentInterface *agent) const;
    DReaMRoute::Waypoints ConvertRoute(std::vector<GlobalObserver::Routes::InternWaypoint> route);

private:
    WorldInterface *world;
    StochasticsInterface *stochastics;
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;

    std::vector<std::shared_ptr<DetailedAgentPerception>> &agentPerceptions;
    std::map<int, std::shared_ptr<DetailedAgentPerception>> idMapping;
    std::map<int, DReaMRoute::Waypoints> routeMapping;
};
} // namespace GlobalObserver::Converters