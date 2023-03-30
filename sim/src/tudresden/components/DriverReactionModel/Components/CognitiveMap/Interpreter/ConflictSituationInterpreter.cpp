/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "ConflictSituationInterpreter.h"

namespace Interpreter {

void ConflictSituationInterpreter::Update(WorldInterpretation *interpretation, const WorldRepresentation &representation) {
    try {
        for (const auto &observedAgent : *representation.agentMemory) {
            auto conflictSituation = PossibleConflictSituationAlongLane(representation.egoAgent, *observedAgent);
            auto agentInterpretation = &interpretation->interpretedAgents.at(observedAgent->GetID());
            (*agentInterpretation)->conflictSituation = conflictSituation;
        }
    }
    catch (std::logic_error e) {
        std::string message = e.what();
        Log(message, error);
        throw std::logic_error(message);
    }
    catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update conflict situation failed";
        Log(message, error);
        throw std::logic_error(message);
    }
}

std::optional<ConflictSituation>
ConflictSituationInterpreter::PossibleConflictSituationAlongLane(const EgoAgentRepresentation *ego,
                                                                 const AmbientAgentRepresentation &observedAgent) const {
    auto egoLane = ego->GetLanePosition().lane;
    unsigned int maxNumberLanesExtrapolation = 3;
    for (auto i = 0; i < maxNumberLanesExtrapolation; i++) {
        auto observedLane = observedAgent.GetLanePosition().lane;
        for (auto j = 0; j < maxNumberLanesExtrapolation; j++) {
            if ((egoLane && observedLane) && egoLane->GetConflictAreaWithLane(observedLane)) {
                auto cAEgo = egoLane->GetConflictAreaWithLane(observedLane);
                auto cAObserved = observedLane->GetConflictAreaWithLane(egoLane);
                auto result =
                    NetDistanceToConflictPoint({*cAEgo, egoLane->GetOwlId()}, {*cAObserved, observedLane->GetOwlId()}, ego, observedAgent);
                return result;
            }
            observedLane =
                observedLane ? observedLane->NextLane(observedAgent.GetIndicatorState(), observedAgent.IsMovingInLaneDirection()) : nullptr;
        }
        egoLane = egoLane ? egoLane->NextLane(ego->GetIndicatorState(), ego->IsMovingInLaneDirection()) : nullptr;
    }
    return std::nullopt;
};

ConflictSituation
ConflictSituationInterpreter::NetDistanceToConflictPoint(std::pair<const MentalInfrastructure::ConflictArea &, OwlId> egoCA,
                                                         std::pair<const MentalInfrastructure::ConflictArea &, OwlId> observedCA,
                                                         const EgoAgentRepresentation *ego,
                                                         const AmbientAgentRepresentation &observedAgent) const {
    ConflictSituation result;
    auto distanceEgoToStartCA = DistanceToConflictPoint(ego, egoCA.first.start, egoCA.second);
    auto distanceEgoToEndCA = DistanceToConflictPoint(ego, egoCA.first.end, egoCA.second);
    if (!ego->IsMovingInLaneDirection()) {
        auto temp = distanceEgoToStartCA;
        distanceEgoToStartCA = distanceEgoToEndCA;
        distanceEgoToEndCA = temp;
    }
    auto distanceObservedToStartCA = DistanceToConflictPoint(&observedAgent, observedCA.first.start, observedCA.second);
    auto distanceObservedToEndCA = DistanceToConflictPoint(&observedAgent, observedCA.first.end, observedCA.second);
    if (!observedAgent.IsMovingInLaneDirection()) {
        auto temp = distanceObservedToStartCA;
        distanceObservedToStartCA = distanceObservedToEndCA;
        distanceObservedToEndCA = temp;
    }
    result.egoDistance.vehicleFrontToCAStart = distanceEgoToStartCA - ego->GetDistanceReferencePointToLeadingEdge();
    result.egoDistance.vehicleBackToCAEnd = distanceEgoToEndCA + ego->GetLength() - ego->GetDistanceReferencePointToLeadingEdge();
    result.oAgentDistance.vehicleFrontToCAStart = distanceObservedToStartCA - observedAgent.GetDistanceReferencePointToLeadingEdge();
    result.oAgentDistance.vehicleBackToCAEnd =
        distanceObservedToEndCA + observedAgent.GetLength() - observedAgent.GetDistanceReferencePointToLeadingEdge();

    return result;
}

double ConflictSituationInterpreter::DistanceToConflictPoint(const AgentRepresentation *agent,
                                                             const MentalInfrastructure::LanePoint &conflictAreaBorder, OwlId laneId) const {
    double distanceToPoint;
    auto nextLaneEgo = agent->GetNextLane();

    if (laneId == agent->GetLanePosition().lane->GetOwlId()) {
        distanceToPoint = conflictAreaBorder.sOffset - agent->GetLanePosition().sCoordinate;
        if (!agent->IsMovingInLaneDirection()) {
            distanceToPoint = agent->GetLanePosition().sCoordinate - conflictAreaBorder.sOffset;
        }
    }
    else {
        const auto &currentLaneEgo = agent->GetLanePosition().lane;
        double distanceConnectionDistance = 0;
        while (nextLaneEgo->GetOwlId() != laneId) {
            distanceConnectionDistance += nextLaneEgo->GetLength();
            nextLaneEgo = nextLaneEgo->NextLane(agent->GetIndicatorState(), agent->IsMovingInLaneDirection());
        }

        auto distanceFromEgoToEndOfLane = currentLaneEgo->GetLastPoint()->sOffset - agent->GetLanePosition().sCoordinate;
        auto distanceFromStartOfLaneToPoint = conflictAreaBorder.sOffset - nextLaneEgo->GetFirstPoint()->sOffset;
        if (!agent->IsMovingInLaneDirection()) {
            distanceFromEgoToEndOfLane = agent->GetLanePosition().sCoordinate - currentLaneEgo->GetFirstPoint()->sOffset;
            distanceFromStartOfLaneToPoint = nextLaneEgo->GetLastPoint()->sOffset - conflictAreaBorder.sOffset;
        }
        distanceToPoint = distanceFromEgoToEndOfLane + distanceFromStartOfLaneToPoint + distanceConnectionDistance;
    }

    return distanceToPoint;
}

} // namespace Interpreter