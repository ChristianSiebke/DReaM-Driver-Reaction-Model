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
    auto egoLane = ego->GetLane();
    auto egoNextLane = ego->GetNextLane();
    auto observedLane = observedAgent.GetLane();
    auto observedNextLane = observedAgent.GetNextLane();

    if (egoLane->GetConflictAreaWithLane(observedLane) && observedLane->GetConflictAreaWithLane(egoLane)) {
        auto cAEgo = egoLane->GetConflictAreaWithLane(observedLane);
        auto cAObserved = observedLane->GetConflictAreaWithLane(egoLane);
        return CalculateConflictSituation({*cAEgo, egoLane->GetOwlId()}, {*cAObserved, observedLane->GetOwlId()}, ego, observedAgent);
    }
    else if (egoNextLane && egoNextLane->GetConflictAreaWithLane(observedLane)) {
        auto cAEgo = egoNextLane->GetConflictAreaWithLane(observedLane);
        auto cAObserved = observedLane->GetConflictAreaWithLane(egoNextLane);
        return CalculateConflictSituation({*cAEgo, egoNextLane->GetOwlId()}, {*cAObserved, observedLane->GetOwlId()}, ego, observedAgent);
    }
    else if (observedNextLane && egoLane->GetConflictAreaWithLane(observedNextLane)) {
        auto cAEgo = egoLane->GetConflictAreaWithLane(observedNextLane);
        auto cAObserved = observedNextLane->GetConflictAreaWithLane(egoLane);
        return CalculateConflictSituation({*cAEgo, egoLane->GetOwlId()}, {*cAObserved, observedNextLane->GetOwlId()}, ego, observedAgent);
    }
    else if ((egoNextLane && observedNextLane) && egoNextLane->GetConflictAreaWithLane(observedNextLane)) {
        auto cAEgo = egoNextLane->GetConflictAreaWithLane(observedNextLane);
        auto cAObserved = observedNextLane->GetConflictAreaWithLane(egoNextLane);
        return CalculateConflictSituation({*cAEgo, egoNextLane->GetOwlId()}, {*cAObserved, observedNextLane->GetOwlId()}, ego,
                                          observedAgent);
    }

    else {
        return std::nullopt;
    }
};

ConflictSituation
ConflictSituationInterpreter::CalculateConflictSituation(std::pair<const MentalInfrastructure::ConflictArea &, OwlId> egoCA,
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
                                                             const MentalInfrastructure::LanePoint &junctionPoint, OwlId laneId) const {
    double distanceToPoint;
    if (laneId == agent->GetLane()->GetOwlId()) {
        distanceToPoint = junctionPoint.sOffset - agent->GetSCoordinate();
        if (!agent->IsMovingInLaneDirection()) {
            distanceToPoint = agent->GetSCoordinate() - junctionPoint.sOffset;
        }
    }
    else if (laneId == agent->GetNextLane()->GetOwlId()) {
        const auto &currentLaneEgo = agent->GetLane();
        const auto &nextLaneEgo = agent->GetNextLane();
        auto distanceFromEgoToEndOfLane = currentLaneEgo->GetLastPoint()->sOffset - agent->GetSCoordinate();
        auto distanceFromStartOfLaneToPoint = junctionPoint.sOffset - nextLaneEgo->GetFirstPoint()->sOffset;
        if (!agent->IsMovingInLaneDirection()) {
            distanceFromEgoToEndOfLane = agent->GetSCoordinate() - currentLaneEgo->GetFirstPoint()->sOffset;
            distanceFromStartOfLaneToPoint = nextLaneEgo->GetLastPoint()->sOffset - junctionPoint.sOffset;
        }
        distanceToPoint = distanceFromEgoToEndOfLane + distanceFromStartOfLaneToPoint;
    }
    else {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " | " +
                              "Distance to Junction Point cannot be calculated ";
        throw std::logic_error(message);
    }
    return distanceToPoint;
}

} // namespace Interpreter