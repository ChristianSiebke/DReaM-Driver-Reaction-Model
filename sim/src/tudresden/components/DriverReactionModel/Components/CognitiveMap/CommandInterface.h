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
#include <memory>
struct WorldInterpretation;
struct WorldRepresentation;

namespace CognitiveMap {

class CommandInterface {
  public:
    CommandInterface(std::unique_ptr<Interpreter::InterpreterInterface> interpreter) : interpreter{std::move(interpreter)} {}
    CommandInterface(const CommandInterface&) {}
    CommandInterface(CommandInterface&&) {}
    CommandInterface& operator=(const CommandInterface&);
    CommandInterface& operator=(CommandInterface&&);
    ~CommandInterface() = default;

    void Execute(WorldInterpretation* worldInterpretation, const WorldRepresentation& worldRepresentation) {
        interpreter->Update(worldInterpretation, worldRepresentation);
    }

  private:
    std::unique_ptr<Interpreter::InterpreterInterface> interpreter;
};
} // namespace CognitiveMap
