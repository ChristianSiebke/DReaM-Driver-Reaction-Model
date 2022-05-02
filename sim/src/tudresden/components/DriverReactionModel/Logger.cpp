#include "Logger.h"

#include <stdio.h>

Logger::Logger(uint64_t agentId, DReaMLogLevel defaultLevel) : agentId{agentId} {
    defaultLogLevel = defaultLevel;
}
void Logger::SetPath(std::string path) {
    stream = std::ofstream(path, std::ofstream::out);
}
Logger::~Logger() { stream.close(); }

void Logger::Log(const std::string &message, DReaMLogLevel level) {
    stream <<"[" << NowTime() << "] [" << ToString(level) << "]:\t";
    stream <<" | Agent: "<<agentId<< " | "<< message << "\n";
}

std::string Logger::ToString(DReaMLogLevel level) {
    static const char* const buffer[] = {"DEBUG", "WARN", "ERROR", "INFO"};
    return buffer[level];
}
