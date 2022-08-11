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

#include "DriverReactionModel.h"

#include "Components/CognitiveMap/CognitiveMap.h"
#include "Components/GazeMovement/GazeMovement.h"
#include "Components/Importer/BehaviourImporter.h"
#include "Components/LongitudinalDecision/LongitudinalDecision.h"

DriverReactionModel::DriverReactionModel(std::string behaviourConfigPath, std::string resultPath, LoggerInterface &loggerInterface,
                                         int cycleTime, StochasticsInterface *stochastics) {
    BehaviourImporter importer(behaviourConfigPath, &loggerInterface);
    behaviourData = importer.GetBehaviourData();
    cognitiveMap = std::make_unique<CognitiveMap::CognitiveMap>(cycleTime, stochastics, &loggerInterface, *behaviourData);
    lateralDecision =
        std::make_unique<LateralDecision::LateralDecision>(cognitiveMap->GetWorldRepresentation(), cognitiveMap->GetWorldInterpretation(),
                                                           cycleTime, stochastics, &loggerInterface, *behaviourData);
    gazeMovement =
        std::make_unique<GazeMovement::GazeMovement>(cognitiveMap->GetWorldRepresentation(), cognitiveMap->GetWorldInterpretation(),
                                                     cycleTime, stochastics, &loggerInterface, *behaviourData);
    longitudinalDecision = std::make_unique<LongitudinalDecision::LongitudinalDecision>(cognitiveMap->GetWorldRepresentation(),
                                                                                        cognitiveMap->GetWorldInterpretation(), cycleTime,
                                                                                        stochastics, &loggerInterface, *behaviourData);
    agentStateRecorder = AgentStateRecorder::AgentStateRecorder::GetInstance(resultPath);
}

void DriverReactionModel::UpdateDReaM(int time, std::shared_ptr<EgoPerception> egoAgent,
                                      std::vector<std::shared_ptr<AgentPerception>> ambientAgents,
                                      std::shared_ptr<InfrastructurePerception> infrastructure,
                                      std::vector<const MentalInfrastructure::TrafficSign *> trafficSigns) {
    UpdateInput(time, egoAgent, ambientAgents, infrastructure, trafficSigns);
    UpdateComponents();
    UpdateAgentStateRecorder(time, egoAgent->id, infrastructure);
}

void DriverReactionModel::UpdateInput(int time, std::shared_ptr<EgoPerception> egoAgent,
                                      std::vector<std::shared_ptr<AgentPerception>> ambientAgents,
                                      std::shared_ptr<InfrastructurePerception> infrastructure,
                                      std::vector<const MentalInfrastructure::TrafficSign *> trafficSigns) {
    cognitiveMap->UpdateInput(time, egoAgent, ambientAgents, infrastructure, trafficSigns);
}

void DriverReactionModel::UpdateComponents() {
    cognitiveMap->Update();
    lateralDecision->Update();
    gazeMovement->Update();
    longitudinalDecision->Update();
}

void DriverReactionModel::UpdateAgentStateRecorder(int time, int id, std::shared_ptr<InfrastructurePerception> infrastructure) {
    std::vector<AgentPerception> observedAgents;
    for (const auto &oAgent : *(GetWorldRepresentation().agentMemory)) {
        observedAgents.push_back(oAgent->GetInternalData());
    }
    agentStateRecorder->AddOtherAgents(time, id, observedAgents);
    agentStateRecorder->AddGazeStates(time, id, GetGazeState());
    agentStateRecorder->AddCrossingInfos(time, id, GetWorldInterpretation().crossingInfo);
    agentStateRecorder->AddFixationPoints(time, id, GetSegmentControlFixationPoints());
    agentStateRecorder->AddInfrastructurePerception(infrastructure);
}

double DriverReactionModel::GetAcceleration() {
    return longitudinalDecision->GetAcceleration();
}

const LateralAction DriverReactionModel::GetLateralAction() {
    return lateralDecision->GetLateralAction();
}

const GazeState DriverReactionModel::GetGazeState() {
    return gazeMovement->GetGazeState();
}

const std::vector<Common::Vector2d> DriverReactionModel::GetSegmentControlFixationPoints() {
    return gazeMovement->GetSegmentControlFixationPoints();
}

const WorldRepresentation &DriverReactionModel::GetWorldRepresentation() {
    return cognitiveMap->GetWorldRepresentation();
}
const WorldInterpretation &DriverReactionModel::GetWorldInterpretation() {
    return cognitiveMap->GetWorldInterpretation();
}
