#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include "Calculators/ConflictAreaCalculator.h"
#include "Calculators/RoadmapGraphCalculator.h"
#include "Calculators/StoppingPointCalculator.h"
#include "Converters/AgentPerceptionConverter.h"
#include "Converters/RoadNetworkConverter.h"
#include "common/PerceptionData.h"

#ifdef QMAKE_BUILD
#define GlobalObserverIMPORT
#define GlobalObserverEXPORT
#else
#if defined(_WIN32) && !defined(NODLL)
#define GlobalObserverIMPORT __declspec(dllimport)
#define GlobalObserverEXPORT __declspec(dllexport)

#elif (defined(__GNUC__) && __GNUC__ >= 4 || defined(__clang__))
#define GlobalObserverEXPORT __attribute__((visibility("default")))
#define GlobalObserverIMPORT GlobalObserverEXPORT

#else
#define GlobalObserverIMPORT
#define GlobalObserverEXPORT
#endif
#endif

#if defined(GlobalObserver_EXPORTS)
#define EXPORT GlobalObserverEXPORT
#else
#define EXPORT GlobalObserverIMPORT
#endif

namespace GlobalObserver {

/**
 * @brief Singleton shared by all agents, provides the core GlobalObserver functionality. Will update internal components as needed
 * whenever the \code Trigger() \endcode method is invoked.
 *
 */
class EXPORT Main {
public:
    static std::shared_ptr<Main> GetInstance(WorldInterface *world, StochasticsInterface *stochastics) {
        if (!instance)
            instance = std::shared_ptr<Main>(new Main(world, stochastics));
        return instance;
    }

    ~Main() {
        std::cout << "GlobalObserver::Main destroyed" << std::endl;
    }

    Main(Main const &) = delete;
    Main &operator=(Main const &) = delete;

    /**
     * @brief Resets the instance of GlobalObserver.
     *
     */
    static void Reset() {
        instance.reset();
    }

    /**
     * @brief Sets the current run ID.
     *
     */
    static void SetRunId(int invocation) {
        runId = invocation;
    }

    /**
     * @brief Triggers an update of the internally stored shared representations as well as the logic for detecting and categorizing crash
     * events.
     *
     * @param time current time in the simulation
     */
    void Trigger(int time);

    /**
     * @brief Triggers ONLY the conversion of parts of the road network that are required for performing route conversion.
     *
     */
    void TriggerRoadNetworkConversion();

    /**
     * @brief Returns a detailed AgentPerception used for representing the driver (all information is available to the driver).
     *
     * @param agentId id of the agent (driver) to return detailed information on
     */
    std::shared_ptr<DetailedAgentPerception> GetDetailedAgentPerception(int agentId);

    /**
     * @brief Returns a list of general AgentPerceptions used for representing agents surrounding a driver.
     *
     * @param agentIds list of ids of agents that are visible to the driver
     */
    std::vector<std::shared_ptr<GeneralAgentPerception>> GetGeneralAgentPerception(std::vector<int> agentIds);

    /**
     * @brief Returns the perceived (converted) infrastructure.
     *
     */
    std::shared_ptr<InfrastructurePerception> GetInfrastructurePerception();

    /**
     * @brief Returns a list of converted traffic signals.
     *
     * @param ids list of ids of traffic signals that are visible to the driver
     */
    std::vector<const MentalInfrastructure::TrafficSignal *> GetVisibleTrafficSignals(std::vector<OdId> ids);

    /**
     * @brief Set the initial route for an agent, will pass through to AgentPerceptionConverter::SetInitialRoute.
     *
     * @param agentId the agent for which to set the initial route
     * @param route the route that this agent should / will take
     */
    void SetInitialRoute(int agentId, std::vector<GlobalObserver::Routes::InternWaypoint> route);

private:
    Main(WorldInterface *world, StochasticsInterface *stochastics) :
        world(world),
        stochastics(stochastics),
        infrastructurePerception(std::make_shared<InfrastructurePerception>()),
        rnConverter(world, infrastructurePerception),
        caCalculator(infrastructurePerception),
        rgCalculator(infrastructurePerception),
        spCalculator(infrastructurePerception),
        apConverter(world, stochastics, infrastructurePerception, agentPerceptions) {
    }

private:
    // singleton related fields
    static std::shared_ptr<Main> instance;
    static int runId;

    // internal fields
    WorldInterface *world;
    StochasticsInterface *stochastics;
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;
    int lastConversionTime;

    // infrastructure related fields
    GlobalObserver::Converters::RoadNetworkConverter rnConverter;
    GlobalObserver::Calculators::ConflictAreaCalculator caCalculator;
    GlobalObserver::Calculators::RoadmapGraphCalculator rgCalculator;
    GlobalObserver::Calculators::StoppingPointCalculator spCalculator;
    bool staticInfrastructureCreated = false;

    // agent perception related fields
    GlobalObserver::Converters::AgentPerceptionConverter apConverter;
    std::vector<std::shared_ptr<DetailedAgentPerception>> agentPerceptions;
    bool agentPerceptionsCreated = false;
};

}