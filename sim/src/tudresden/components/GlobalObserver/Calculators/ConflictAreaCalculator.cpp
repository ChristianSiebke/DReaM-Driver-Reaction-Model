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
                    infrastructurePerception->conflictAreas.at(junctionId).push_back(std::move(*conflictAreas));
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
        auto pSDistance1 = std::sqrt((std::pow((point->x - p1->x), 2)) + (std::pow((point->y - p1->y), 2)));
        auto pSDistance2 = std::sqrt((std::pow((point->x - p2->x), 2)) + (std::pow((point->y - p2->y), 2)));
        auto qSDistance1 = std::sqrt((std::pow((point->x - q1->x), 2)) + (std::pow((point->y - q1->y), 2)));
        auto qSDistance2 = std::sqrt((std::pow((point->x - q2->x), 2)) + (std::pow((point->y - q2->y), 2)));
        double pS, qS;
        if (pSDistance1 > 0 && pSDistance2 > 0) {
            pS = ((p1->sOffset / pSDistance1) + (p2->sOffset / pSDistance2)) / ((1 / pSDistance1) + (1 / pSDistance2));
        }
        else if (std::abs(pSDistance1) < 0.001) {
            pS = p1->sOffset;
        }
        else if (std::abs(pSDistance2) < 0.001) {
            pS = p2->sOffset;
        }

        if (qSDistance1 > 0 && qSDistance2 > 0) {
            qS = ((q1->sOffset / qSDistance1) + (q2->sOffset / qSDistance2)) / ((1 / qSDistance1) + (1 / qSDistance2));
        }
        else if (std::abs(qSDistance1) < 0.001) {
            qS = q1->sOffset;
        }
        else if (std::abs(qSDistance2) < 0.001) {
            qS = q2->sOffset;
        }
        return {{{point->x, point->y, p1->hdg, pS}, {point->x, point->y, q1->hdg, qS}}};
    }
    return std::nullopt;
}

std::optional<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>>
ConflictAreaCalculator::CalculateConflictAreas(const MentalInfrastructure::Lane *currentLane,
                                               const MentalInfrastructure::Lane *intersectionLane) const {
    auto minPoint = [](std::vector<MentalInfrastructure::LanePoint> vec) {
        return *std::min_element(vec.begin(), vec.end(), [](auto a, auto b) { return a.sOffset < b.sOffset; });
    };
    auto maxPoint = [](std::vector<MentalInfrastructure::LanePoint> vec) {
        return *std::max_element(vec.begin(), vec.end(), [](auto a, auto b) { return a.sOffset < b.sOffset; });
    };
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
    if (!leftLeft.first.empty())
        conflictPointsCL.push_back(maxPoint(leftLeft.first));
    if (!leftRight.first.empty())
        conflictPointsCL.push_back(maxPoint(leftRight.first));
    if (!rightLeft.first.empty())
        conflictPointsCL.push_back(maxPoint(rightLeft.first));
    if (!rightRight.first.empty())
        conflictPointsCL.push_back(maxPoint(rightRight.first));
    std::vector<MentalInfrastructure::LanePoint> conflictPointsIL;
    if (!leftLeft.second.empty())
        conflictPointsIL.push_back(maxPoint(leftLeft.second));
    if (!leftRight.second.empty())
        conflictPointsIL.push_back(maxPoint(leftRight.second));
    if (!rightLeft.second.empty())
        conflictPointsIL.push_back(maxPoint(rightLeft.second));
    if (!rightRight.second.empty())
        conflictPointsIL.push_back(maxPoint(rightRight.second));

    std::vector<std::pair<MentalInfrastructure::LanePoint, MentalInfrastructure::LanePoint>> cAPoints;
    for (unsigned int i = 0; i < leftLeft.first.size(); i++) {
        cAPoints.push_back(std::make_pair(leftLeft.first[i], leftLeft.second[i]));
    }
    for (unsigned int i = 0; i < leftRight.first.size(); i++) {
        cAPoints.push_back(std::make_pair(leftRight.first[i], leftRight.second[i]));
    }
    for (unsigned int i = 0; i < rightLeft.first.size(); i++) {
        cAPoints.push_back(std::make_pair(rightLeft.first[i], rightLeft.second[i]));
    }
    for (unsigned int i = 0; i < rightRight.first.size(); i++) {
        cAPoints.push_back(std::make_pair(rightRight.first[i], rightRight.second[i]));
    }
    auto findPartnerPointWithMaxS = [cAPoints](std::vector<MentalInfrastructure::LanePoint> points) {
        double s = -1;
        MentalInfrastructure::LanePoint result;
        for (auto point : points) {
            for (auto element : cAPoints) {
                if (point == element.first) {
                    if (s < element.second.sOffset) {
                        s = element.second.sOffset;
                        result = element.second;
                    }
                }
                if (point == element.second) {
                    if (s < element.first.sOffset) {
                        s = element.first.sOffset;
                        result = element.first;
                    }
                }
            }
        }
        return result;
    };
    auto maxPointIL = findPartnerPointWithMaxS(conflictPointsCL);
    auto maxPointCL = findPartnerPointWithMaxS(conflictPointsIL);

    MentalInfrastructure::ConflictArea clConflictArea;
    clConflictArea.start = minPoint(conflictPointsCL).sOffset <= currentLane->GetFirstPoint()->sOffset
                               ? *currentLane->GetFirstPoint()
                               : currentLane->InterpolatePoint(minPoint(conflictPointsCL).sOffset);
    clConflictArea.end = maxPointCL.sOffset >= currentLane->GetLastPoint()->sOffset ? *currentLane->GetLastPoint()
                                                                                    : currentLane->InterpolatePoint(maxPointCL.sOffset);
    clConflictArea.junction = currentLane->GetRoad()->GetJunction();
    clConflictArea.road = currentLane->GetRoad();
    clConflictArea.lane = currentLane;

    MentalInfrastructure::ConflictArea ilConflictArea;
    ilConflictArea.start = minPoint(conflictPointsIL).sOffset <= intersectionLane->GetFirstPoint()->sOffset
                               ? *intersectionLane->GetFirstPoint()
                               : intersectionLane->InterpolatePoint(minPoint(conflictPointsIL).sOffset);
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
