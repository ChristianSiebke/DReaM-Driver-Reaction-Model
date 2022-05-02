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
#include <algorithm>
#include <cstdint>

#include "Common/PerceptionData.h"
#include "Logger.h"

class LoggerInterface {
  public:
    LoggerInterface(Logger& logger) : logger{logger} {}
    LoggerInterface();

    void Log(const std::string& message, DReaMLogLevel level = info) const { logger.Log(message, level); }

  private:
    Logger& logger;
};
