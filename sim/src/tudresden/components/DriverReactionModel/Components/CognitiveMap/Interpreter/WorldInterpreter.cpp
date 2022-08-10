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
void WorldInterpreter::SetPrimaryCommand(std::unique_ptr<CognitiveMap::CommandInterface> command) {
    primaryCommands.push_back(std::move(command));
}
void WorldInterpreter::SetSecondaryCommand(std::unique_ptr<CognitiveMap::CommandInterface> command) {
    secondaryCommands.push_back(std::move(command));
}

void WorldInterpreter::ExecuteCommands(WorldInterpretation* interpretation, const WorldRepresentation& representation) {

    // transfer agents
    for (const auto& agent : *representation.agentMemory) {
        auto agentInterpretation = std::make_unique<AgentInterpretation>(agent.get());
        interpretation->interpretedAgents.insert(std::make_pair(agent->GetID(), std::move(agentInterpretation)));
    }
    // update interpretation
    std::for_each(primaryCommands.begin(), primaryCommands.end(),
                  [&interpretation, &representation](const auto &element) { element->Execute(interpretation, representation); });
    std::for_each(secondaryCommands.begin(), secondaryCommands.end(),
                  [&interpretation, &representation](const auto &element) { element->Execute(interpretation, representation); });
}

} // namespace Interpreter
