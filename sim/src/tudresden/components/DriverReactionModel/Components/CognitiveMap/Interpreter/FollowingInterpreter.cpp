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

#include "common/Helper.h"
namespace Interpreter {

void FollowingInterpreter::Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) {
    try {
        for (const auto& agent : *representation.agentMemory) {
            interpretation->interpretedAgents.at(agent->GetID())->relativeDistance = CalculateFollowingDistance(*agent, representation);
        }
    } catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update followingInterpreter failed";
        Log(message, error);
        throw std::logic_error(message);

    }
};

std::optional<double> FollowingInterpreter::CalculateFollowingDistance(const AgentRepresentation& agent,
                                                                       const WorldRepresentation& representation) {
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
            return observedS - egoS - distanceReferenceToEdgesFollowing;
        }
        else {
            return observedS - egoS + distanceReferenceToEdgesLeading;
        }
    }
    // observed agent on successor lane
    if (std::any_of(egoLane->GetSuccessors().begin(), egoLane->GetSuccessors().end(),
                    [observedLane](const auto& lane) { return lane == observedLane; })) {
        return egoLane->GetLastPoint()->sOffset - egoS + observedS - observedLane->GetFirstPoint()->sOffset -
               distanceReferenceToEdgesFollowing;
    }
    // observed agent on predecessor lane
    if (std::any_of(egoLane->GetPredecessors().begin(), egoLane->GetPredecessors().end(),
                    [observedLane](const auto &lane) { return lane == observedLane; })) {
        return -(((observedLane->GetLastPoint()->sOffset - observedS) + (egoS - egoLane->GetFirstPoint()->sOffset)) -
                 distanceReferenceToEdgesLeading);
    }

    // one of observed agent predecessor lanes == successor lane of egoAgent
    auto sucLaneEgo = representation.egoAgent->GetNextLane();
    if (std::any_of(observedLane->GetPredecessors().begin(), observedLane->GetPredecessors().end(),
                    [sucLaneEgo](const auto& lane) { return lane == sucLaneEgo; })) {
        return egoLane->GetLastPoint()->sOffset - egoS + sucLaneEgo->GetLength() + observedS - observedLane->GetFirstPoint()->sOffset -
               distanceReferenceToEdgesFollowing;
    }
    // one of ego predecessor lanes == successor lane of observed
    auto sucLaneOAgent = agent.GetNextLane();
    if (std::any_of(egoLane->GetPredecessors().begin(), egoLane->GetPredecessors().end(),
                    [sucLaneOAgent](const auto &lane) { return lane == sucLaneOAgent; })) {
        return -(sucLaneOAgent->GetLength() + (observedLane->GetLastPoint()->sOffset - observedS) +
                 (egoS - egoLane->GetFirstPoint()->sOffset) - distanceReferenceToEdgesLeading);
    }

    //  agents have conflict area and same successor --> merging manoeuvre
    //  agents have conflict area and same predecessor --> splitting manoeuvre
    auto conflictAreaDistance = MergeOrSplitManoeuvreDistanceToConflictArea(representation.egoAgent, agent);
    if (conflictAreaDistance) {
        auto distanceOAgentToEndCA = conflictAreaDistance->oAgentDistance.vehicleReferenceToCAEnd;
        std::cout << "distanceOAgentToEndCA=" << distanceOAgentToEndCA << std::endl;
        if (distanceOAgentToEndCA + oAgentDistanceReferenceToBack <= 0) {
            // observed agent leaves confict area
            return std::nullopt;
        }
        auto distanceEgoToEndCA = conflictAreaDistance->egoDistance.vehicleReferenceToCAEnd;
        double followingDistance = distanceEgoToEndCA - distanceOAgentToEndCA;
        auto finalDistance = [distanceReferenceToEdgesFollowing, distanceReferenceToEdgesLeading](double followingDistance) {
            return followingDistance > 0 ? followingDistance - distanceReferenceToEdgesFollowing
                                         : followingDistance + distanceReferenceToEdgesLeading;
        };
        std::cout << " oAgent =" << agent.GetID() << " | followingDistance raw = " << followingDistance
                  << " | followingDistance final = " << finalDistance(followingDistance) << std::endl;
        if ((followingDistance >= 0 && followingDistance - distanceReferenceToEdgesFollowing < 0) ||
            (followingDistance <= 0 && followingDistance + distanceReferenceToEdgesLeading > 0)) {
            return std::nullopt;
        }
        return followingDistance > 0 ? followingDistance - distanceReferenceToEdgesFollowing
                                     : followingDistance + distanceReferenceToEdgesLeading;
    }
    return std::nullopt;
}

std::optional<ConflictSituation>
FollowingInterpreter::MergeOrSplitManoeuvreDistanceToConflictArea(const EgoAgentRepresentation *ego,
                                                                  const AgentRepresentation &observedAgent) const {
    auto egoLane = ego->GetLanePosition().lane;
    for (auto i = 0; i < maxNumberLanesExtrapolation; i++) {
        auto observedLane = observedAgent.GetLanePosition().lane;
        for (auto j = 0; j < maxNumberLanesExtrapolation; j++) {
            if (((egoLane && observedLane) && egoLane->GetConflictAreaWithLane(observedLane)) &&
                (Common::anyElementOfCollectionIsElementOfOtherCollection(egoLane->GetSuccessors(), observedLane->GetSuccessors()) ||
                 Common::anyElementOfCollectionIsElementOfOtherCollection(egoLane->GetPredecessors(), observedLane->GetPredecessors()))) {
                auto cAEgo = egoLane->GetConflictAreaWithLane(observedLane);
                auto cAObserved = observedLane->GetConflictAreaWithLane(egoLane);
                auto result = Common::DistanceToConflictArea({*cAEgo, egoLane->GetOwlId()}, {*cAObserved, observedLane->GetOwlId()}, ego,
                                                              observedAgent);
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
