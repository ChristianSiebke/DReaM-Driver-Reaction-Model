/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2016-2019 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//! @file  main.cpp
//! @brief This file contains the main entry point.
//!
//! The main module handles the initialization and starts the simulation.
//!
//! @mainpage openPASS
//!
//! @section intro_sec Introduction
//!
//! opSimulationManager component
//! This program component represent opSimulationManager node of the pre-crash
//! simulation tool.
//-----------------------------------------------------------------------------

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QProcess>
#include <QThreadPool>
#include <QtConcurrent>

#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "common/log.h"
#include "config.h"
#include "../importer/configImporter.h"
#include "processManager.h"

using namespace SimulationManager;

using Arguments = std::vector<std::pair<std::string, std::string>>;

//-----------------------------------------------------------------------------
//! \brief Parses command line arguments for the opSimulationManager config file.
//! \param[in] arguments The list of command line arguments to parse.
//! \returns The supplied opSimulationManager config file name.
//-----------------------------------------------------------------------------
static QString ParseArguments(const QStringList& arguments);

//-----------------------------------------------------------------------------
//! \brief Creates the directories necessary to make the resultPath valid, if
//! 	   they don't currently exist.
//! \param[in] resultPath A string representing the desired path to results.
//-----------------------------------------------------------------------------
static void CreateResultPathIfNecessary(const std::string& resultPath);

//-----------------------------------------------------------------------------
//! \brief Initializes application logging.
//! \param[in] logPath The path identifying where to write the logs.
//! \param[in] logLevel The log reporting level.
//! \returns a std::string representing the path to the log file
//-----------------------------------------------------------------------------
std::string InitLogging(const std::string& logPath, int logLevel);

//-----------------------------------------------------------------------------
//! \brief Parses opSimulationManager configuration file for opSimulationManager configuration
//! 	   details.
//! 	   Throws if the configuration import fails.
//! \param[in] opSimulationManagerConfigFile The file containing the configuration details.
//! \returns A Configuration::Config object containing opSimulationManager
//! 	     configuration details.
//-----------------------------------------------------------------------------
Configuration::Config ParseConfig(const QString& opSimulationManagerConfigFile);

#ifndef USESIMULATIONLIBRARY
//-----------------------------------------------------------------------------
//! \brief Retrieve the name of the simulations executable
//-----------------------------------------------------------------------------
std::string GetExecutable(std::string simulation);
#else
typedef int (*SimulationRunFunction)(int argc, char* argv[]);

//-----------------------------------------------------------------------------
//! \brief Retrieve the run function of the simulation
//-----------------------------------------------------------------------------
SimulationRunFunction GetExecutable(std::string simulation);
#endif // USESIMULATIONLIBRARY

#ifndef OPENPASSOPSIMULATIONMANAGERLIBRARY
//-----------------------------------------------------------------------------
//! \brief Main entry point called by main
//! \param[in] argc		argument counter
//! \param[in] argv[]   argument vector
//-----------------------------------------------------------------------------
int run(int argc, char* argv[])
#else
//-----------------------------------------------------------------------------
//! \brief Main entry point, called by external program
//! \param[in] argc		argument counter
//! \param[in] argv[]   argument vector
//-----------------------------------------------------------------------------
extern "C" Q_DECL_EXPORT int run(int argc, char* argv[])
#endif //OPENPASSOPSIMULATIONMANAGERLIBRARY
{
    QCoreApplication app(argc, argv);
    QString opSimulationManagerConfigFile = ParseArguments(app.arguments());

    auto opSimulationManagerConfig = ParseConfig(opSimulationManagerConfigFile);
    auto logFile = InitLogging(opSimulationManagerConfig.logFileOpSimulationManager, opSimulationManagerConfig.logLevel);
    auto simulation =  GetExecutable(opSimulationManagerConfig.simulation);

    LOG_INTERN(LogLevel::DebugCore) << "\n\n## opSimulationManager start ##";
    LOG_INTERN(LogLevel::DebugCore) << "log level: " << opSimulationManagerConfig.logLevel;
    LOG_INTERN(LogLevel::DebugCore) << "log file opSimulationManager: " << logFile;
    LOG_INTERN(LogLevel::DebugCore) << "simulation: " << simulation;
    LOG_INTERN(LogLevel::DebugCore) << "libraries: " << opSimulationManagerConfig.libraries;
    LOG_INTERN(LogLevel::DebugCore) << "number of simulations: " << opSimulationManagerConfig.simulationConfigs.size();

    for (const auto& simulationConfig : opSimulationManagerConfig.simulationConfigs)
    {
        CreateResultPathIfNecessary(simulationConfig.results);

        Arguments arguments
        {
            { "--logLevel", std::to_string(opSimulationManagerConfig.logLevel) },
            { "--logFile",  simulationConfig.logFile },
            { "--lib",      opSimulationManagerConfig.libraries },
            { "--configs",  simulationConfig.configs },
            { "--results",  simulationConfig.results }
        };

        #ifndef USESIMULATIONLIBRARY

        if (ProcessManager::getInstance().StartProcess(simulation, arguments))
        {
            std::stringstream strStream;
            for (const auto& [command, value] : arguments)
            {
                strStream << command << " " << value << "\n";
            }
            LOG_INTERN(LogLevel::Info) << simulation << " started with \n" << strStream.str() << std::endl;
        }
        else
        {
            LOG_INTERN(LogLevel::Error) << simulation << " not started, check path.";
            exit(EXIT_FAILURE);
        }
    }
    ProcessManager::getInstance().WaitAndClear();

        #else
        QtConcurrent::run([arguments, &argv, &simulation]
        {
            int i = 0;
            char* argumentsValue[arguments.size() + 1 ];
            argumentsValue[i++] = argv[0];

            for (QString value : arguments)
            {
                argumentsValue[i] = new char [value.toLocal8Bit().size()];
                strcpy(argumentsValue[i], value.toLocal8Bit().constData());
                i++;
            }
            simulation(i, argumentsValue);
        });

    }
    QThreadPool::globalInstance()->waitForDone();
        #endif // USESIMULATIONLIBRARY

    LOG_INTERN(LogLevel::DebugCore) << "## opSimulationManager finished ##";
    return 0;
}

std::string InitLogging(const std::string& logPath, int logLevel)
{
    QDir baseDir = QCoreApplication::applicationDirPath();
    QString logFile = baseDir.absoluteFilePath(QString::fromStdString(logPath));
    logFile = baseDir.cleanPath(logFile);

    LogOutputPolicy::SetFile(logFile.toStdString());
    LogFile::ReportingLevel() = static_cast<LogLevel>(logLevel);
    return logFile.toStdString();
}

QString ParseArguments(const QStringList& arguments)
{
    QCommandLineParser commandLineParser;
    commandLineParser.addHelpOption();

    QCommandLineOption optionConfigFile(
        QStringList() << "c" << "config",
        "configuration <opSimulationManager.xml>",
        "opSimulationManager",
        QCoreApplication::applicationDirPath() + "/opSimulationManager.xml");

    commandLineParser.addOption(optionConfigFile);
    commandLineParser.process(arguments);

    return commandLineParser.value(optionConfigFile);
}

void CreateResultPathIfNecessary(const std::string& resultPath)
{
    auto qResultPath = QString::fromStdString(resultPath);
    if (!QDir(qResultPath).exists())
    {
        LOG_INTERN(LogLevel::DebugCore) << "created result folder " << resultPath;
        QDir().mkpath(qResultPath);
    }
}

Configuration::Config ParseConfig(const QString& opSimulationManagerConfigFile)
{
    try
    {
        return Configuration::ConfigImporter::Import(opSimulationManagerConfigFile);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Configuration related error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

#ifndef USESIMULATIONLIBRARY
std::string GetExecutable(std::string simulation)
{
    #ifndef USESIMULATIONLIBRARY

    #if WIN32
    if (QString::fromStdString(simulation).split(".").last() != "exe")
    {
        simulation += DEBUG_POSTFIX".exe";
    }
    #endif

    #endif // USESIMULATIONLIBRARY
    return simulation;
}
#else
SimulationRunFunction GetExecutable(std::string simulation)
{
    QLibrary simulationLib(QString::fromStdString(simulation));
    if (!simulationLib.load())
    {
        LOG_INTERN(LogLevel::Error) << " failed to load library " << simulation << " " << simulationLib.errorString().toStdString();
        exit(EXIT_FAILURE);
    }

    SimulationRunFunction SimulationRunFunc = (SimulationRunFunction)simulationLib.resolve("run");

    if (!SimulationRunFunc)
    {
        LOG_INTERN(LogLevel::Error) << " unable to resolve run function in library " << simulation;
        exit(EXIT_FAILURE);
    }

    return SimulationRunFunc;
}
#endif // USESIMULATIONLIBRARY

#ifndef OPENPASSOPSIMULATIONMANAGERLIBRARY
int main(int argc, char* argv[])
{
    return run(argc, argv);
}
#endif //OPENPASSOPSIMULATIONMANAGERLIBRARY
