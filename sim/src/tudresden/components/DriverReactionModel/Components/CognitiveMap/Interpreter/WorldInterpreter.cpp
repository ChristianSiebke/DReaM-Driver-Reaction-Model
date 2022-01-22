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
#include "CognitiveMap.h"
namespace Interpreter {
void WorldInterpreter::SetCommand(std::unique_ptr<CognitiveMap::CommandInterface> command) { commands.push_back(std::move(command)); }

void WorldInterpreter::ExecuteCommands(WorldInterpretation* interpretation, const WorldRepresentation& representation) {

    // transfer agents
    for (const auto& agent : *representation.agentMemory) {
        auto agentInterpretation = std::make_unique<AgentInterpretation>(agent.get());
        interpretation->interpretedAgents.insert(std::make_pair(agent->GetID(), std::move(agentInterpretation)));
    }
    // update interpretation
    std::for_each(commands.begin(), commands.end(),
                  [&interpretation, &representation](auto& command) { command->Execute(interpretation, representation); });
}

} // namespace Interpreter
