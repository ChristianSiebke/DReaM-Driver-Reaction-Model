#include "ConflictAreaCalculator.h"

namespace GlobalObserver::Calculators {
void ConflictAreaCalculator::Populate() {
    if (conflictAreasCreated)
        return;

    for (const auto &currentLane : infrastructurePerception->lanes) {
        for (const auto &intersectionLane : infrastructurePerception->lanes) {
            if (PotentialConflictAreaExist(currentLane, intersectionLane)) {
                if (auto conflictAreas = CalculateConflictAreas(currentLane.get(), intersectionLane.get())) {
                    const_cast<MentalInfrastructure::Lane *>(currentLane.get())
                        ->AddConflictArea({intersectionLane.get(), conflictAreas->first});
                    const_cast<MentalInfrastructure::Lane *>(intersectionLane.get())
                        ->AddConflictArea({currentLane.get(), conflictAreas->second});

                    std::string junctionInvalid = "not on Junction";
                    std::string junctionIdFirst = conflictAreas->first.road->IsOnJunction()
                                                      ? conflictAreas->first.road->GetJunction()->GetOpenDriveId()
                                                      : junctionInvalid;
                    std::string junctionIdSecond = conflictAreas->second.road->IsOnJunction()
                                                       ? conflictAreas->second.road->GetJunction()->GetOpenDriveId()
                                                       : junctionInvalid;
                    std::string junctionId = junctionIdFirst != junctionInvalid ? junctionIdFirst : junctionIdSecond;
                    std::vector<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>> vec{};

                    infrastructurePerception->conflictAreas.insert({junctionId, vec});
                    infrastructurePerception->conflictAreas.at(junctionId)
                        .push_back(std::make_pair(conflictAreas->first, conflictAreas->second));
                }
            }
        }
    }

    conflictAreasCreated = true;
}

bool ConflictAreaCalculator::PotentialConflictAreaExist(const std::shared_ptr<const MentalInfrastructure::Lane> currentLane,
                                                        const std::shared_ptr<const MentalInfrastructure::Lane> intersectionLane) const {
    if (currentLane == intersectionLane)
        return false;
    if (currentLane->GetConflictAreaWithLane(intersectionLane.get()))
        return false;
    if ((currentLane->GetType() == MentalInfrastructure::LaneType::Shoulder ||
         intersectionLane->GetType() == MentalInfrastructure::LaneType::Shoulder) ||
        (currentLane->GetType() == MentalInfrastructure::LaneType::Border ||
         intersectionLane->GetType() == MentalInfrastructure::LaneType::Border) ||
        (currentLane->GetType() == MentalInfrastructure::LaneType::Parking ||
         intersectionLane->GetType() == MentalInfrastructure::LaneType::Parking))
        return false;
    if (LanesDoNotIntersect(currentLane.get(), intersectionLane.get()))
        return false;
    if (LanesHavePotentialConfliceArea(currentLane.get(), intersectionLane.get()))
        return true;
    return false;
}

bool ConflictAreaCalculator::LanesDoNotIntersect(const MentalInfrastructure::Lane *laneA, const MentalInfrastructure::Lane *laneB) const {
    const auto &roadA = laneA->GetRoad();
    const auto &roadB = laneB->GetRoad();

    // lanes in row
    if (roadA == roadB) {
        return true;
    }
    if ((roadA->GetSuccessor() == roadB && roadA->GetSuccessor() != nullptr) ||
        (roadA->GetPredecessor() == roadB && roadA->GetPredecessor() != nullptr)) {
        return true;
    }
    if ((roadA->GetSuccessor() == roadB->GetJunction() && roadA->GetSuccessor() != nullptr) ||
        (roadA->GetPredecessor() == roadB->GetJunction() && roadA->GetPredecessor() != nullptr)) {
        return true;
    }
    // junction between lanes
    if ((roadA->IsSuccessorJunction() && roadB->IsSuccessorJunction()) ||
        (roadA->IsSuccessorJunction() && roadB->IsPredecessorJunction()) ||
        (roadA->IsPredecessorJunction() && roadB->IsSuccessorJunction()) ||
        (roadA->IsPredecessorJunction() && roadB->IsPredecessorJunction())) {
        return true;
    }
    return false;
}

bool ConflictAreaCalculator::LanesHavePotentialConfliceArea(const MentalInfrastructure::Lane *laneA,
                                                            const MentalInfrastructure::Lane *laneB) const {
    auto roadA = laneA->GetRoad();
    auto roadB = laneB->GetRoad();
    if ((roadA->GetJunction() == roadB->GetJunction() && roadA->GetJunction() != nullptr) &&
        !(roadA->GetPredecessor() == roadB->GetPredecessor()))
        return true;

    if (IntersectionPoints(laneA->GetFirstPoint(), laneA->GetLastPoint(), laneB->GetFirstPoint(), laneB->GetLastPoint()))
        return true;

    return false;
}

std::optional<std::pair<MentalInfrastructure::LanePoint, MentalInfrastructure::LanePoint>>
ConflictAreaCalculator::IntersectionPoints(const MentalInfrastructure::LanePoint *p1, const MentalInfrastructure::LanePoint *p2,
                                           const MentalInfrastructure::LanePoint *q1, const MentalInfrastructure::LanePoint *q2) const {
    auto point = Common::IntersectionPoint({p1->x, p1->y}, {p2->x, p2->y}, {q1->x, q1->y}, {q2->x, q2->y});
    if (!point) {
        return std::nullopt;
    }
    double threshold = 0.3;
    if (((std::min(p1->x, p2->x) <= point->x + threshold && std::max(p1->x, p2->x) >= point->x - threshold) &&
         ((std::min(p1->y, p2->y) <= point->y + threshold && std::max(p1->y, p2->y) >= point->y - threshold))) &&
        ((std::min(q1->x, q2->x) <= point->x + threshold && std::max(q1->x, q2->x) >= point->x - threshold) &&
         ((std::min(q1->y, q2->y) <= point->y + threshold && std::max(q1->y, q2->y) >= point->y - threshold)))) {
        auto pS = std::sqrt((std::pow((point->x - p1->x), 2)) + (std::pow((point->y - p1->y), 2)));
        auto qS = std::sqrt((std::pow((point->x - q1->x), 2)) + (std::pow((point->y - q1->y), 2)));
        return {{{point->x, point->y, p1->hdg, p1->sOffset + pS}, {point->x, point->y, q1->hdg, q1->sOffset + qS}}};
    }
    return std::nullopt;
}

std::optional<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>>
ConflictAreaCalculator::CalculateConflictAreas(const MentalInfrastructure::Lane *currentLane,
                                               const MentalInfrastructure::Lane *intersectionLane) const {
    auto leftLeft = CalculateLaneIntersectionPoints(currentLane->GetLeftSidePoints(), intersectionLane->GetLeftSidePoints());
    auto leftRight = CalculateLaneIntersectionPoints(currentLane->GetLeftSidePoints(), intersectionLane->GetRightSidePoints());
    auto rightLeft = CalculateLaneIntersectionPoints(currentLane->GetRightSidePoints(), intersectionLane->GetLeftSidePoints());
    auto rightRight = CalculateLaneIntersectionPoints(currentLane->GetRightSidePoints(), intersectionLane->GetRightSidePoints());

    int intersectionPointNumber = !leftLeft.first.empty();
    intersectionPointNumber += !leftRight.first.empty();
    intersectionPointNumber += !rightLeft.first.empty();
    intersectionPointNumber += !rightRight.first.empty();
    // at least three intersection points have to exist to define conflict area
    if (intersectionPointNumber < 3)
        return std::nullopt;
    std::vector<MentalInfrastructure::LanePoint> conflictPointsCL;
    std::move(leftLeft.first.begin(), leftLeft.first.end(), std::back_inserter(conflictPointsCL));
    std::move(leftRight.first.begin(), leftRight.first.end(), std::back_inserter(conflictPointsCL));
    std::move(rightLeft.first.begin(), rightLeft.first.end(), std::back_inserter(conflictPointsCL));
    std::move(rightRight.first.begin(), rightRight.first.end(), std::back_inserter(conflictPointsCL));

    std::vector<MentalInfrastructure::LanePoint> conflictPointsIL;
    std::move(leftLeft.second.begin(), leftLeft.second.end(), std::back_inserter(conflictPointsIL));
    std::move(leftRight.second.begin(), leftRight.second.end(), std::back_inserter(conflictPointsIL));
    std::move(rightLeft.second.begin(), rightLeft.second.end(), std::back_inserter(conflictPointsIL));
    std::move(rightRight.second.begin(), rightRight.second.end(), std::back_inserter(conflictPointsIL));

    auto minPointCL =
        *std::min_element(conflictPointsCL.begin(), conflictPointsCL.end(), [](auto a, auto b) { return a.sOffset < b.sOffset; });
    auto maxPointCL =
        *std::max_element(conflictPointsCL.begin(), conflictPointsCL.end(), [](auto a, auto b) { return a.sOffset < b.sOffset; });

    auto minPointIL =
        *std::min_element(conflictPointsIL.begin(), conflictPointsIL.end(), [](auto a, auto b) { return a.sOffset < b.sOffset; });
    auto maxPointIL =
        *std::max_element(conflictPointsIL.begin(), conflictPointsIL.end(), [](auto a, auto b) { return a.sOffset < b.sOffset; });

    MentalInfrastructure::ConflictArea clConflictArea;
    clConflictArea.start = minPointCL.sOffset <= currentLane->GetFirstPoint()->sOffset ? *currentLane->GetFirstPoint()
                                                                                       : currentLane->InterpolatePoint(minPointCL.sOffset);
    clConflictArea.end = maxPointCL.sOffset >= currentLane->GetLastPoint()->sOffset ? *currentLane->GetLastPoint()
                                                                                    : currentLane->InterpolatePoint(maxPointCL.sOffset);
    clConflictArea.junction = currentLane->GetRoad()->GetJunction();
    clConflictArea.road = currentLane->GetRoad();
    clConflictArea.lane = currentLane;

    MentalInfrastructure::ConflictArea ilConflictArea;
    ilConflictArea.start = minPointIL.sOffset <= intersectionLane->GetFirstPoint()->sOffset
                               ? *intersectionLane->GetFirstPoint()
                               : intersectionLane->InterpolatePoint(minPointIL.sOffset);
    ilConflictArea.end = maxPointIL.sOffset >= intersectionLane->GetLastPoint()->sOffset
                             ? *intersectionLane->GetLastPoint()
                             : intersectionLane->InterpolatePoint(maxPointIL.sOffset);
    ilConflictArea.junction = intersectionLane->GetRoad()->GetJunction();
    ilConflictArea.road = intersectionLane->GetRoad();
    ilConflictArea.lane = intersectionLane;
    return {{clConflictArea, ilConflictArea}};
}

std::pair<std::vector<MentalInfrastructure::LanePoint>, std::vector<MentalInfrastructure::LanePoint>>
ConflictAreaCalculator::CalculateLaneIntersectionPoints(const std::list<MentalInfrastructure::LanePoint> &lanePointsA,
                                                        const std::list<MentalInfrastructure::LanePoint> &lanePointsB) const {
    std::vector<MentalInfrastructure::LanePoint> intersectionPointA;
    std::vector<MentalInfrastructure::LanePoint> intersectionPointB;
    for (auto pA1 = lanePointsA.begin(), pA2 = std::next(pA1); pA2 != lanePointsA.end(); pA1++, pA2++) {
        for (auto pB1 = lanePointsB.begin(), pB2 = std::next(pB1); pB2 != lanePointsB.end(); pB1++, pB2++) {
            if (auto result = IntersectionPoints(&(*pA1), &(*pA2), &(*pB1), &(*pB2))) {
                intersectionPointA.push_back(result->first);
                intersectionPointB.push_back(result->second);
            }
        }
    }
    return {intersectionPointA, intersectionPointB};
}
} // namespace GlobalObserver::Calculators
