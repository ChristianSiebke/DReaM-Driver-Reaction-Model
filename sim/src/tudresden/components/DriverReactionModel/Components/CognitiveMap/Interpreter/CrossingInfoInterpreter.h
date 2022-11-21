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
#include "Common/Helper.h"
#include "Common/MentalInfrastructure/Road.h"
#include "Common/PerceptionData.h"
#include "Common/vector2d.h"
#include "InterpreterInterface.h"
#include "TargetVelocityCalculation.h"
#include "include/stochasticsInterface.h"
#include "math.h"
#include "qglobal.h"

namespace Interpreter {

class CrossingInfoInterpreter : public InterpreterInterface {
  public:
      CrossingInfoInterpreter(LoggerInterface *logger, const BehaviourData &behaviourData, StochasticsInterface *stochastics) :
          InterpreterInterface(logger, behaviourData), targetVelocityCalculation(logger, behaviourData, stochastics) {
      }
    CrossingInfoInterpreter(const CrossingInfoInterpreter&) = delete;
    CrossingInfoInterpreter(CrossingInfoInterpreter&&) = delete;
    CrossingInfoInterpreter& operator=(const CrossingInfoInterpreter&) = delete;
    CrossingInfoInterpreter& operator=(CrossingInfoInterpreter&&) = delete;
    virtual ~CrossingInfoInterpreter() override = default;

    virtual void Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) override;

  private:
    void Localize(const WorldRepresentation& representation);
    void UpdateStoppingPoints(const WorldRepresentation& representation);

    void SetDistanceSP(const AgentRepresentation* representation,                   
                       StoppingPointMap &stoppingpoints);

    void DetermineCrossingType(const WorldRepresentation& representation);
    void DetermineCrossingPhase(const WorldRepresentation& representation);

    CrossingInfo crossingInfo;
    TargetVelocityCalculation targetVelocityCalculation;
};
} // namespace Interpreter
