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

#include <iostream>

#include "Common/Helper.h"
namespace Navigation {

Navigation::Navigation(const WorldRepresentation &worldRepresentation, const WorldInterpretation &worldInterpretation, int cycleTime,
                       StochasticsInterface *stochastics, LoggerInterface *loggerInterface, const BehaviourData &behaviourData) :
    ComponentInterface(cycleTime, stochastics, loggerInterface, behaviourData),
    worldRepresentation{worldRepresentation},
    worldInterpretation{worldInterpretation} {
}

void Navigation::Update() {
    auto egoAgent = worldRepresentation.egoAgent;

    if (waypoints != egoAgent->GetRoute()) {
        waypoints = egoAgent->GetRoute();
        targetWP = waypoints.begin();
    }

    if (waypoints.empty()) {
        std::string msg = __FILE__ " " + std::to_string(__LINE__) + " Agent has no waypoints";
        Log(msg, DReaMLogLevel::error);
        throw std::logic_error(msg);
    }

    auto currentRoad = egoAgent->GetRoad()->GetOpenDriveId();
    auto currentSCoordinate = egoAgent->GetSCoordinate();

    if (targetWP->roadId == currentRoad && targetWP->s <= currentSCoordinate) {
        targetWP = std::next(targetWP);
    }

    if (waypoints.end() == targetWP) {
        // agent reach target
        return;
    }

    if (lastTimeStepWP != targetWP || egoLane != egoAgent->GetMainLocatorLane())
        path = worldRepresentation.infrastructure->FindShortestPath(egoAgent->GetMainLocatorLane(), targetWP->lane);
    lastTimeStepWP = targetWP;
    egoLane = egoAgent->GetMainLocatorLane();

    // lane change needed to stay on path?
    auto laneIter = path.begin();
    std::advance(laneIter, 1);
    if (path.end() == laneIter) {
        routeDecision.lateralDisplacement = 0;
        if (AgentIsTurningOnJunction()) {
            return;
        }
        routeDecision.indicator = IndicatorState::IndicatorState_Off;
        return;
    }

    auto targetLane = (*laneIter)->GetNode();
    if (std::none_of(egoAgent->GetMainLocatorLane()->GetSuccessors().begin(), egoAgent->GetMainLocatorLane()->GetSuccessors().end(),
                     [targetLane](auto element) { return element == targetLane; })) {
        // New Lane free ?
        // TODO: implement Algorithm -->new lane free?
        // TODO: move to interpreter
        if (NewLaneIsFree()) {
            if (targetLane == egoAgent->GetMainLocatorLane()->GetLeftLane()) {
                routeDecision.lateralDisplacement = ((egoAgent->GetMainLocatorLane()->GetWidth() / 2) + (targetLane->GetWidth() / 2));
                routeDecision.indicator = IndicatorState::IndicatorState_Left;
            }
            else if (targetLane == egoAgent->GetMainLocatorLane()->GetRightLane()) {
                routeDecision.lateralDisplacement = -((egoAgent->GetMainLocatorLane()->GetWidth() / 2) + (targetLane->GetWidth() / 2));
                routeDecision.indicator = IndicatorState::IndicatorState_Right;
            }
            else {
                const std::string msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                        " next lane in path is not neighbor";
                Log(msg, error);
                throw std::logic_error(msg);
            }
        }
        else {
            // TODO: slow down and wait at end of road/ after waiting a specific time choose new lane
            // TODO acceleration calcualtion in longitudinalAction (ActionDecision)
            // TODO: move to interpreter
        }
    }
    else {
        routeDecision.lateralDisplacement = 0;
        if (TurningAtJunction()) {
            routeDecision.indicator = SetIndicatorAtJunction(path);
            return;
        }
        if (AgentIsTurningOnJunction()) {
            return;
        }
        routeDecision.indicator = IndicatorState::IndicatorState_Off;
    }
}
bool Navigation::NewLaneIsFree() const {
    // TODO: implement
    return true;
}
bool Navigation::TurningAtJunction() const {
    return worldRepresentation.egoAgent->NextJunction() != nullptr &&
           worldInterpretation.crossingInfo.phase == CrossingPhase::Deceleration_TWO;
}
bool Navigation::AgentIsTurningOnJunction() const {
    return worldInterpretation.crossingInfo.phase >= CrossingPhase::Deceleration_TWO;
}

IndicatorState Navigation::SetIndicatorAtJunction(std::list<const RoadmapGraph::RoadmapNode *> path) const {
    try {
        std::list<const RoadmapGraph::RoadmapNode *>::iterator laneIter = path.begin();
        auto currentLane = (*laneIter)->GetNode();
        std::advance(laneIter, 1);

        auto nextLane = (*laneIter)->GetNode();
        auto currentLanePoints = currentLane->GetLanePoints();
        auto secondLast = std::prev(currentLanePoints.end(), 2);
        Common::Vector2d CurrentDirection((currentLane->GetLastPoint()->x) - (secondLast->x),
                                          (currentLane->GetLastPoint()->y) - (secondLast)->y);
        auto nextLanePoints = nextLane->GetLanePoints();
        auto pointNL = std::prev((nextLanePoints).end(), 2);
        Common::Vector2d NextLaneDirection(nextLane->GetLastPoint()->x - pointNL->x, nextLane->GetLastPoint()->y - pointNL->y);

        auto angleDeg = AngleBetween2d(CurrentDirection, NextLaneDirection) * (180 / M_PI);

        if (10 >= angleDeg || std::fabs(angleDeg - 180) < 10) {
            return IndicatorState::IndicatorState_Off;
        }
        else if (10 < angleDeg && CurrentDirection.Cross(NextLaneDirection) > 0) {
            return IndicatorState::IndicatorState_Left;
        }
        else if (10 < angleDeg && CurrentDirection.Cross(NextLaneDirection) < 0) {
            return IndicatorState::IndicatorState_Right;
        }
        else {
            const std::string msg =
                static_cast<std::string>(__FILE__) + std::to_string(__LINE__) + " error during route calculation: cannot set CrossingType";
            Log(msg, error);
            throw std::runtime_error(msg);
        }
    }
    catch (std::runtime_error e) {
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + static_cast<std::string>(e.what());
        Log(msg, error);
        throw std::runtime_error(msg);
    }
    catch (...) {
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " error during route calculation";
        Log(msg, error);
        throw std::runtime_error(msg);
    }
}

} // namespace Navigation
