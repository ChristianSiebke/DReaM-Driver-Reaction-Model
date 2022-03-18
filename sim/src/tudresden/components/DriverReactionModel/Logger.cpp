#include "Logger.h"

#include <stdio.h>

Logger::Logger(uint64_t agentId, LogLevel_new defaultLevel) : agentId{agentId} {
    defaultLogLevel = defaultLevel;
}
void Logger::SetPath(std::string path) {
    stream = std::ofstream(path, std::ofstream::out);
}
Logger::~Logger() { stream.close(); }

void Logger::Log(const std::string& message, LogLevel_new level) {
    stream <<"[" << NowTime() << "] [" << ToString(level) << "]:\t";
    stream <<" | Agent: "<<agentId<< " | "<< message << "\n";
}

std::string Logger::ToString(LogLevel_new level) {
    static const char* const buffer[] = {"DEBUG", "WARN", "ERROR", "INFO"};
    return buffer[level];
}
