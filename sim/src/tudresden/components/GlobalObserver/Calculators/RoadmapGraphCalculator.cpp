#include "RoadmapGraphCalculator.h"

namespace GlobalObserver::Calculators {

void RoadmapGraphCalculator::Populate() {
    if (graphCreated)
        return;
    infrastructurePerception->graph = CreateGraph(infrastructurePerception->lanes);
    graphCreated = true;
}

RoadmapGraph::RoadmapGraph RoadmapGraphCalculator::CreateGraph(std::vector<std::shared_ptr<const MentalInfrastructure::Lane>> &lanes) {
    RoadmapGraph::RoadmapGraph g(lanes);
    return g;
}
} // namespace GlobalObserver::Calculator