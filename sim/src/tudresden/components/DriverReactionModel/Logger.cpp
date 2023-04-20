/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "Logger.h"

#include <stdio.h>

bool Logger::cleared = false;

Logger::Logger(uint64_t agentId, std::string logPath, DReaMLogLevel defaultLevel) : agentId{agentId} {
    defaultLogLevel = defaultLevel;
    if (!cleared) {
        cleared = true;
        std::filesystem::remove_all(logPath + "\\agentLogs");
        std::filesystem::create_directories(logPath + "\\agentLogs");
    }
    std::string path = logPath + "\\agentLogs\\" + "agent" + std::to_string(agentId) + ".txt";
    stream = std::ofstream(path, std::ofstream::out);
}

Logger::~Logger() { stream.close(); }

void Logger::Log(const std::string &message, DReaMLogLevel level) {
    stream << "[" << NowTime() << "] [" << ToString(level) << "]:\t";
    stream <<" | Agent: "<<agentId<< " | "<< message << "\n";
}

std::string Logger::ToString(DReaMLogLevel level) {
    static const char* const buffer[] = {"DEBUG", "WARN", "ERROR", "INFO"};
    return buffer[level];
}
