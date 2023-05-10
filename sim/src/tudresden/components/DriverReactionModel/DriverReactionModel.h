/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#pragma once
#include "AgentStateRecorder/AgentStateRecorder.h"
#include "Components/CognitiveMap/CognitiveMap.h"
#include "Components/ComponentInterface.h"
#include "Components/GazeMovement/GazeMovement.h"
#include "Components/GazeMovement/RoadSegments/RoadSegmentInterface.h"
#include "Components/LateralDecision.h"
#include "Components/LongitudinalDecision/LongitudinalDecision.h"
#include "Components/TrafficSignalMemory/TrafficSignalMemory.h"

class DriverReactionModel {
  public:
      DriverReactionModel(std::string behaviourConfigPath, std::string resultPath, LoggerInterface &loggerInterface, int cycleTime,
                          StochasticsInterface *stochastics);
      DriverReactionModel(const DriverReactionModel &) = delete;
      DriverReactionModel(DriverReactionModel &&) = delete;
      DriverReactionModel &operator=(const DriverReactionModel &) = delete;
      DriverReactionModel &operator=(DriverReactionModel &&) = delete;
      ~DriverReactionModel() = default;

      void UpdateDReaM(int time, std::shared_ptr<DetailedAgentPerception> egoAgent,
                       std::vector<std::shared_ptr<GeneralAgentPerception>> ambientAgents,
                       std::shared_ptr<InfrastructurePerception> infrastructure,
                       std::vector<const MentalInfrastructure::TrafficSignal *> trafficSignals);
      double GetAcceleration();
      const LateralAction GetLateralAction();
      const GazeState GetGazeState();
      const std::vector<Common::Vector2d> GetSegmentControlFixationPoints();
      const WorldRepresentation &GetWorldRepresentation();
      const WorldInterpretation &GetWorldInterpretation();
      //--debugging
      std::string GetDebuggingState() {
          return longitudinalDecision->debuggingState;
      }
      //--debugging

  private:
      void UpdateInput(int time, std::shared_ptr<DetailedAgentPerception> egoAgent,
                       std::vector<std::shared_ptr<GeneralAgentPerception>> ambientAgents,
                       std::shared_ptr<InfrastructurePerception> infrastructure,
                       std::vector<const MentalInfrastructure::TrafficSignal *> trafficSignals);
      void UpdateComponents();
      void UpdateAgentStateRecorder(int time, int id, std::shared_ptr<InfrastructurePerception> infrastructure);

      std::shared_ptr<AgentStateRecorder::AgentStateRecorder> agentStateRecorder;
      std::unique_ptr<BehaviourData> behaviourData;
      std::unique_ptr<CognitiveMap::CognitiveMap> cognitiveMap;
      std::unique_ptr<LateralDecision::LateralDecision> lateralDecision;
      std::unique_ptr<GazeMovement::GazeMovement> gazeMovement;
      std::unique_ptr<LongitudinalDecision::LongitudinalDecision> longitudinalDecision;
};
