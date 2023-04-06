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

#include "Helper.h"

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
    for (auto i = 0; i < maxNumberLanesExtrapolation; i++) {
        if (!egoLane)
            break;
        auto observedLane = observedAgent.GetLanePosition().lane;
        for (auto j = 0; j < maxNumberLanesExtrapolation; j++) {
            if (!observedLane)
                break;
            if (auto cAEgo = egoLane->GetConflictAreaWithLane(observedLane)) {
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
};

} // namespace Interpreter