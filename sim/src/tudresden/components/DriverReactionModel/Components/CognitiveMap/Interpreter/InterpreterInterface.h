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
#include "Common/BehaviourData.h"
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

    void Log(const std::string& message, DReaMLogLevel level = info) const { loggerInterface->Log(message, level); }

    const BehaviourData& GetBehaviourData() const { return behaviourData; }

  private:
    LoggerInterface* loggerInterface;
    const BehaviourData& behaviourData;
};
} // namespace Interpreter
