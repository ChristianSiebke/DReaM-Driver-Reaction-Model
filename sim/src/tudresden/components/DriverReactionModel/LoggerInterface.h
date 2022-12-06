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
