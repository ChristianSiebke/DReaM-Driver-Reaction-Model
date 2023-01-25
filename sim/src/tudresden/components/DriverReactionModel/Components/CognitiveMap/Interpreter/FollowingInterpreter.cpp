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
            interpretation->interpretedAgents.at(agent->GetID())->followingDistanceToLeadingVehicle =
                CalculateFollowingDistance(*agent, representation);
        }
    } catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update right of way failed";
        Log(message, error);
    }
};

std::optional<double> FollowingInterpreter::CalculateFollowingDistance(const AgentRepresentation& agent,
                                                                       const WorldRepresentation& representation) {
    auto egoLane = representation.egoAgent->GetLanePosition().lane;
    double egoS = representation.egoAgent->GetLanePosition().sCoordinate;
    auto observedLane = agent.GetLanePosition().lane;
    double observedS = agent.GetLanePosition().sCoordinate;

    std::cout << " !!!!!!!!!!!!!!!sCoordinate: " << egoS << std::endl;

    double oAgentDistanceBackToReference = agent.GetLength() - agent.GetDistanceReferencePointToLeadingEdge();
    double egoAgentDistanceReferenceToFront = representation.egoAgent->GetDistanceReferencePointToLeadingEdge();
    double distanceReferenceToEdges = oAgentDistanceBackToReference + egoAgentDistanceReferenceToFront;

    // following on same lane
    if (observedLane == egoLane && egoLane != nullptr) {
        double diff = observedS - egoS - distanceReferenceToEdges;
        if (observedS > egoS) {
            return diff;
        } else {
            return std::nullopt;
        }
    }
    // leading agent on successor lane
    if (std::any_of(egoLane->GetSuccessors().begin(), egoLane->GetSuccessors().end(),
                    [observedLane](const auto& lane) { return lane == observedLane; })) {
        return egoLane->GetLastPoint()->sOffset - egoS + observedS - observedLane->GetFirstPoint()->sOffset - distanceReferenceToEdges;
    }

    // one of leading agent predecessor lanes == successor lane of egoAgent
    auto sucLaneEgo = representation.egoAgent->GetNextLane();
    if (std::any_of(observedLane->GetPredecessors().begin(), observedLane->GetPredecessors().end(),
                    [sucLaneEgo](const auto& lane) { return lane == sucLaneEgo; })) {
        return egoLane->GetLastPoint()->sOffset - egoS + sucLaneEgo->GetLength() + observedS - observedLane->GetFirstPoint()->sOffset -
               distanceReferenceToEdges;
    }

    // agents have conflict area and same successor --> merging manoeuvre
    auto conflictAreaOL = observedLane->GetConflictAreaWithLane(egoLane);
    auto conflictAreaEL = egoLane->GetConflictAreaWithLane(observedLane);
    egoS = conflictAreaEL ? egoS : -(egoLane->GetLength() - egoS);
    auto nextEgoLane = representation.egoAgent->GetNextLane();
    conflictAreaOL = conflictAreaOL ? conflictAreaOL : observedLane->GetConflictAreaWithLane(nextEgoLane);
    if (nextEgoLane)
        conflictAreaEL = conflictAreaEL ? conflictAreaEL : nextEgoLane->GetConflictAreaWithLane(observedLane);

    if (conflictAreaOL &&
        (Common::anyElementOfCollectionIsElementOfOtherCollection(egoLane->GetSuccessors(), observedLane->GetSuccessors()) ||
         Common::anyElementOfCollectionIsElementOfOtherCollection(representation.egoAgent->GetNextLane()->GetSuccessors(),
                                                                  observedLane->GetSuccessors()))) {
        auto distanceOAgentToEndCA = conflictAreaOL->end.sOffset - observedS;
        auto distanceEgoToEndCA = conflictAreaEL->end.sOffset - egoS;
        double followingDistance = distanceEgoToEndCA - distanceOAgentToEndCA - distanceReferenceToEdges;
        return followingDistance > 0 ? std::optional<double>(followingDistance) : std::nullopt;
    }

    return std::nullopt;
}
} // namespace Interpreter
