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
#include <cstdarg>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

enum LogLevel_new { debug, warning, error, info };

class Logger {
  public:
      Logger(uint64_t agentId, LogLevel_new defaultLogLevel = info);
      ~Logger();
      void SetPath(std::string path);
      void Log(const std::string &message, LogLevel_new level = info);

  private:
    std::string ToString(LogLevel_new level);

    std::string NowTime() {
        const int MAX_LEN = 200;
        char buffer[MAX_LEN];
        if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, "HH':'mm':'ss", buffer, MAX_LEN) == 0)
            return "Error in NowTime()";

        char result[100] = {0};
        static DWORD first = GetTickCount();
        std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
        return result;
    }

    std::ofstream stream;
    LogLevel_new defaultLogLevel;
    uint64_t agentId;
};
