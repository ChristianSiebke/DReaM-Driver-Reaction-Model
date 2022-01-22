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
#include "Common/WorldRepresentation.h"
#include "InterpreterInterface.h"

namespace Interpreter {

class FollowingInterpreter : public InterpreterInterface {
  public:
    FollowingInterpreter(LoggerInterface* logger, const BehaviourData& behaviourData) : InterpreterInterface(logger, behaviourData) {}
    FollowingInterpreter(const FollowingInterpreter&) = delete;
    FollowingInterpreter(FollowingInterpreter&&) = delete;
    FollowingInterpreter& operator=(const FollowingInterpreter&) = delete;
    FollowingInterpreter& operator=(FollowingInterpreter&&) = delete;
    virtual ~FollowingInterpreter() override = default;

    virtual void Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) override;

  protected:
    std::optional<double> CalculateFollowingDistance(const AgentRepresentation& agent, const WorldRepresentation& representation);
};
} // namespace Interpreter
