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
#include "BehaviourData.h"
#include "LoggerInterface.h"

struct WorldInterpretation;
struct WorldRepresentation;

namespace Interpreter {

class InterpreterInterface {
  public:
    InterpreterInterface(LoggerInterface* loggerInterface, const BehaviourData& behaviourData)
        : loggerInterface(loggerInterface), behaviourData(behaviourData) {}
    InterpreterInterface(const InterpreterInterface&) = delete;
    InterpreterInterface(InterpreterInterface&&) = delete;
    InterpreterInterface& operator=(const InterpreterInterface&) = delete;
    InterpreterInterface& operator=(InterpreterInterface&&) = delete;
    virtual ~InterpreterInterface() = default;

    virtual void Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) = 0;

    void Log(const std::string& message, LogLevel_new level = info) const { loggerInterface->Log(message, level); }

    const BehaviourData& GetBehaviourData() const { return behaviourData; }

  private:
    LoggerInterface* loggerInterface;
    const BehaviourData& behaviourData;
};
} // namespace Interpreter
