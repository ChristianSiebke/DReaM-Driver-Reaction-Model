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

    void Trigger(int time);

    void TriggerRoadNetworkConversion();

    std::shared_ptr<DetailedAgentPerception> GetDetailedAgentPerception(int agentId);
    std::vector<std::shared_ptr<GeneralAgentPerception>> GetGeneralAgentPerception(std::vector<int> agentIds);
    std::shared_ptr<InfrastructurePerception> GetInfrastructurePerception();
    std::vector<const MentalInfrastructure::TrafficSignal *> GetVisibleTrafficSignals(std::vector<OdId> ids);

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
    static std::shared_ptr<Main> instance;
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