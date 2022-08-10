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
#include "CognitiveMap.h"
namespace CognitiveMap {

CognitiveMap::CognitiveMap(int cycleTime, StochasticsInterface* stochastics, LoggerInterface* loggerInterface,
                           const BehaviourData& behaviourData)
    : ComponentInterface(cycleTime, stochastics, loggerInterface, behaviourData), memory(cycleTime, behaviourData, stochastics) {
    std::unique_ptr<Interpreter::InterpreterInterface> crossingInfoInterpreter =
        std::make_unique<Interpreter::CrossingInfoInterpreter>(loggerInterface, GetBehaviourData(), stochastics);
    auto crossingInfoInterpreterCommand = std::make_unique<CommandInterface>(std::move(crossingInfoInterpreter));
    worldInterpreter.SetCommand(std::move(crossingInfoInterpreterCommand));

    std::unique_ptr<Interpreter::InterpreterInterface> collisionInterpreter =
        std::make_unique<Interpreter::CollisionInterpreter>(loggerInterface, GetBehaviourData());
    auto collisionInterpreterCommand = std::make_unique<CommandInterface>(std::move(collisionInterpreter));
    worldInterpreter.SetCommand(std::move(collisionInterpreterCommand));

    std::unique_ptr<Interpreter::InterpreterInterface> followingInterpreter =
        std::make_unique<Interpreter::FollowingInterpreter>(loggerInterface, GetBehaviourData());
    auto followingInterpreterCommand = std::make_unique<CommandInterface>(std::move(followingInterpreter));
    worldInterpreter.SetCommand(std::move(followingInterpreterCommand));

    std::unique_ptr<Interpreter::InterpreterInterface> rightOfWayInterpreter =
        std::make_unique<Interpreter::RightOfWayInterpreter>(loggerInterface, GetBehaviourData());
    auto rightOfWayInterpreterCommand = std::make_unique<CommandInterface>(std::move(rightOfWayInterpreter));
    worldInterpreter.SetCommand(std::move(rightOfWayInterpreterCommand));
}

void CognitiveMap::Update() {
    UpdateWorldRepresentation();
    ResetWorldInterpretation();
    worldInterpreter.ExecuteCommands(&worldInterpretation, worldRepresentation);
};

void CognitiveMap::UpdateInput(int time, std::shared_ptr<EgoPerception> egoAgent,
                               std::vector<std::shared_ptr<AgentPerception>> ambientAgents,
                               std::shared_ptr<InfrastructurePerception> infrastructure,
                               std::vector<const MentalInfrastructure::TrafficSign*> trafficSigns) {
    memory.UpdateSensorInput(time, egoAgent, ambientAgents, infrastructure, trafficSigns);
}

void CognitiveMap::UpdateWorldRepresentation() { memory.UpdateWorldRepresentation(worldRepresentation); }

void CognitiveMap::ResetWorldInterpretation() { worldInterpretation = {}; }

}; // namespace CognitiveMap
