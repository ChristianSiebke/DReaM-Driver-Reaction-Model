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
#include "Logger.h"
#include "TU_Dresden/Common/PerceptionData.h"
#include <algorithm>
#include <cstdint>

class LoggerInterface {
  public:
    LoggerInterface(Logger& logger) : logger{logger} {}

    void Log(const std::string& message, LogLevel_new level = info) const { logger.Log(message, level); }

  private:
    Logger& logger;
};
