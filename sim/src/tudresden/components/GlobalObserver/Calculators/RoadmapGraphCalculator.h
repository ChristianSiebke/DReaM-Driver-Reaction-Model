#pragma once

#include "common/PerceptionData.h"

namespace GlobalObserver::Calculators {
class RoadmapGraphCalculator {
public:
    RoadmapGraphCalculator(std::shared_ptr<InfrastructurePerception> infrastructurePerception) :
        infrastructurePerception(infrastructurePerception) {
    }

    void Populate();

private:
    RoadmapGraph::RoadmapGraph CreateGraph(std::vector<std::shared_ptr<const MentalInfrastructure::Lane>> &lanes);

private:
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;
    bool graphCreated = false;
};
}