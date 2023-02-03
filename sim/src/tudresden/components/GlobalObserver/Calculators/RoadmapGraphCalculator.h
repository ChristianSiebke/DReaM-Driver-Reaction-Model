#pragma once

#include "common/PerceptionData.h"

namespace GlobalObserver::Calculators {

/**
 * @brief Class for handling the calculation of roadmap graph (lane based) from an already created internal infrastructure format.
 *
 */
class RoadmapGraphCalculator {
public:
    RoadmapGraphCalculator(std::shared_ptr<InfrastructurePerception> infrastructurePerception) :
        infrastructurePerception(infrastructurePerception) {
    }

    /**
     * @brief Triggers the internal conversion logic and populates the InfrastructurePerception. Checks whether the conversion was already
     * performed to avoid double conversion.
     *
     */
    void Populate();

private:
    /**
     * @brief Creates the graph (passes through to RoadmapGraph constructor).
     *
     * @param lanes the lanes to be converted into the graph.
     */
    RoadmapGraph::RoadmapGraph CreateGraph(std::vector<std::shared_ptr<const MentalInfrastructure::Lane>> &lanes);

private:
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;
    bool graphCreated = false;
};
}