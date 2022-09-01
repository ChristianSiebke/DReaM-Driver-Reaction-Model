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
    worldInterpreter.SetPrimaryTask(std::move(crossingInfoInterpreter));

    std::unique_ptr<Interpreter::InterpreterInterface> collisionInterpreter =
        std::make_unique<Interpreter::CollisionInterpreter>(loggerInterface, GetBehaviourData());
    worldInterpreter.SetPrimaryTask(std::move(collisionInterpreter));

    std::unique_ptr<Interpreter::InterpreterInterface> followingInterpreter =
        std::make_unique<Interpreter::FollowingInterpreter>(loggerInterface, GetBehaviourData());
    worldInterpreter.SetPrimaryTask(std::move(followingInterpreter));

    std::unique_ptr<Interpreter::InterpreterInterface> targetLaneInterpreter =
        std::make_unique<Interpreter::TargetLaneInterpreter>(loggerInterface, GetBehaviourData());
    worldInterpreter.SetPrimaryTask(std::move(targetLaneInterpreter));

    std::unique_ptr<Interpreter::InterpreterInterface> conflictSituationInterpreter =
        std::make_unique<Interpreter::ConflictSituationInterpreter>(loggerInterface, GetBehaviourData());
    worldInterpreter.SetPrimaryTask(std::move(conflictSituationInterpreter));

    std::unique_ptr<Interpreter::InterpreterInterface> rightOfWayInterpreter =
        std::make_unique<Interpreter::RightOfWayInterpreter>(loggerInterface, GetBehaviourData());
    worldInterpreter.SetSecondaryTask(std::move(rightOfWayInterpreter));
}

void CognitiveMap::Update() {
    UpdateWorldRepresentation();
    ResetWorldInterpretation();
    worldInterpreter.ExecuteTasks(&worldInterpretation, worldRepresentation);
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
