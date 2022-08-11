/******************************************************************************
 * Copyright (c) 2019 TU Dresden
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

#include "WorldInterpreter.h"

#include "Components/CognitiveMap/CognitiveMap.h"
namespace Interpreter {
void WorldInterpreter::SetPrimaryTask(std::unique_ptr<Interpreter::InterpreterInterface> task) {
    primaryTasks.push_back(std::move(task));
}
void WorldInterpreter::SetSecondaryTask(std::unique_ptr<Interpreter::InterpreterInterface> task) {
    secondaryTasks.push_back(std::move(task));
}

void WorldInterpreter::ExecuteTasks(WorldInterpretation *interpretation, const WorldRepresentation &representation) {
    // transfer agents
    for (const auto& agent : *representation.agentMemory) {
        auto agentInterpretation = std::make_unique<AgentInterpretation>(agent.get());
        interpretation->interpretedAgents.insert(std::make_pair(agent->GetID(), std::move(agentInterpretation)));
    }
    // update interpretation
    std::for_each(primaryTasks.begin(), primaryTasks.end(),
                  [&interpretation, &representation](const auto &element) { element->Update(interpretation, representation); });
    std::for_each(secondaryTasks.begin(), secondaryTasks.end(),
                  [&interpretation, &representation](const auto &element) { element->Update(interpretation, representation); });
}

} // namespace Interpreter
