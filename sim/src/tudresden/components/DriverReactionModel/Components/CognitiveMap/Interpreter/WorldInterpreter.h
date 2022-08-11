/******************************************************************************
 * Copyright (c) 2019 TU Dresden
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
    std::vector<std::unique_ptr<Interpreter::InterpreterInterface>> primaryTasks;
    std::vector<std::unique_ptr<Interpreter::InterpreterInterface>> secondaryTasks;
};
} // namespace Interpreter
