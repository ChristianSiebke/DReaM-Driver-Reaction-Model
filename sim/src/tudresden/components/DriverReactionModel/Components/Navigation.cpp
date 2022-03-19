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
#include "Navigation.h"
#include "Common/Helper.h"

#include <iostream>
namespace Navigation {

Navigation::Navigation(const WorldRepresentation &worldRepresentation, const WorldInterpretation &worldInterpretation,
                       const RouteElement &routeElement, int cycleTime, StochasticsInterface *stochastics, LoggerInterface *loggerInterface,
                       const BehaviourData &behaviourData) :
    ComponentInterface(cycleTime, stochastics, loggerInterface, behaviourData),
    worldRepresentation{worldRepresentation},
    worldInterpretation{worldInterpretation},
    routeElement{routeElement} {

}

void Navigation::Update() {
    // TODO: convert RouteElement to Route
    CrossingType turningDecision;
    if (TurningDecisionAtIntersectionHaveToBeSelected()) {
        // Final Target reached or not correctly handled
        if (route.GetWaypoints().empty()) {
            // TODO: Can we exclude errors here?
            Log("Agent has passed Largetlane: The successive route is chosen at random");
            turningDecision = CrossingType::Random;
        }
        else {
            Waypoint currentWaypoint = route.GetWaypoints().front();
            auto currentPosition = worldRepresentation.egoAgent->GetLane();
            // FIXME change stoi to method
            if (currentWaypoint.GetRoadId() == currentPosition->GetRoad()->GetOpenDriveId() &&
                currentWaypoint.GetLaneId() == currentPosition->GetOwlId()) {
                route.GetWaypoints().erase(route.GetWaypoints().begin());
            }
            turningDecision = DetermineCrossingType(worldRepresentation.infrastructure->FindShortestPath(currentPosition, GetTargetNode()));
        }
        routeDecision.indicator = ConvertCrossingTypeToIndicator(turningDecision);
        directionChosen = true;
    }

    if (ResetDirectionChosen()) {
        intersectionCounter++;
        directionChosen = false;
    }

    if (ResetIndicator()) {
        routeDecision.indicator = IndicatorState::IndicatorState_Off;
    }
    auto nextLane = InfrastructureRepresentation::NextLane(worldRepresentation.egoAgent->GetIndicatorState(),
                                                           worldRepresentation.egoAgent->IsMovingInLaneDirection(),
                                                           worldRepresentation.egoAgent->GetLane());
    if (nextLane != nullptr) {
        const auto road = nextLane->GetRoad();
        routeDecision.odRoadID = road->GetOpenDriveId();
        routeDecision.odLaneID = static_cast<int>(std::stoi(nextLane->GetOpenDriveId()));
    }
    else {
        routeDecision.odRoadID = "-999";
        routeDecision.odLaneID = "-999";
    }
}

bool Navigation::TurningDecisionAtIntersectionHaveToBeSelected() const {
    return worldRepresentation.egoAgent->NextJunction() != nullptr && !directionChosen &&
           worldInterpretation.crossingInfo.phase == CrossingPhase::Deceleration_TWO;
}



CrossingType Navigation::DetermineCrossingType(std::list<const RoadmapGraph::RoadmapNode*> path) const {
    try {
        Log("Routing by targetRoad and targetLane");
        if (1 == path.size() && worldRepresentation.egoAgent->GetLane() != path.front()->GetNode()) {
            auto msg = "There exist no path to chosen TargetLane";
            Log(msg, error);
            throw std::runtime_error(msg);
        }
        if (worldRepresentation.egoAgent->GetLane() == path.front()->GetNode()) {
            path.erase(path.begin());
        }
        auto currentLane = worldRepresentation.egoAgent->GetLane();
        auto nextLane = path.front()->GetNode();

        auto currentLanePoints = currentLane->GetLanePoints();
        auto secondLast = std::prev(currentLanePoints.end(), 2);
        Common::Vector2d CurrentDirection((currentLane->GetLastPoint()->x) - (secondLast->x),
                                          (currentLane->GetLastPoint()->y) - (secondLast)->y);
        auto nextLanePoints = nextLane->GetLanePoints();
        auto pointNL = std::prev((nextLanePoints).end(), 2);
        Common::Vector2d NextLaneDirection(nextLane->GetLastPoint()->x - pointNL->x, nextLane->GetLastPoint()->y - pointNL->y);

        auto angleDeg = AngleBetween2d(CurrentDirection, NextLaneDirection) * (180 / M_PI);

        if (10 >= angleDeg || std::fabs(angleDeg - 180) < 10) {
            return CrossingType::Straight;
        } else if (10 < angleDeg && CurrentDirection.Cross(NextLaneDirection) > 0) {
            return CrossingType::Left;

        } else if (10 < angleDeg && CurrentDirection.Cross(NextLaneDirection) < 0) {
            return CrossingType::Right;
        } else {
            const std::string msg =
                static_cast<std::string>(__FILE__) + std::to_string(__LINE__) + " error during route calculation: cannot set CrossingType";
            Log(msg, error);
            throw std::runtime_error(msg);
        }
    } catch (std::runtime_error e) {
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + static_cast<std::string>(e.what());
        Log(msg, error);
        throw std::runtime_error(msg);
    } catch (...) {
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " error during route calculation";
        Log(msg, error);
        throw std::runtime_error(msg);
    }
}

const MentalInfrastructure::Lane* Navigation::GetTargetNode() const {
    Waypoint currentWaypoint = route.GetWaypoints().front();

    try {
        // TODO: corrcet LaneId
        return worldRepresentation.infrastructure->NavigateToTargetNode(currentWaypoint.GetRoadId(), currentWaypoint.GetLaneId())
            ->GetNode();
    }
    catch (std::out_of_range e) {
        auto msg = static_cast<std::string>(e.what()) + "File: " + static_cast<std::string>(__FILE__) +
                   " Line: " + std::to_string(__LINE__) + " Route target is not correct! Check whether the waypoints exist! ";
        Log(msg, error);
        throw std::out_of_range(msg);
    }
}

IndicatorState Navigation::ConvertCrossingTypeToIndicator(CrossingType turningDecision) const {
    if (turningDecision == CrossingType::Right) {
        return IndicatorState::IndicatorState_Right;
    } else if (turningDecision == CrossingType::Left) {
        return IndicatorState::IndicatorState_Left;
    } else if (turningDecision == CrossingType::Straight) {
        return IndicatorState::IndicatorState_Off;
    } else if (turningDecision == CrossingType::Random) {
        return Randomize();
    } else {
        const std::string msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                " Indicator state cannot be set: TurningDecision  is invalid";
        Log(msg, error);
        throw std::out_of_range(msg);
    }
}

bool Navigation::ResetDirectionChosen() const { return worldRepresentation.egoAgent->GetRoad()->IsOnJunction() && directionChosen; }

bool Navigation::ResetIndicator() const { return worldInterpretation.crossingInfo.phase < CrossingPhase::Deceleration_TWO; }

IndicatorState Navigation::Randomize() const {
    auto connections = InfrastructureRepresentation::NextLanes(worldRepresentation.egoAgent->IsMovingInLaneDirection(),
                                                               worldRepresentation.egoAgent->GetLane());
    int totalNumberConnections =
        static_cast<int>(connections->rightLanes.size() + connections->straightLanes.size() + connections->leftLanes.size());
    double random_number = std::rand() / static_cast<double>(RAND_MAX);

    if (random_number <= static_cast<double>(connections->rightLanes.size()) / totalNumberConnections) {
        return IndicatorState::IndicatorState_Right;
    } else if (random_number >= 1 - static_cast<double>(connections->leftLanes.size()) / totalNumberConnections) {
        return IndicatorState::IndicatorState_Left;
    } else {
        return IndicatorState::IndicatorState_Off;
    }
}
} // namespace Navigation
