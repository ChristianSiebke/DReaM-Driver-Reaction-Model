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
#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

enum DReaMLogLevel { debug, warning, error, info };

class Logger {
  public:
      Logger(uint64_t agentId, std::string logPath, DReaMLogLevel defaultLogLevel = info);
      ~Logger();
      void Log(const std::string &message, DReaMLogLevel level = info);

  private:
    std::string ToString(DReaMLogLevel level);

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
    DReaMLogLevel defaultLogLevel;
    uint64_t agentId;

    static bool cleared;
};
