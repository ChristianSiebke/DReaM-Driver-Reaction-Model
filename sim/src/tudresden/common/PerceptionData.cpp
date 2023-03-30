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
            nextLaneInternal = newLane->NextLane(indicatorState, movingInLaneDirection);
            newSCoordinate = (newLane->GetFirstPoint()->sOffset + deltaS);
            deltaS = newSCoordinate - (newLane->GetLastPoint())->sOffset;
        }
    }
    else {
        double deltaS = (lanePosition.sCoordinate - newLane->GetFirstPoint()->sOffset) + distance;
        newSCoordinate = deltaS >= 0 ? deltaS + newLane->GetFirstPoint()->sOffset : deltaS;
        nextLaneInternal = newLane->NextLane(indicatorState, movingInLaneDirection);
        // agent exceeds oldlane
        while (newSCoordinate < 0) {
            if (nextLaneInternal == nullptr) {
                return std::nullopt;
            }
            newLane = nextLaneInternal;
            nextLaneInternal = newLane->NextLane(indicatorState, movingInLaneDirection);
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
