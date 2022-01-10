
/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/
#include "WorldRepresentation.h"
#include "Helper.h"
#include <algorithm>
namespace CognitiveMap {

std::optional<PositionAlongRoad> AgentRepresentation::FindNewPositionInDistance(double distance) const {
    return internalData->FindNewPositionInDistance(distance);
}

bool AgentRepresentation::ObservedVehicleCameFromRight(const AgentRepresentation& oAgentPerceptionData) const {
    const auto laneEgoAgent = GetLane();
    const auto laneOAgent = oAgentPerceptionData.GetLane();
    Common::Vector2d referenceVec;
    Common::Vector2d directionVec;
    const auto egoRoad = GetRoad();
    const auto oAgentRoad = oAgentPerceptionData.GetRoad();
    const auto& listOfPoints = laneEgoAgent->GetLanePoints();
    auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " No Intersection Situation ";

    if (egoRoad->IsOnIntersection()) {
        // Calculation of the referenceVec vector of the incoming intersection road of ego
        auto secondPoint = std::next(listOfPoints.begin(), 1);
        referenceVec =
            Common::Vector2d(secondPoint->x - laneEgoAgent->GetFirstPoint()->x, secondPoint->y - laneEgoAgent->GetFirstPoint()->y);

        // Calculation of the direction vector to the examination point
        if (oAgentRoad->IsOnIntersection()) {
            directionVec = Common::Vector2d(laneOAgent->GetFirstPoint()->x - laneEgoAgent->GetFirstPoint()->x,
                                            laneOAgent->GetFirstPoint()->y - laneEgoAgent->GetFirstPoint()->y);
        } else if (oAgentRoad->IsSuccessorIntersection() || oAgentRoad->IsPredecessorIntersection()) {
            directionVec = Common::Vector2d(laneOAgent->GetLastPoint()->x - laneEgoAgent->GetFirstPoint()->x,
                                            laneOAgent->GetLastPoint()->y - laneEgoAgent->GetFirstPoint()->y);
        } else {
            throw std::logic_error(msg);
        }
    } else if (egoRoad->IsSuccessorIntersection() || egoRoad->IsPredecessorIntersection()) {
        // Calculation of the referenceVec vector of the incoming intersection road of ego
        auto secondLastPoint = std::prev(listOfPoints.end(), 2);
        referenceVec =
            Common::Vector2d(laneEgoAgent->GetLastPoint()->x - secondLastPoint->x, laneEgoAgent->GetLastPoint()->y - secondLastPoint->y);

        // Calculation of the direction vector to the examination point
        if (oAgentRoad->IsOnIntersection()) {
            directionVec = Common::Vector2d(laneOAgent->GetFirstPoint()->x - laneEgoAgent->GetLastPoint()->x,
                                            laneOAgent->GetFirstPoint()->y - laneEgoAgent->GetLastPoint()->y);
        } else if (oAgentRoad->IsSuccessorIntersection() || oAgentRoad->IsPredecessorIntersection()) {
            directionVec = Common::Vector2d(laneOAgent->GetLastPoint()->x - laneEgoAgent->GetLastPoint()->x,
                                            laneOAgent->GetLastPoint()->y - laneEgoAgent->GetLastPoint()->y);
        } else {
            throw std::logic_error(msg);
        }
    } else {
        throw std::logic_error(msg);
    }
    return referenceVec.Cross(directionVec) < 0 ? true : false;
}

const MentalInfrastructure::Intersection* AgentRepresentation::NextIntersection() const {
    const auto& currentLane = GetLane();
    const auto& currentRoad = GetRoad();
    const MentalInfrastructure::Intersection* nextIntersection = nullptr;

    if (currentLane->IsInRoadDirection()) {
        if (IsMovingInLaneDirection()) {
            if (currentRoad->IsSuccessorIntersection()) {
                nextIntersection = dynamic_cast<const MentalInfrastructure::Intersection*>(currentRoad->GetSuccessor());
            }
        } else {
            if (currentRoad->IsPredecessorIntersection()) {
                nextIntersection = dynamic_cast<const MentalInfrastructure::Intersection*>(currentRoad->GetPredecessor());
            }
        }
    } else {
        if (IsMovingInLaneDirection()) {
            if (currentRoad->IsPredecessorIntersection()) {
                nextIntersection = dynamic_cast<const MentalInfrastructure::Intersection*>(currentRoad->GetPredecessor());
            }
        } else {
            if (currentRoad->IsSuccessorIntersection()) {
                nextIntersection = dynamic_cast<const MentalInfrastructure::Intersection*>(currentRoad->GetSuccessor());
            }
        }
    }
    return nextIntersection;
}

std::optional<MentalInfrastructure::TrafficSign> AgentRepresentation::NextROWSign() const {
    const auto& egoRoad = GetRoad();
    const auto& egoLane = GetLane();
    bool egoIsMovingInLaneDirection = IsMovingInLaneDirection();
    bool egoLaneInRoadDirection = egoLane->IsInRoadDirection();

    // TODO re-implement

    //    auto rowSigns = FilterROWTrafficSigns(*egoRoad.GetTrafficSigns());

    //    auto findROWSignInDirection = [egoIsMovingInLaneDirection,
    //                                   egoLaneInRoadDirection](const std::vector<MentalInfrastructure::TrafficSign>& rowSigns) {
    //        return std::find_if(rowSigns.begin(), rowSigns.end(),
    //                            [egoIsMovingInLaneDirection, egoLaneInRoadDirection](MentalInfrastructure::TrafficSign element) {
    //                                if (element.GetT() < 0 || (egoIsMovingInLaneDirection && egoLaneInRoadDirection)) {
    //                                    return true;
    //                                } else if (element.GetT() < 0 || (!egoIsMovingInLaneDirection && !egoLaneInRoadDirection)) {
    //                                    return true;
    //                                } else if (element.GetT() > 0 || (egoIsMovingInLaneDirection && !egoLaneInRoadDirection)) {
    //                                    return true;
    //                                } else if (element.GetT() > 0 || (!egoIsMovingInLaneDirection && egoLaneInRoadDirection)) {
    //                                    return true;
    //                                }
    //                                return false;
    //                            });
    //    };

    //    const MentalInfrastructure::TrafficSign* rowSign = nullptr;
    //    std::for_each(rowSigns.begin(), rowSigns.end(), [&rowSign, findROWSignInDirection](const auto& element) {
    //        std::vector<MentalInfrastructure::TrafficSign>::const_iterator rowSignIter = findROWSignInDirection(element.second);
    //        if (rowSignIter != element.second.end()) {
    //            rowSign = &(*rowSignIter);
    //        }
    //    });

    //    if (rowSign) {
    //        return *rowSign;
    //    } else {
    //        return std::nullopt;
    //    }

    return std::nullopt;
}

const std::unordered_map<MentalInfrastructure::TrafficSignType, std::vector<MentalInfrastructure::TrafficSign>>
AgentRepresentation::FilterROWTrafficSigns(
    const std::unordered_map<MentalInfrastructure::TrafficSignType, std::vector<MentalInfrastructure::TrafficSign>>& trafficSignMap) const {
    std::unordered_map<MentalInfrastructure::TrafficSignType, std::vector<MentalInfrastructure::TrafficSign>> result;
    std::copy_if(trafficSignMap.begin(), trafficSignMap.end(), std::inserter(result, result.begin()),
                 [](const std::pair<MentalInfrastructure::TrafficSignType, std::vector<MentalInfrastructure::TrafficSign>> element) {
                     return element.first == MentalInfrastructure::TrafficSignType::Stop ||
                            element.first == MentalInfrastructure::TrafficSignType::GiveWay ||
                            element.first == MentalInfrastructure::TrafficSignType::RightOfWayBegin ||
                            element.first == MentalInfrastructure::TrafficSignType::RightOfWayNextIntersection;
                 });
    return result;
}

std::optional<IntersectionSituation> AgentRepresentation::IntersectionSituation(const AgentRepresentation& observedAgent) const {
    auto egoRoad = GetRoad();
    auto oAgentRoad = observedAgent.GetRoad();

    if (egoRoad->IsOnIntersection() && oAgentRoad->IsOnIntersection()) {
        if (egoRoad->GetIntersection() == oAgentRoad->GetIntersection()) {
            return IntersectionSituation::INTERSECTION_A;
        }
    } else if (egoRoad->IsOnIntersection()) {
        auto intersection = egoRoad->GetIntersection();
        if (observedAgent.IsMovingTowardsIntersection(intersection)) {
            return IntersectionSituation::INTERSECTION_B;
        }
    } else if (oAgentRoad->IsOnIntersection()) {
        auto intersection = oAgentRoad->GetIntersection();
        if (IsMovingTowardsIntersection(intersection)) {
            return IntersectionSituation::INTERSECTION_C;
        }
    }

    const auto& egoLane = GetLane();
    if (IsMovingInLaneDirection()) {
        if (egoLane->IsInRoadDirection() && egoRoad->IsSuccessorIntersection()) {
            auto intersection = dynamic_cast<const MentalInfrastructure::Intersection*>(oAgentRoad->GetSuccessor());
            if (observedAgent.IsMovingTowardsIntersection(intersection)) {
                return IntersectionSituation::INTERSECTION_D;
            }

        } else if (!egoLane->IsInRoadDirection() && egoRoad->IsPredecessorIntersection()) {
            auto intersection = dynamic_cast<const MentalInfrastructure::Intersection*>(oAgentRoad->GetPredecessor());
            if (observedAgent.IsMovingTowardsIntersection(intersection)) {
                return IntersectionSituation::INTERSECTION_D;
            }
        }
    } else {
        if (egoLane->IsInRoadDirection() && egoRoad->IsPredecessorIntersection()) {
            auto intersection = dynamic_cast<const MentalInfrastructure::Intersection*>(oAgentRoad->GetPredecessor());
            if (observedAgent.IsMovingTowardsIntersection(intersection)) {
                return IntersectionSituation::INTERSECTION_D;
            }

        } else if (!egoLane->IsInRoadDirection() && egoRoad->IsSuccessorIntersection()) {
            auto intersection = dynamic_cast<const MentalInfrastructure::Intersection*>(oAgentRoad->GetSuccessor());
            if (observedAgent.IsMovingTowardsIntersection(intersection)) {
                return IntersectionSituation::INTERSECTION_D;
            }
        }
    }
    return std::nullopt;
}

bool AgentRepresentation::IsMovingTowardsIntersection(const MentalInfrastructure::Intersection* intersection) const {
    if (!intersection)
        return false;

    auto roadOfAgent = GetRoad();
    const auto& currentLane = GetLane();
    if (currentLane->IsInRoadDirection()) {
        if (IsMovingInLaneDirection()) {
            if (roadOfAgent->IsSuccessorIntersection()) {
                return roadOfAgent->GetSuccessor()->GetId() == intersection->GetId();
            }
        } else {
            if (roadOfAgent->IsPredecessorIntersection()) {
                return roadOfAgent->GetPredecessor()->GetId() == intersection->GetId();
            }
        }
    } else {
        if (IsMovingInLaneDirection()) {
            if (roadOfAgent->IsPredecessorIntersection()) {
                return roadOfAgent->GetPredecessor()->GetId() == intersection->GetId();
            }
        } else {
            if (roadOfAgent->IsSuccessorIntersection()) {
                return roadOfAgent->GetSuccessor()->GetId() == intersection->GetId();
            }
        }
    }
    return false;
}

double AgentRepresentation::ExtrapolateDistanceAlongLane(double timeStep) const {
    // extrapolated distance
    double extrapolatedDistance = (GetAcceleration() / 2) * std::pow((timeStep), 2) + GetVelocity() * (timeStep);
    auto nextVelocity = GetVelocity() + GetAcceleration() * timeStep;

    if (GetVelocity() * nextVelocity < 0.0) {
        // agent change moving direction -->agent brakes to a standstill
        extrapolatedDistance = std::abs((GetVelocity() * GetVelocity()) / (2 * GetAcceleration()));
    }
    return IsMovingInLaneDirection() ? extrapolatedDistance : -extrapolatedDistance;
}

std::optional<ConflictArea> AgentRepresentation::PossibleConflictAreaAlongLane(const AgentRepresentation& observedAgent) const {
    auto egoLane = GetLane();
    auto egoNextLane = GetNextLane();
    auto observedLane = observedAgent.GetLane();
    auto observedNextLane = observedAgent.GetNextLane();

    if (egoLane->GetConflictAreaWithLane(observedLane) && observedLane->GetConflictAreaWithLane(egoLane)) {
        auto cAEgo = egoLane->GetConflictAreaWithLane(observedLane);
        auto cAObserved = observedLane->GetConflictAreaWithLane(egoLane);
        return DistanceToConflictArea({*cAEgo, egoLane->GetId()}, {*cAObserved, observedLane->GetId()}, observedAgent);

    } else if (egoNextLane && egoNextLane->GetConflictAreaWithLane(observedLane)) {
        auto cAEgo = egoNextLane->GetConflictAreaWithLane(observedLane);
        auto cAObserved = observedLane->GetConflictAreaWithLane(egoNextLane);
        return DistanceToConflictArea({*cAEgo, egoNextLane->GetId()}, {*cAObserved, observedLane->GetId()}, observedAgent);

    } else if (observedNextLane && egoLane->GetConflictAreaWithLane(observedNextLane)) {
        auto cAEgo = egoLane->GetConflictAreaWithLane(observedNextLane);
        auto cAObserved = observedNextLane->GetConflictAreaWithLane(egoLane);
        return DistanceToConflictArea({*cAEgo, egoLane->GetId()}, {*cAObserved, observedNextLane->GetId()}, observedAgent);

    } else if ((egoNextLane && observedNextLane) && egoNextLane->GetConflictAreaWithLane(observedNextLane)) {
        auto cAEgo = egoNextLane->GetConflictAreaWithLane(observedNextLane);
        auto cAObserved = observedNextLane->GetConflictAreaWithLane(egoNextLane);
        return DistanceToConflictArea({*cAEgo, egoNextLane->GetId()}, {*cAObserved, observedNextLane->GetId()}, observedAgent);
    }

    else {
        return std::nullopt;
    }
};

ConflictArea AgentRepresentation::DistanceToConflictArea(std::pair<const MentalInfrastructure::ConflictArea&, Id> egoCA,
                                                         std::pair<const MentalInfrastructure::ConflictArea&, Id> observedCA,
                                                         const AgentRepresentation& observedAgent) const {
    ConflictArea result;
    result.opponentID = observedAgent.GetID();
    auto distanceEgoToStartCA = DistanceToConflictPoint(*this, egoCA.first.start, egoCA.second);
    auto distanceEgoToEndCA = DistanceToConflictPoint(*this, egoCA.first.end, egoCA.second);
    if (!this->IsMovingInLaneDirection()) {
        auto temp = distanceEgoToStartCA;
        distanceEgoToStartCA = distanceEgoToEndCA;
        distanceEgoToEndCA = temp;
    }
    auto distanceObservedToStartCA = DistanceToConflictPoint(observedAgent, observedCA.first.start, observedCA.second);
    auto distanceObservedToEndCA = DistanceToConflictPoint(observedAgent, observedCA.first.end, observedCA.second);
    if (!observedAgent.IsMovingInLaneDirection()) {
        auto temp = distanceObservedToStartCA;
        distanceObservedToStartCA = distanceObservedToEndCA;
        distanceObservedToEndCA = temp;
    }
    result.distanceEgoToCA.start = distanceEgoToStartCA - GetDistanceReferencePointToLeadingEdge();
    result.distanceEgoToCA.end = distanceEgoToEndCA + GetLength() - GetDistanceReferencePointToLeadingEdge();
    result.distanceObservedToCA.start = distanceObservedToStartCA - observedAgent.GetDistanceReferencePointToLeadingEdge();
    result.distanceObservedToCA.end =
        distanceObservedToEndCA + observedAgent.GetLength() - observedAgent.GetDistanceReferencePointToLeadingEdge();

    return result;
}

double AgentRepresentation::DistanceToConflictPoint(const AgentRepresentation& agent,
                                                    const MentalInfrastructure::LanePoint& intersectionPoint, Id laneId) const {
    double distanceToPoint;
    if (laneId == agent.GetLane()->GetId()) {
        distanceToPoint = intersectionPoint.sOffset - agent.GetSCoordinate();
        if (!agent.IsMovingInLaneDirection()) {
            distanceToPoint = agent.GetSCoordinate() - intersectionPoint.sOffset;
        }
    } else if (laneId == agent.GetNextLane()->GetId()) {
        const auto& currentLaneEgo = agent.GetLane();
        const auto& nextLaneEgo = agent.GetNextLane();
        auto distanceFromEgoToEndOfLane = currentLaneEgo->GetLastPoint()->sOffset - agent.GetSCoordinate();
        auto distanceFromStartOfLaneToPoint = intersectionPoint.sOffset - nextLaneEgo->GetFirstPoint()->sOffset;
        if (!agent.IsMovingInLaneDirection()) {
            distanceFromEgoToEndOfLane = agent.GetSCoordinate() - currentLaneEgo->GetFirstPoint()->sOffset;
            distanceFromStartOfLaneToPoint = nextLaneEgo->GetLastPoint()->sOffset - intersectionPoint.sOffset;
        }
        distanceToPoint = distanceFromEgoToEndOfLane + distanceFromStartOfLaneToPoint;
    } else {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " | " +
                              "Distance to Intersection Point cannot be calculated ";
        throw std::logic_error(message);
    }
    return distanceToPoint;
}

const MentalInfrastructure::Lane* InfrastructureRepresentation::NextLane(IndicatorState indicatorState, bool movingInLaneDirection,
                                                                         const MentalInfrastructure::Lane* lane) {
    return InfrastructurePerception::NextLane(indicatorState, movingInLaneDirection, lane);
}

const std::optional<NextDirectionLanes> InfrastructureRepresentation::NextLanes(bool movingInLaneDirection,
                                                                                const MentalInfrastructure::Lane* currentLane) {
    return InfrastructurePerception::NextLanes(movingInLaneDirection, currentLane);
}

const RoadmapGraph::RoadmapNode* InfrastructureRepresentation::NavigateToTargetNode(std::string targetRoadOdId,
                                                                                    int64_t targetLaneOdId) const {
    return infrastructure->graph.NavigateToTargetNode(targetRoadOdId, targetLaneOdId);
}
} // namespace CognitiveMap
