#pragma once

#include "common/PerceptionData.h"
#include "common/Helper.h"

namespace GlobalObserver::Calculators {

/**
 * @brief Class for handling the calculation of conflict areas in an already created internal infrastructure format.
 *
 */
class ConflictAreaCalculator {
public:
    ConflictAreaCalculator(std::shared_ptr<InfrastructurePerception> infrastructurePerception) :
        infrastructurePerception(infrastructurePerception) {
    }

    /**
     * @brief Triggers the internal conversion logic and populates the InfrastructurePerception. Checks whether the conversion was already
     * performed to avoid double conversion.
     *
     */
    void Populate();

private:
    bool PotentialConflictAreaExist(const std::shared_ptr<const MentalInfrastructure::Lane> currentLane,
                                    const std::shared_ptr<const MentalInfrastructure::Lane> intersectionLane) const;
    bool LanesDoNotIntersect(const MentalInfrastructure::Lane *laneA, const MentalInfrastructure::Lane *laneB) const;

    bool LanesHavePotentialConfliceArea(const MentalInfrastructure::Lane *laneA, const MentalInfrastructure::Lane *laneB) const;

    std::optional<std::pair<MentalInfrastructure::LanePoint, MentalInfrastructure::LanePoint>>
    IntersectionPoints(const MentalInfrastructure::LanePoint *p1, const MentalInfrastructure::LanePoint *p2,
                       const MentalInfrastructure::LanePoint *q1, const MentalInfrastructure::LanePoint *q2) const;

    std::optional<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>>
    CalculateConflictAreas(const MentalInfrastructure::Lane *currentLane, const MentalInfrastructure::Lane *junctionLane) const;

    std::pair<std::vector<MentalInfrastructure::LanePoint>, std::vector<MentalInfrastructure::LanePoint>>
    CalculateLaneIntersectionPoints(const std::list<MentalInfrastructure::LanePoint> &lanePointsA,
                                    const std::list<MentalInfrastructure::LanePoint> &lanePointsB) const;

private:
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;
    bool conflictAreasCreated = false;
};
} // namespace GlobalObserver::Calculators
