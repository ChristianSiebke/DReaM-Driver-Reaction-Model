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
#include <algorithm>
#include <memory>
#include <vector>
#include "Common/Definitions.h"
namespace Interpreter {
class WorldInterpreter {
  public:
    WorldInterpreter() {}
    WorldInterpreter(const WorldInterpreter&) = delete;
    WorldInterpreter(WorldInterpreter&&) = delete;
    WorldInterpreter& operator=(const WorldInterpreter&) = delete;
    WorldInterpreter& operator=(WorldInterpreter&&) = delete;
    ~WorldInterpreter() = default;

    void ExecuteTasks(WorldInterpretation *interpretation, const WorldRepresentation &representation);

    void SetPrimaryTask(std::unique_ptr<Interpreter::InterpreterInterface> task);
    void SetSecondaryTask(std::unique_ptr<Interpreter::InterpreterInterface> task);

private:
    std::vector<std::unique_ptr<Interpreter::InterpreterInterface>> primaryTasks{};
    std::vector<std::unique_ptr<Interpreter::InterpreterInterface>> secondaryTasks{};
};
} // namespace Interpreter
