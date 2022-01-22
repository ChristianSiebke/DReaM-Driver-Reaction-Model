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
#include "RightOfWayInterpreter.h"
#include "Helper.h"
#include "RightBeforeLeftRegulation.h"
#include "TrafficSignRegulation.h"

namespace Interpreter {

void RightOfWayInterpreter::Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) {
    try {
        UpdateRightOfWayRegulation(representation);
        interpretation->rightOfWayMap.clear();

        for (const auto& observedAgent : *representation.agentMemory) {
            RightOfWay rightOfWay{true, true};

            auto conflictArea = representation.egoAgent->PossibleConflictAreaAlongLane(*observedAgent);
            auto intersectionSituation = representation.egoAgent->IntersectionSituation(*observedAgent);
            if (conflictArea && intersectionSituation) {
                rightOfWay = PerformRightOfWayDetermination(*observedAgent, representation);
            }

            interpretation->rightOfWayMap.insert({observedAgent->GetID(), rightOfWay});
            auto agentInterpretation = &interpretation->interpretedAgents.at(observedAgent->GetID());
            (*agentInterpretation)->rightOfWay.ego = rightOfWay.ego;
            (*agentInterpretation)->rightOfWay.observed = rightOfWay.observed;
            (*agentInterpretation)->conflictArea = conflictArea;
        }

        if (interpretation->rightOfWayMap.size() != representation.agentMemory->size()) {
            auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " rightOfWayMap does not have the same size like agents in memory ";
            throw std::logic_error(msg);
        }
    } catch (std::logic_error e) {
        std::string message = e.what();
        Log(message, error);
        throw std::logic_error(message);

    } catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update right of way failed";
        Log(message, error);
        throw std::logic_error(message);
    }
}

void RightOfWayInterpreter::UpdateRightOfWayRegulation(const WorldRepresentation& representation) {
    if (auto rowSign = representation.egoAgent->NextROWSign()) {
        rightOfWayRegulation = std::make_unique<RightOfWayRegulation::TrafficSignRegulationGermany>();
    } else {
        rightOfWayRegulation = std::make_unique<RightOfWayRegulation::RightBeforeLeftRegulationGermany>();
    }
}

RightOfWay RightOfWayInterpreter::PerformRightOfWayDetermination(const AgentRepresentation& observedAgent,
                                                                 const WorldRepresentation& representation) {
    return rightOfWayRegulation->RightOfWayDetermination(observedAgent, representation);
}

} // namespace Interpreter
