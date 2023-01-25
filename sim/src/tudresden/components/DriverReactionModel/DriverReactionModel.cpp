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

#include "Common/TimeMeasurement.hpp"
#include "Components/CognitiveMap/CognitiveMap.h"
#include "Components/GazeMovement/GazeMovement.h"
#include "Components/Importer/BehaviourImporter.h"
#include "Components/LongitudinalDecision/LongitudinalDecision.h"

TimeMeasurement timeMeasure("DriverReactionModel.cpp");

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

void DriverReactionModel::UpdateDReaM(int time, std::shared_ptr<DetailedAgentPerception> egoAgent,
                                      std::vector<std::shared_ptr<GeneralAgentPerception>> ambientAgents,
                                      std::shared_ptr<InfrastructurePerception> infrastructure,
                                      std::vector<const MentalInfrastructure::TrafficSignal *> trafficSignals) {
    UpdateInput(time, egoAgent, ambientAgents, infrastructure, trafficSignals);
    UpdateComponents();
    UpdateAgentStateRecorder(time, egoAgent->id, infrastructure);
}

void DriverReactionModel::UpdateInput(int time, std::shared_ptr<DetailedAgentPerception> egoAgent,
                                      std::vector<std::shared_ptr<GeneralAgentPerception>> ambientAgents,
                                      std::shared_ptr<InfrastructurePerception> infrastructure,
                                      std::vector<const MentalInfrastructure::TrafficSignal *> trafficSignals) {
    cognitiveMap->UpdateInput(time, egoAgent, ambientAgents, infrastructure, trafficSignals);
}

void DriverReactionModel::UpdateComponents() {
    timeMeasure.StartTimePoint("CognitiveMap Update");
    cognitiveMap->Update();
    timeMeasure.EndTimePoint();
    timeMeasure.StartTimePoint("Lateral Decision Update");
    lateralDecision->Update();
    timeMeasure.EndTimePoint();
    timeMeasure.StartTimePoint("GazeMovement Update");
    gazeMovement->Update();
    timeMeasure.EndTimePoint();
    timeMeasure.StartTimePoint("Long. Decision Update");
    longitudinalDecision->Update();
    timeMeasure.EndTimePoint();
}

void DriverReactionModel::UpdateAgentStateRecorder(int time, int id, std::shared_ptr<InfrastructurePerception> infrastructure) {
    std::vector<GeneralAgentPerception> observedAgents;
    for (const auto &oAgent : *(GetWorldRepresentation().agentMemory)) {
        observedAgents.push_back(oAgent->GetInternalData());
    }
    agentStateRecorder->AddOtherAgents(time, id, observedAgents);
    agentStateRecorder->AddGazeStates(time, id, GetGazeState());
    agentStateRecorder->AddCrossingInfos(time, id, GetWorldInterpretation().crossingInfo);
    agentStateRecorder->AddFixationPoints(time, id, GetSegmentControlFixationPoints());
    agentStateRecorder->AddInfrastructurePerception(infrastructure);
    agentStateRecorder->AddTrafficSignals(time, id, GetWorldRepresentation().trafficSignalMemory->memory);
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

