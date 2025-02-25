/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "DriverReactionModel.h"

#include "Components/CognitiveMap/CognitiveMap.h"
#include "Components/GazeMovement/GazeMovement.h"
#include "Components/LongitudinalDecision/LongitudinalDecision.h"

DriverReactionModel::DriverReactionModel(std::string behaviourConfigPath, std::string resultPath, LoggerInterface &loggerInterface,
                                         int cycleTime, StochasticsInterface *stochastics, DReaMDefinitions::AgentVehicleType agentType) {
    importer->GetInstance(behaviourConfigPath, &loggerInterface);
    behaviourData = importer->GetBehaviourData(agentType);
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

void DriverReactionModel::UpdateDReaM(int time, std::shared_ptr<DetailedAgentPerception> egoAgent,
                                      std::vector<std::shared_ptr<GeneralAgentPerception>> ambientAgents,
                                      std::shared_ptr<InfrastructurePerception> infrastructure,
                                      std::vector<const MentalInfrastructure::TrafficSignal *> trafficSignals) {
    UpdateInput(time, egoAgent, ambientAgents, infrastructure, trafficSignals);
    UpdateComponents();
    UpdateAgentStateRecorder(egoAgent->id, infrastructure);
}

void DriverReactionModel::UpdateInput(int time, std::shared_ptr<DetailedAgentPerception> egoAgent,
                                      std::vector<std::shared_ptr<GeneralAgentPerception>> ambientAgents,
                                      std::shared_ptr<InfrastructurePerception> infrastructure,
                                      std::vector<const MentalInfrastructure::TrafficSignal *> trafficSignals) {
    cognitiveMap->UpdateInput(time, egoAgent, ambientAgents, infrastructure, trafficSignals);
}

void DriverReactionModel::UpdateComponents() {
    cognitiveMap->Update();
    lateralDecision->Update();
    gazeMovement->Update();
    longitudinalDecision->Update();
}

void DriverReactionModel::UpdateAgentStateRecorder(int id, std::shared_ptr<InfrastructurePerception> infrastructure) {
    agentStateRecorder->BufferTimeStep(id, GetGazeState(), *GetWorldRepresentation().agentMemory, GetWorldInterpretation().crossingInfo,
                                       GetSegmentControlFixationPoints(), GetWorldRepresentation().trafficSignalMemory->memory);
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

const AnalysisSignal DriverReactionModel::GetAnalysisSignal() {
    auto data = cognitiveMap->GetWorldInterpretation().analysisData.get();
    for (auto &agent : cognitiveMap->GetWorldInterpretation().interpretedAgents) {
        if (std::any_of(GetWorldRepresentation().processedAgents.begin(), GetWorldRepresentation().processedAgents.end(),
                        [id = agent.first](std::shared_ptr<GeneralAgentPerception> processedAgent) { return processedAgent->id == id; }) &&
            agent.second->collisionPoint.has_value()) {
            double ttc = agent.second->collisionPoint->timeToCollision;
            data->ttcs.insert(std::make_pair(agent.first, ttc));
        }
    }
    data->maxComfortDeceleration = -1.5;
    return *data;
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

