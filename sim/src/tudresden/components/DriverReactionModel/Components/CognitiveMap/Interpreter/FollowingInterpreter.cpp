/******************************************************************************
 * Copyright (c) 2020 TU Dresden
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
    auto egoLane = representation.egoAgent->GetLane();
    double egoS = representation.egoAgent->GetSCoordinate();
    auto observedLane = agent.GetLane();
    double observedS = agent.GetSCoordinate();

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
    if (conflictAreaOL &&
        Common::anyElementOfCollectionIsElementOfOtherCollection(egoLane->GetSuccessors(), observedLane->GetSuccessors())) {
        auto distanceOAgentToEndCA = conflictAreaOL->end.sOffset - observedS;
        auto conflictAreaEL = egoLane->GetConflictAreaWithLane(observedLane);
        auto distanceEgoToEndCA = conflictAreaEL->end.sOffset - egoS;
        double followingDistance = distanceEgoToEndCA - distanceOAgentToEndCA - distanceReferenceToEdges;
        return followingDistance > 0 ? std::optional<double>(followingDistance) : std::nullopt;
    }

    return std::nullopt;
}
} // namespace Interpreter
