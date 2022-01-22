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
#pragma once
#include "InterpreterInterface.h"
#include "RightOfWayRegulationInterface.h"

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

    std::unique_ptr<RightOfWayRegulation::RightOfWayRegulationInterface> rightOfWayRegulation{nullptr};
};
} // namespace Interpreter
