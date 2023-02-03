#pragma once

#include <map>
#include <optional>
#include <vector>

#include "Routes/RouteConverter.h"
#include "common/PerceptionData.h"
#include "common/RoutePlanning/RouteCalculation.h"

namespace GlobalObserver::Converters {

/**
 * @brief Class for handling the conversion of all agents to the format used by DReaM. Will generate a list of DetailedAgentPerception
 * which can later be casted to GeneralAgentPerception to hide some of the fields.
 *
 */
class AgentPerceptionConverter {
public:
    AgentPerceptionConverter(WorldInterface *world, StochasticsInterface *stochastics,
                             std::shared_ptr<InfrastructurePerception> infrastructurePerception,
                             std::vector<std::shared_ptr<DetailedAgentPerception>> &agentPerceptions) :
        world(world), stochastics(stochastics), infrastructurePerception(infrastructurePerception), agentPerceptions(agentPerceptions) {
    }

    /**
     * @brief Triggers the internal conversion logic and populates the list of agent perceptions.
     *
     */
    void Populate();

    /**
     * @brief Set the initial route for an agent. Will convert the route to DReaM::Waypoints.
     *
     * @param agentId the agent for which to set the initial route
     * @param route the route that this agent should / will take
     */
    void SetInitialRoute(int agentId, std::vector<GlobalObserver::Routes::InternWaypoint> route);

private:
    /**
     * @brief Converts a given agent into a DetailedAgentPerception.
     *
     * @param agent the agent to be converted
     */
    std::shared_ptr<DetailedAgentPerception> ConvertAgent(const AgentInterface *agent);

    /**
     * @brief Calculates & returns the position of the driver inside the agent.
     *
     * @param agent the agent to calculate the position of
     */
    Common::Vector2d GetDriverPosition(const AgentInterface *agent) const;

    /**
     * @brief Checks whether the route of an agent must be updated. This might be the case because the agent has no valid route or finished
     * their previous route.
     *
     * @param agent the agent to check
     * @return true a new route needs to be picked
     * @return false no route update is required
     */
    bool CheckForRouteUpdate(const AgentInterface *agent) const;

    /**
     * @brief Calculates a new route for an agent. Due to internal errors this might result in no route.
     *
     * @param agent the agent to calculate a new route for
     */
    std::optional<std::vector<GlobalObserver::Routes::InternWaypoint>> RouteUpdate(const AgentInterface *agent) const;

    /**
     * @brief Converts a route of InternWaypoints to DReaMRoute::Waypoints.
     *
     * @param route the route to convert
     */
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