
#include "TargetLaneInterpreter.h"
namespace Interpreter {

void TargetLaneInterpreter::Update(WorldInterpretation *interpretation, const WorldRepresentation &representation) {
    interpretation->targetLane = TargetLane(representation);
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

    auto currentRoad = egoAgent->GetRoad()->GetOpenDriveId();
    auto currentSCoordinate = egoAgent->GetSCoordinate();

    if (targetWP->roadId == currentRoad && targetWP->s <= currentSCoordinate) {
        targetWP = std::next(targetWP);
    }

    if (waypoints.end() == targetWP) {
        // agent reach target
        return std::nullopt;
    }

    if (lastTimeStepWP != targetWP || egoLane != egoAgent->GetMainLocatorLane())
        path = representation.infrastructure->FindShortestPath(egoAgent->GetMainLocatorLane(), targetWP->lane);
    lastTimeStepWP = targetWP;
    egoLane = egoAgent->GetMainLocatorLane();

    // lane change needed to stay on path?
    auto laneIter = path.begin();
    std::advance(laneIter, 1);
    if (path.end() == laneIter) {
        return std::nullopt;
    }

    return (*laneIter)->GetNode();
}
} // namespace Interpreter