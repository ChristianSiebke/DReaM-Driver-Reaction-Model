/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QThreadPool>
#include <QElapsedTimer>

#include <algorithm>

#include "frameworkModules.h"
#include "configurationFiles.h"
#include "common/callbacks.h"
#include "commandLineParser.h"
#include "configurationContainer.h"
#include "frameworkModuleContainer.h"
#include "common/log.h"
#include "runInstantiator.h"

#include "directories.h"
#include "common/runtimeInformation.h"
#include "common/version.h"

using namespace core;

//-----------------------------------------------------------------------------
//! \brief SetupLogging Sets up the logging and print pending (buffered)
//! 	   logging messages
//! \param[in] logLevel 0 (none) to 5 (debug core)
//! \param[in] logFile  name of the logfile
//! \param[in] bufferedMessages messages recorded before creation of the logfile
//-----------------------------------------------------------------------------
static void SetupLogging(LogLevel logLevel, const std::string& logFile, const std::vector<std::string>& bufferedMessages);

//-----------------------------------------------------------------------------
//! \brief  Check several parameters of a readable directory
//! \param  directory
//! \param  prefix   for readable logging in case of an error
//! \return true, if everything is allright
//-----------------------------------------------------------------------------
static bool CheckReadableDir(const std::string& directory, const std::string& prefix);

//-----------------------------------------------------------------------------
//! \brief  Check several parameters of a writeable directory
//! \param  directory
//! \param  prefix   for readable logging in case of an error
//! \return true, if everything is allright
//-----------------------------------------------------------------------------
static bool CheckWritableDir(const std::string& directory, const std::string& prefix);

//-----------------------------------------------------------------------------
//! \brief   CheckDirectories Checks the directories given by the command line parameters
//! \param   directories
//! \return  true, if everything is allright
//-----------------------------------------------------------------------------
static bool CheckDirectories(const openpass::core::Directories& directories);

//-----------------------------------------------------------------------------
//! Entry point of program.
//!
//! Setup of program parameters and framework infrastructure.
//! Startup of servers.
//!
//! @param[in]     argc    number of program arguments
//! @param[in]     argv    program arguments
//! @return                program error code / result
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    QElapsedTimer timer;
    timer.start();

    QCoreApplication application(argc, argv);

    application.setApplicationVersion(QString::fromStdString(openpass::common::framework.str()));

    CommandLineArguments parsedArguments = CommandLineParser::Parse(application.arguments());

    SetupLogging(static_cast<LogLevel>(parsedArguments.logLevel),
                 parsedArguments.logFile,
                 CommandLineParser::GetParsingLog());

    openpass::core::Directories directories(QCoreApplication::applicationDirPath().toStdString(),
                                              parsedArguments.libPath,
                                              parsedArguments.configsPath,
                                              parsedArguments.resultsPath);
    if (!CheckDirectories(directories))
    {
        exit(EXIT_FAILURE);
    }

    ConfigurationFiles configurationFiles {directories.configurationDir, "systemConfigBlueprint.xml", "simulationConfig.xml"};
    openpass::common::RuntimeInformation runtimeInformation
    { {openpass::common::framework}, { directories.configurationDir, directories.outputDir, directories.libraryDir }};

    Configuration::ConfigurationContainer configurationContainer(configurationFiles, runtimeInformation);
    if (!configurationContainer.ImportAllConfigurations())
    {
        LOG_INTERN(LogLevel::Error) << "Failed to import all configurations";
        exit(EXIT_FAILURE);
    }

    const auto& libraries = configurationContainer.GetSimulationConfig()->GetExperimentConfig().libraries;
    FrameworkModules frameworkModules
    {
        parsedArguments.logLevel,
        directories.libraryDir,
        libraries.at("DataBufferLibrary"),
        libraries.at("EventDetectorLibrary"),
        libraries.at("ManipulatorLibrary"),
        configurationContainer.GetSimulationConfig()->GetObservationConfig(),
        libraries.at("StochasticsLibrary"),
        libraries.at("WorldLibrary"),
        configurationContainer.GetSimulationConfig()->GetSpawnPointsConfig()
    };

    SimulationCommon::Callbacks callbacks;
    FrameworkModuleContainer frameworkModuleContainer(frameworkModules,
                                                      &configurationContainer,
                                                      runtimeInformation,
                                                      &callbacks);

    RunInstantiator runInstantiator(configurationContainer,
                                    frameworkModuleContainer,
                                    frameworkModules);

    if (runInstantiator.ExecuteRun())
    {
        LOG_INTERN(LogLevel::DebugCore) << "simulation finished successfully";
    }
    else
    {
        LOG_INTERN(LogLevel::Error) << "simulation failed";
        exit(EXIT_FAILURE);
    }

    LOG_INTERN(LogLevel::Error) << "Simulation time elapsed: " << timer.elapsed() << " ms";
    LOG_INTERN(LogLevel::DebugCore) << "end simulation";

    return 0;
}

void SetupLogging(LogLevel logLevel, const std::string& logFile, const std::vector<std::string>& bufferedMessages)
{
    QDir logFilePath(QString::fromStdString(logFile));
    QDir logFileDir = QFileInfo(QString::fromStdString(logFile)).absoluteDir();
    logFileDir.mkpath(".");
    LogOutputPolicy::SetFile(logFilePath.absolutePath().toStdString());
    LogFile::ReportingLevel() = logLevel;
    LOG_INTERN(LogLevel::DebugCore) << std::endl << std::endl << "### simulation start ##";

    // delayed logging, since the log file was unknown before
    std::for_each(bufferedMessages.begin(), bufferedMessages.end(),
    [](const std::string & m) { LOG_INTERN(LogLevel::Warning) << m; });
}

bool CheckDirectories(const openpass::core::Directories& directories)
{
    LOG_INTERN(LogLevel::DebugCore) << "base path: " << directories.baseDir;
    LOG_INTERN(LogLevel::DebugCore) << "library path: " << directories.libraryDir;
    LOG_INTERN(LogLevel::DebugCore) << "configuration path: " << directories.configurationDir;
    LOG_INTERN(LogLevel::DebugCore) << "output path: " << directories.outputDir;

    if (!QDir(QString::fromStdString(directories.outputDir)).exists())
    {
        LOG_INTERN(LogLevel::DebugCore) << "create output folder " + directories.outputDir;
        QDir().mkpath(QString::fromStdString(directories.outputDir));
    }

    auto status_library = CheckReadableDir(directories.libraryDir, "Library");
    auto status_config  = CheckReadableDir(directories.configurationDir, "Configuration");
    auto status_output  = CheckWritableDir(directories.outputDir, "Output");

    // return after probing, so all directories are reported at once
    return status_config && status_library && status_output;
}

bool CheckReadableDir(const std::string& directory, const std::string& prefix)
{
    const QFileInfo dir(QString::fromStdString(directory));
    if (!dir.exists() || !dir.isDir() || !dir.isReadable())
    {
        LOG_INTERN(LogLevel::Error) << prefix << " directory "
                                    << directory << " does not exist, is not a directory, or is not readable";
        return false;
    }
    return true;
}

bool CheckWritableDir(const std::string& directory, const std::string& prefix)
{
    const QFileInfo dir(QString::fromStdString(directory));
    if (!dir.exists() || !dir.isDir() || !dir.isWritable())
    {
        LOG_INTERN(LogLevel::Error) << prefix << " directory "
                                    << directory << " does not exist, is not a directory, or is not writable";
        return false;
    }
    return true;
}
