/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "FollowingInterpreter.h"

#include "Common/TimeMeasurement.hpp"
#include "common/Helper.h"

TimeMeasurement timeMeasure4("FollowingInterpreter.cpp");

namespace Interpreter {

void FollowingInterpreter::Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) {
    try {
        timeMeasure4.StartTimePoint("FollowingInterpreter ");
        for (const auto& agent : *representation.agentMemory) {
            auto state = FollowingState(*agent, representation, interpretation);

            interpretation->interpretedAgents.at(agent->GetID())->relativeDistance = state.first;
            interpretation->interpretedAgents.at(agent->GetID())->laneInLineWithEgoLane = state.second;
        }
        timeMeasure4.EndTimePoint();
    } catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update followingInterpreter failed";
        Log(message, error);
        throw std::logic_error(message);
    }
};

std::pair<std::optional<double>, bool> FollowingInterpreter::FollowingState(const AgentRepresentation &agent,
                                                                            const WorldRepresentation &representation,
                                                                            const WorldInterpretation *interpretation) {
    auto egoLane = representation.egoAgent->GetLanePosition().lane;
    double egoS = representation.egoAgent->GetLanePosition().sCoordinate;
    auto observedLane = agent.GetLanePosition().lane;
    double observedS = agent.GetLanePosition().sCoordinate;

    double oAgentDistanceReferenceToBack = agent.GetLength() - agent.GetDistanceReferencePointToLeadingEdge();
    double egoAgentDistanceReferenceToFront = representation.egoAgent->GetDistanceReferencePointToLeadingEdge();
    double oAgentDistanceReferenceToFront = agent.GetDistanceReferencePointToLeadingEdge();
    double egoAgentDistanceReferenceToBack =
        representation.egoAgent->GetLength() - representation.egoAgent->GetDistanceReferencePointToLeadingEdge();
    double distanceReferenceToEdgesFollowing = oAgentDistanceReferenceToBack + egoAgentDistanceReferenceToFront;
    double distanceReferenceToEdgesLeading = oAgentDistanceReferenceToFront + egoAgentDistanceReferenceToBack;

    // following on same lane
    if (observedLane == egoLane && egoLane != nullptr) {
        if (observedS > egoS) {
            return {{observedS - egoS - distanceReferenceToEdgesFollowing}, true};
        }
        else {
            return {{observedS - egoS + distanceReferenceToEdgesLeading}, true};
        }
    }
    // TODO: define followingThreshold  (min safety distance 2s --> 4 *min safety distance)
    double followingThreshold = representation.egoAgent->GetVelocity() * 8;
    followingThreshold = followingThreshold < 50 ? 50 : followingThreshold;
    if ((representation.egoAgent->GetRefPosition() - agent.GetRefPosition()).Length() - (followingThreshold) > 0) {
        // reduce simulaton time
        return {std::nullopt, false};
    }

    // observed agent on successor lane
    if (std::any_of(egoLane->GetSuccessors().begin(), egoLane->GetSuccessors().end(),
                    [observedLane](const auto &lane) { return lane == observedLane; })) {
        return {{egoLane->GetLastPoint()->sOffset - egoS + observedS - observedLane->GetFirstPoint()->sOffset -
                 distanceReferenceToEdgesFollowing},
                true};
    }
    // observed agent on predecessor lane
    if (std::any_of(egoLane->GetPredecessors().begin(), egoLane->GetPredecessors().end(),
                    [observedLane](const auto &lane) { return lane == observedLane; })) {
        return {{-(((observedLane->GetLastPoint()->sOffset - observedS) + (egoS - egoLane->GetFirstPoint()->sOffset)) -
                   distanceReferenceToEdgesLeading)},
                true};
    }

    // one of observed agent predecessor lanes == successor lane of egoAgent
    auto sucLaneEgo = representation.egoAgent->GetNextLane();
    if (std::any_of(observedLane->GetPredecessors().begin(), observedLane->GetPredecessors().end(),
                    [sucLaneEgo](const auto &lane) { return lane == sucLaneEgo; })) {
        return {{egoLane->GetLastPoint()->sOffset - egoS + sucLaneEgo->GetLength() + observedS - observedLane->GetFirstPoint()->sOffset -
                 distanceReferenceToEdgesFollowing},
                true};
    }
    // one of ego predecessor lanes == successor lane of observed
    auto sucLaneOAgent = agent.GetNextLane();
    if (std::any_of(egoLane->GetPredecessors().begin(), egoLane->GetPredecessors().end(),
                    [sucLaneOAgent](const auto &lane) { return lane == sucLaneOAgent; })) {
        return {{-(sucLaneOAgent->GetLength() + (observedLane->GetLastPoint()->sOffset - observedS) +
                   (egoS - egoLane->GetFirstPoint()->sOffset) - distanceReferenceToEdgesLeading)},
                true};
    }

    //  agents have conflict area and same successor --> merging manoeuvre
    //  agents have conflict area and same predecessor --> splitting manoeuvre
    auto conflictSituation = interpretation->interpretedAgents.at(agent.GetID())->conflictSituation;
    if (!conflictSituation.has_value()) {
        return {std::nullopt, false};
    }
    if (conflictSituation->junction->GetOpenDriveId() != interpretation->crossingInfo.junctionOdId) {
    }
    auto conflictAreaDistance = MergeOrSplitManoeuvreDistanceToConflictArea(*conflictSituation);
    if (conflictAreaDistance) {
        auto distanceOAgentToEndCA = conflictAreaDistance->oAgentDistance.vehicleReferenceToCAEnd;
        if (distanceOAgentToEndCA + oAgentDistanceReferenceToBack <= 0) {
            // observed agent leaves confict area
            return {std::nullopt, false};
        }
        auto distanceEgoToEndCA = conflictAreaDistance->egoDistance.vehicleReferenceToCAEnd;
        double followingDistance = distanceEgoToEndCA - distanceOAgentToEndCA;
        if ((followingDistance >= 0 && followingDistance - distanceReferenceToEdgesFollowing < 0) ||
            (followingDistance <= 0 && followingDistance + distanceReferenceToEdgesLeading > 0)) {
            return {std::nullopt, false};
        }
        if (followingDistance > 0) {
            return {{followingDistance - distanceReferenceToEdgesFollowing}, false};
        }
        else {
            return {{followingDistance + distanceReferenceToEdgesLeading}, false};
        }
    }
    return {std::nullopt, false};
}

std::optional<ConflictSituation>
FollowingInterpreter::MergeOrSplitManoeuvreDistanceToConflictArea(const ConflictSituation &conflictSituation) const {
    auto egoLane = conflictSituation.egoCA->lane;
    auto observedLane = conflictSituation.oAgentCA->lane;
    if (Common::anyElementOfCollectionIsElementOfOtherCollection(egoLane->GetSuccessors(), observedLane->GetSuccessors()) ||
        Common::anyElementOfCollectionIsElementOfOtherCollection(egoLane->GetPredecessors(), observedLane->GetPredecessors())) {
        return conflictSituation;
    }
    return std::nullopt;
}

} // namespace Interpreter
