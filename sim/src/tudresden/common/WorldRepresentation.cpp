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

#include <algorithm>

#include "Helper.h"
namespace CognitiveMap {

std::optional<LanePosition> AgentRepresentation::FindNewPositionInDistance(double distance) const {
    return internalData->FindNewPositionInDistance(distance);
}

bool AgentRepresentation::ObservedVehicleCameFromRight(const AgentRepresentation &oAgentPerceptionData) const {
    Common::Vector2d referenceVec;
    Common::Vector2d directionVec;

    auto laneOAgent = GetJunctionConnectionLane(oAgentPerceptionData);
    auto laneEgo = GetJunctionConnectionLane(*this);
    if (!laneOAgent || !laneEgo) {
        auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " No junction situation ";
        throw std::logic_error(msg);
        return false;
    }
    auto listOfPoints = (*laneEgo)->GetLanePoints();
    auto secondPoint = std::next(listOfPoints.begin(), 1);
    referenceVec = Common::Vector2d(secondPoint->x - (*laneEgo)->GetFirstPoint()->x, secondPoint->y - (*laneEgo)->GetFirstPoint()->y);
    directionVec = Common::Vector2d((*laneOAgent)->GetFirstPoint()->x - (*laneEgo)->GetFirstPoint()->x,
                                    (*laneOAgent)->GetFirstPoint()->y - (*laneEgo)->GetFirstPoint()->y);

    return referenceVec.Cross(directionVec) < 0 ? true : false;
}
std::optional<const MentalInfrastructure::Lane *> AgentRepresentation::GetJunctionConnectionLane(const AgentRepresentation &agent) const {
    auto laneAgent = agent.GetLanePosition().lane;
    auto successorLane = [&agent](const MentalInfrastructure::Lane *lane) {
        return lane->NextLane(agent.GetIndicatorState(), agent.IsMovingInLaneDirection());
    };
    auto predecessorLane = [](const MentalInfrastructure::Lane *lane) { return lane->GetPredecessors().front(); };
    auto nextLane = [&](auto lane) { return agent.IsMovingInLaneDirection() ? successorLane(lane) : predecessorLane(lane); };
    const MentalInfrastructure::Junction *egoJunction = nullptr;
    auto lane = laneAgent;
    for (auto i = 0; i <= maxNumberLanesExtrapolation; i++) {
        if (lane && lane->IsJunctionLane()) {
            return lane;
        }
        lane = lane ? nextLane(lane) : nullptr;
    }
    return std::nullopt;
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
