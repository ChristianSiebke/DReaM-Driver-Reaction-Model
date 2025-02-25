/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "TargetLaneInterpreter.h"

namespace Interpreter {

void TargetLaneInterpreter::Update(WorldInterpretation *interpretation, const WorldRepresentation &representation) {
    try {
        interpretation->targetLane = TargetLane(representation);
    }
    catch (std::logic_error &e) {
        const std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " " + e.what();
        Log(message, error);
        throw std::logic_error(message);
    }
    catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update targetLaneInterpreter failed";
        Log(message, error);
        throw std::logic_error(message);
    }
}

std::optional<const MentalInfrastructure::Lane *> TargetLaneInterpreter::TargetLane(const WorldRepresentation &representation) {
    auto egoAgent = representation.egoAgent;

    if (waypoints != egoAgent->GetRoute()) {
        waypoints = egoAgent->GetRoute();
        targetWP = waypoints.begin();
    }

    if (waypoints.empty()) {
        std::string msg = __FILE__ " " + std::to_string(__LINE__) + " Agent has no route";
        Log(msg, DReaMLogLevel::error);
        throw std::logic_error(msg);
    }

    auto currentRoad = egoAgent->GetLanePosition().lane->GetRoad()->GetOpenDriveId();
    auto currentSCoordinate = egoAgent->GetLanePosition().sCoordinate;
    if (waypoints.end() == targetWP) {
        // agent reach target
        return std::nullopt;
    }
    if ((targetWP->roadId == currentRoad && targetWP->lane == egoAgent->GetLanePosition().lane) && targetWP->s <= currentSCoordinate) {
        targetWP = std::next(targetWP);
    }
    else if (targetWP->roadId == currentRoad && targetWP->lane == egoAgent->GetLanePosition().lane) {
        return targetWP->lane;
    }

    if (waypoints.end() == targetWP) {
        // agent reach target
        return std::nullopt;
    }

    if (lastTimeStepWP != targetWP || egoLane != egoAgent->GetMainLocatorLane()) {
        path = representation.infrastructure->FindShortestPath(egoAgent->GetMainLocatorLane(), targetWP->lane);
    }
    auto nextWP = std::next(targetWP);
    if (waypoints.end() != nextWP &&
        (egoAgent->GetMainLocatorLane() == nextWP->lane && egoAgent->GetMainLocatorLane()->GetRoad()->GetOpenDriveId() == nextWP->roadId)) {
        targetWP = std::next(targetWP);
    }
    lastTimeStepWP = targetWP;
    egoLane = egoAgent->GetMainLocatorLane();

    assert(!path.empty());

    auto laneIter = path.begin();
    std::advance(laneIter, 1);
    if (path.end() == laneIter) {
        return std::nullopt;
    }

    return (*laneIter)->GetNode();
}
} // namespace Interpreter
