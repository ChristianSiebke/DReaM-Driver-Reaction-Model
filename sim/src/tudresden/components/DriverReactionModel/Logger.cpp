#include "Logger.h"
#include <conio.h>
#include <dir.h>
#include <process.h>
#include <qcoreapplication.h>
#include <qdir.h>
#include <stdio.h>

Logger::Logger(uint64_t agentId, LogLevel_new defaultLevel) : agentId{agentId} {
    QDir baseDir = QCoreApplication::applicationDirPath();
    std::string path = baseDir.path().toStdString() + "\\agentLogs\\" + "agent" + std::to_string(agentId) + ".txt";
    std::cout << path << std::endl;
    stream = std::ofstream(path, std::ofstream::out);
    // stream.open(fileName.c_str());
    defaultLogLevel = defaultLevel;
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
