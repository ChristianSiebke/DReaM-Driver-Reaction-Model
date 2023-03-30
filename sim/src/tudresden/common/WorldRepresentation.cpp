/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "WorldRepresentation.h"
#include "Helper.h"
#include <algorithm>
namespace CognitiveMap {

std::optional<LanePosition> AgentRepresentation::FindNewPositionInDistance(double distance) const {
    return internalData->FindNewPositionInDistance(distance);
}

bool AgentRepresentation::ObservedVehicleCameFromRight(const AgentRepresentation& oAgentPerceptionData) const {
    const auto laneEgoAgent = GetLanePosition().lane;
    const auto laneOAgent = oAgentPerceptionData.GetLanePosition().lane;
    Common::Vector2d referenceVec;
    Common::Vector2d directionVec;
    const auto egoRoad = laneEgoAgent->GetRoad();
    const auto oAgentRoad = laneOAgent->GetRoad();
    const auto& listOfPoints = laneEgoAgent->GetLanePoints();
    auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " No junction situation ";

    if (egoRoad->IsOnJunction()) {
        // Calculation of the referenceVec vector of the incoming junction road of ego
        auto secondPoint = std::next(listOfPoints.begin(), 1);
        referenceVec =
            Common::Vector2d(secondPoint->x - laneEgoAgent->GetFirstPoint()->x, secondPoint->y - laneEgoAgent->GetFirstPoint()->y);

        // Calculation of the direction vector to the examination point
        if (oAgentRoad->IsOnJunction()) {
            directionVec = Common::Vector2d(laneOAgent->GetFirstPoint()->x - laneEgoAgent->GetFirstPoint()->x,
                                            laneOAgent->GetFirstPoint()->y - laneEgoAgent->GetFirstPoint()->y);
        }
        else if (oAgentRoad->IsSuccessorJunction() || oAgentRoad->IsPredecessorJunction()) {
            directionVec = Common::Vector2d(laneOAgent->GetLastPoint()->x - laneEgoAgent->GetFirstPoint()->x,
                                            laneOAgent->GetLastPoint()->y - laneEgoAgent->GetFirstPoint()->y);
        }
        else {
            throw std::logic_error(msg);
        }
    }
    else if (egoRoad->IsSuccessorJunction() || egoRoad->IsPredecessorJunction()) {
        // Calculation of the referenceVec vector of the incoming junction road of ego
        auto secondLastPoint = std::prev(listOfPoints.end(), 2);
        referenceVec =
            Common::Vector2d(laneEgoAgent->GetLastPoint()->x - secondLastPoint->x, laneEgoAgent->GetLastPoint()->y - secondLastPoint->y);

        // Calculation of the direction vector to the examination point
        if (oAgentRoad->IsOnJunction()) {
            directionVec = Common::Vector2d(laneOAgent->GetFirstPoint()->x - laneEgoAgent->GetLastPoint()->x,
                                            laneOAgent->GetFirstPoint()->y - laneEgoAgent->GetLastPoint()->y);
        }
        else if (oAgentRoad->IsSuccessorJunction() || oAgentRoad->IsPredecessorJunction()) {
            directionVec = Common::Vector2d(laneOAgent->GetLastPoint()->x - laneEgoAgent->GetLastPoint()->x,
                                            laneOAgent->GetLastPoint()->y - laneEgoAgent->GetLastPoint()->y);
        }
        else {
            throw std::logic_error(msg);
        }
    }
    else {
        throw std::logic_error(msg);
    }
    return referenceVec.Cross(directionVec) < 0 ? true : false;
}

const MentalInfrastructure::Junction *AgentRepresentation::NextJunction() const {
    const auto &currentLane = GetLanePosition().lane;
    const auto &currentRoad = GetLanePosition().lane->GetRoad();
    const MentalInfrastructure::Junction *nextJunction = nullptr;

    if (currentLane->IsInRoadDirection()) {
        if (IsMovingInLaneDirection()) {
            if (currentRoad->IsSuccessorJunction()) {
                nextJunction = dynamic_cast<const MentalInfrastructure::Junction *>(currentRoad->GetSuccessor());
            }
        } else {
            if (currentRoad->IsPredecessorJunction()) {
                nextJunction = dynamic_cast<const MentalInfrastructure::Junction *>(currentRoad->GetPredecessor());
            }
        }
    } else {
        if (IsMovingInLaneDirection()) {
            if (currentRoad->IsPredecessorJunction()) {
                nextJunction = dynamic_cast<const MentalInfrastructure::Junction *>(currentRoad->GetPredecessor());
            }
        } else {
            if (currentRoad->IsSuccessorJunction()) {
                nextJunction = dynamic_cast<const MentalInfrastructure::Junction *>(currentRoad->GetSuccessor());
            }
        }
    }
    return nextJunction;
}

std::optional<MentalInfrastructure::TrafficSign> AgentRepresentation::NextROWSign() const {
    const auto &egoLane = GetLanePosition().lane;
    const auto &egoRoad = GetLanePosition().lane->GetRoad();
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

const RoadmapGraph::RoadmapNode *InfrastructureRepresentation::NavigateToTargetNode(OdId targetRoadOdId, OwlId targetLaneOdId) const {
    return infrastructure->graph.NavigateToTargetNode(targetRoadOdId, targetLaneOdId);
}
} // namespace CognitiveMap
