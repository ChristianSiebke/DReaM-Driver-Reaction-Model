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
    // TODO: define followingThreshold  (min safety distance 2s --> 4 *min safety distance)
    double followingThreshold = representation.egoAgent->GetVelocity() * 8;
    followingThreshold = followingThreshold < 30 ? 30 : followingThreshold;

    if ((representation.egoAgent->GetRefPosition() - agent.GetRefPosition()).Length() - (followingThreshold) > 0) {
        return {std::nullopt, false};
    }
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
    if (interpretation->interpretedAgents.at(agent.GetID())->conflictSituation.has_value() &&
        (interpretation->interpretedAgents.at(agent.GetID())->conflictSituation->junction->GetOpenDriveId() !=
         interpretation->crossingInfo.junctionOdId)) {
        return {std::nullopt, false};
    }
    auto conflictAreaDistance = MergeOrSplitManoeuvreDistanceToConflictArea(representation.egoAgent, agent);
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
FollowingInterpreter::MergeOrSplitManoeuvreDistanceToConflictArea(const EgoAgentRepresentation *ego,
                                                                  const AgentRepresentation &observedAgent) const {
    auto egoLane = ego->GetLanePosition().lane;
    for (auto i = 0; i < maxNumberLanesExtrapolation; i++) {
        if (!egoLane)
            break;
        auto observedLane = observedAgent.GetLanePosition().lane;
        for (auto j = 0; j < maxNumberLanesExtrapolation; j++) {
            if (!observedLane)
                break;
            auto cAEgo = egoLane->GetConflictAreaWithLane(observedLane);
            if (cAEgo &&
                (Common::anyElementOfCollectionIsElementOfOtherCollection(egoLane->GetSuccessors(), observedLane->GetSuccessors()) ||
                 Common::anyElementOfCollectionIsElementOfOtherCollection(egoLane->GetPredecessors(), observedLane->GetPredecessors()))) {
                auto cAObserved = observedLane->GetConflictAreaWithLane(egoLane);
                auto result = Common::DistanceToConflictArea({*cAEgo, egoLane}, {*cAObserved, observedLane}, ego, observedAgent);
                return result;
            }
            observedLane =
                observedLane ? observedLane->NextLane(observedAgent.GetIndicatorState(), observedAgent.IsMovingInLaneDirection()) : nullptr;
        }
        egoLane = egoLane ? egoLane->NextLane(ego->GetIndicatorState(), ego->IsMovingInLaneDirection()) : nullptr;
    }
    return std::nullopt;
}

} // namespace Interpreter
