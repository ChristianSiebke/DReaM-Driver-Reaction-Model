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
#include "InterpreterInterface.h"
#include "RightOfWayRegulation/RightOfWayRegulationInterface.h"

namespace Interpreter {

class RightOfWayInterpreter : public InterpreterInterface {
  public:
    RightOfWayInterpreter(LoggerInterface* logger, const BehaviourData& behaviourData) : InterpreterInterface(logger, behaviourData) {}
    RightOfWayInterpreter(const RightOfWayInterpreter&) = delete;
    RightOfWayInterpreter(RightOfWayInterpreter&&) = delete;
    RightOfWayInterpreter& operator=(const RightOfWayInterpreter&) = delete;
    RightOfWayInterpreter& operator=(RightOfWayInterpreter&&) = delete;
    virtual ~RightOfWayInterpreter() override = default;

    virtual void Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) override;

  private:
    void UpdateRightOfWayRegulation(const WorldRepresentation& representation);

    RightOfWay PerformRightOfWayDetermination(const AgentRepresentation&, const WorldRepresentation& representation);

    std::optional<JunctionSituation> JunctionSituation(const EgoAgentRepresentation *ego, const AgentRepresentation &observedAgent) const;
    bool IsMovingTowardsJunction(const AgentRepresentation &agent, const MentalInfrastructure::Junction *junction) const;

    std::unique_ptr<RightOfWayRegulation::RightOfWayRegulationInterface> rightOfWayRegulation{nullptr};
};
} // namespace Interpreter
