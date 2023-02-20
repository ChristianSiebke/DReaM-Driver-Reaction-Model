/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "PerceptionData.h"

#include "Definitions.h"
#include "Helper.h"

std::optional<LanePosition> GeneralAgentPerception::FindNewPositionInDistance(double distance) const {
    // Lane is outside of infrastructure perception
    if (!lanePosition.lane) {
        return std::nullopt;
    }
    auto newLane = lanePosition.lane;
    double newSCoordinate;
    const MentalInfrastructure::Lane *nextLaneInternal;

    bool movingInLaneDirection = distance >= 0 ? true : false;
    if (distance >= 0) {
        newSCoordinate = (lanePosition.sCoordinate + distance);
        nextLaneInternal = nextLane;

        double deltaS = newSCoordinate - (newLane->GetLastPoint())->sOffset;
        // agent exceeds oldlane
        while (deltaS > 0) {
            if (nextLaneInternal == nullptr) {
                return std::nullopt;
            }
            newLane = nextLaneInternal;
            nextLaneInternal = InfrastructurePerception::NextLane(indicatorState, movingInLaneDirection, newLane);
            newSCoordinate = (newLane->GetFirstPoint()->sOffset + deltaS);
            deltaS = newSCoordinate - (newLane->GetLastPoint())->sOffset;
        }
    }
    else {
        double deltaS = (lanePosition.sCoordinate - newLane->GetFirstPoint()->sOffset) + distance;
        newSCoordinate = deltaS >= 0 ? deltaS + newLane->GetFirstPoint()->sOffset : deltaS;
        nextLaneInternal = InfrastructurePerception::NextLane(indicatorState, movingInLaneDirection, newLane);
        // agent exceeds oldlane
        while (newSCoordinate < 0) {
            if (nextLaneInternal == nullptr) {
                return std::nullopt;
            }
            newLane = nextLaneInternal;
            nextLaneInternal = InfrastructurePerception::NextLane(indicatorState, movingInLaneDirection, newLane);
            deltaS = (newLane->GetLastPoint()->sOffset - newLane->GetFirstPoint()->sOffset) + newSCoordinate;
            newSCoordinate = deltaS >= 0 ? deltaS + newLane->GetFirstPoint()->sOffset : deltaS;
        }
    }
    LanePosition position{newLane, newSCoordinate};
    return position;
}

bool GeneralAgentPerception::IsMovingInLaneDirection(const MentalInfrastructure::Lane *agentLane, double yawAngle, double sCoordinate,
                                                     double velocity) {
    auto pointHDG = agentLane->InterpolatePoint(sCoordinate).hdg;
    auto normRad = std::fabs(std::fmod(yawAngle - pointHDG, (2 * M_PI)));
    auto absDiffDeg = std::min((2 * M_PI) - normRad, normRad);
    bool direction = M_PI_2 >= absDiffDeg ? true : false;
    return velocity >= 0 ? direction : !direction;
}

JunctionDistance GeneralAgentPerception::CalculateJunctionDistance(GeneralAgentPerception perception,
                                                                   const MentalInfrastructure::Road *agentRoad,
                                                                   const MentalInfrastructure::Lane *agentLane) {
    JunctionDistance distance;

    if (agentRoad->IsOnJunction() && (agentRoad->IsPredecessorJunction() || agentRoad->IsSuccessorJunction())) {
        throw std::logic_error(__FILE__ " Line: " + std::to_string(__LINE__) + " Junctions must be connected via roads ");
    }

    if (perception.movingInLaneDirection) {
        if (agentRoad->IsOnJunction()) {
            distance.on = perception.lanePosition.sCoordinate;
        }
        if (agentLane->IsInRoadDirection()) {
            if (agentRoad->IsSuccessorJunction()) {
                distance.toNext = agentRoad->GetLength() - perception.lanePosition.sCoordinate;
            }
        }
        else {
            if (agentRoad->IsPredecessorJunction()) {
                distance.toNext = agentRoad->GetLength() - perception.lanePosition.sCoordinate;
            }
        }
    }
    else {
        if (agentRoad->IsOnJunction()) {
            distance.on = agentRoad->GetLength() - perception.lanePosition.sCoordinate;
        }
        if (agentLane->IsInRoadDirection()) {
            if (agentRoad->IsPredecessorJunction()) {
                distance.toNext = perception.lanePosition.sCoordinate;
            }
        }
        else {
            if (agentRoad->IsSuccessorJunction()) {
                distance.toNext = perception.lanePosition.sCoordinate;
            }
        }
    }
    return distance;
}

const MentalInfrastructure::Lane *InfrastructurePerception::NextLane(IndicatorState indicatorState, bool movingInLaneDirection,
                                                                     const MentalInfrastructure::Lane *currentLane) {
    try {
        auto nextLanePointers = currentLane->GetSuccessors();
        if (!movingInLaneDirection) {
            nextLanePointers = currentLane->GetPredecessors();
        }

        if (nextLanePointers.size() == 1) {
            return nextLanePointers.front();
        }
        // return FIRST lane pointing in indicator direction
        if (auto nextLanes = InfrastructurePerception::NextLanes(movingInLaneDirection, currentLane)) {
            if (!nextLanes->straightLanes.empty() &&
                (indicatorState == IndicatorState::IndicatorState_Off || indicatorState == IndicatorState::IndicatorState_Warn)) {
                return nextLanes->straightLanes.front();
            }
            else if (!nextLanes->leftLanes.empty() && indicatorState == IndicatorState::IndicatorState_Left) {
                return nextLanes->leftLanes.front();
            }
            else if (!nextLanes->rightLanes.empty() && indicatorState == IndicatorState::IndicatorState_Right) {
                return nextLanes->rightLanes.front();
            }
            else {
                // the state of the indicator does not match to the directions of the
                // successor lanes (possibly the indicator is not yet set)
                return nullptr;
            }
        }
        else {
            // no next lane exist or next lane is invalide
            return nullptr;
        }
    }
    catch (...) {
        throw std::runtime_error(__FILE__ " Line: " + std::to_string(__LINE__) + " successorLane can not be determined ");
    }
}

std::optional<NextDirectionLanes> InfrastructurePerception::NextLanes(bool movingInLaneDirection,
                                                                      const MentalInfrastructure::Lane *currentLane) {
    NextDirectionLanes nextLanes;
    auto nextLanePointers = currentLane->GetSuccessors();

    if (!movingInLaneDirection) {
        nextLanePointers = currentLane->GetPredecessors();
    }
    if (nextLanePointers.empty()) {
        return std::nullopt;
    }
    if (nextLanePointers.size() == 1) {
        nextLanes.straightLanes.push_back(nextLanePointers.front());
        return std::move(nextLanes);
    }

    // calculate direction vector of current lane at its end
    auto pointsCurrentLane = currentLane->GetLanePoints();
    auto pointCL = std::prev((pointsCurrentLane).end(), 2);
    Common::Vector2d currentDirection(currentLane->GetLastPoint()->x - pointCL->x, currentLane->GetLastPoint()->y - pointCL->y);
    if (!movingInLaneDirection) {
        pointCL = std::next((pointsCurrentLane).begin(), 1);
        currentDirection = {currentLane->GetFirstPoint()->x - pointCL->x, currentLane->GetFirstPoint()->y - pointCL->y};
    }

    for (auto nextLane : nextLanePointers) {
        auto pointsNextLane = nextLane->GetLanePoints();
        // calculate direction vector of successor lane
        auto pointNL = std::prev(pointsNextLane.end(), 2);
        Common::Vector2d successorDirection(nextLane->GetLastPoint()->x - pointNL->x, nextLane->GetLastPoint()->y - pointNL->y);
        if (!movingInLaneDirection) {
            pointNL = std::next(pointsNextLane.begin(), 1);
            successorDirection = {nextLane->GetFirstPoint()->x - pointNL->x, nextLane->GetFirstPoint()->y - pointNL->y};
        }

        auto angleDeg = AngleBetween2d(currentDirection, successorDirection) * (180 / M_PI);
        if (parallelEpsilonDeg >= angleDeg || parallelEpsilonDeg >= std::fabs(180 - angleDeg)) {
            nextLanes.straightLanes.push_back(nextLane);
        }
        else if (parallelEpsilonDeg < angleDeg && currentDirection.Cross(successorDirection) > 0) {
            nextLanes.leftLanes.push_back(nextLane);
        }
        else if (parallelEpsilonDeg < angleDeg && currentDirection.Cross(successorDirection) < 0) {
            nextLanes.rightLanes.push_back(nextLane);
        }
    }
    return std::move(nextLanes);
}
