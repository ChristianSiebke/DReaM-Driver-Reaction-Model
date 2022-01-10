/********************************************************************************
 * Copyright (c) 2020 ITK Engineering GmbH
 *               2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "Models/ProjectModel.h"

#include <QCoreApplication>
#include <QDir>
#include <QList>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QString>

#include "Models/ProjectXMLLoadModel.h"
#include "Models/ProjectXMLSaveModel.h"
#include "openPASS-Window/WindowInterface.h"

ProjectInterface::Library const ProjectModel::DefaultLibrary = QCoreApplication::applicationDirPath().append(QStringLiteral("/lib"));
ProjectInterface::Config const ProjectModel::DefaultConfig = QCoreApplication::applicationDirPath().append("/configs");
ProjectInterface::Result const ProjectModel::DefaultResult = QCoreApplication::applicationDirPath();
ProjectInterface::LogOpSimulationManager const ProjectModel::DefaultLogOpSimulationManager = QCoreApplication::applicationDirPath().append(QStringLiteral("/opSimulationManager.log"));
ProjectInterface::LogSimulation const ProjectModel::DefaultLogSimulation = QCoreApplication::applicationDirPath().append(QStringLiteral("/opSimulation.log"));
ProjectInterface::Simulation const ProjectModel::DefaultSimulation = QCoreApplication::applicationDirPath().append(QStringLiteral("/opSimulation.exe"));

ProjectModel::ProjectModel(QObject *parent) :
    ProjectInterface(parent), _config(DefaultConfig), _library(DefaultLibrary), _result(DefaultResult), _logOpSimulationManager(DefaultLogOpSimulationManager), _logSimulation(DefaultLogSimulation), _logLevel(0), _simulation(DefaultSimulation)
{
}

bool ProjectModel::clear()
{
    setLibraryPath(DefaultLibrary);
    setConfigPath(DefaultConfig);
    setResultPath(DefaultResult);
    setLogOpSimulationManager(DefaultLogOpSimulationManager);
    setLogSimulation(DefaultLogSimulation);
    setSimulationExe(DefaultSimulation);
    setLogLevel(0);
    Q_EMIT cleared();
    return true;
}

bool ProjectModel::load(const QString &filepath)
{
    clear();
    if (ProjectXMLLoadModel::load(filepath, this))
    {
        Q_EMIT loaded();
        return true;
    }
    return false;
}

bool ProjectModel::save(const QString &filepath) const
{
    if (ProjectXMLSaveModel::save(filepath, this))
    {
        Q_EMIT saved();
        return true;
    }
    return false;
}

bool ProjectModel::simulate()
{
    int ret;

    // Prepare simulation arguments
    QStringList arguments;

    arguments << QString("--logLevel") << QString::number(getLogLevel())
              << QString("--logFile") << getLogSimulation()
              << QString("--lib") << getLibraryPath()
              << QString("--configs") << getConfigPath()
              << QString("--results") << getResultPath();

    QProcess *simulation = new QProcess;

    if (QFileInfo::exists(getSimulationExe()))
    {
        simulation->setProgram(getSimulationExe());
        simulation->setArguments(arguments);
        simulation->start();
    }
    else
    {
        // tell user that simulation exe does not exist
        QMessageBox warning;
        warning.setText("Simulation exe not found! Check simulation path settings in opSimulationManager.xml!");

        warning.setInformativeText("");
        warning.setStandardButtons(QMessageBox::Ok);
        ret = warning.exec();
    }

    return true;
}

bool ProjectModel::setConfigPath(const ProjectInterface::Config &configs)
{
    _config = configs;
    Q_EMIT update();
    return true;
}

QString ProjectModel::relativeToConfigPath(QString const &filepath) const
{
    QDir dir(_config);
    return dir.relativeFilePath(filepath);
}

QString ProjectModel::absoluteToConfigPath(QString const &filepath) const
{
    QDir dir(_config);
    return dir.absoluteFilePath(filepath);
}

ProjectInterface::Config ProjectModel::getConfigPath() const
{
    // Get the path of the directory containing the simulation libraries
    return _config;
}

bool ProjectModel::setLibraryPath(ProjectInterface::Library const &library)
{
    // Update the path to the directory containing the components
    _library = library;
    Q_EMIT update();
    return true;
}

ProjectInterface::Library ProjectModel::getLibraryPath() const
{
    // Get the path of the directory containing the components
    return _library;
}

QDir ProjectModel::getLibrary() const
{
    // Get the directory containing the components
    return QDir(_library);
}

bool ProjectModel::setResultPath(ProjectInterface::Result const &result)
{
    // Update the path to the directory containing the simulation results
    _result = result;
    Q_EMIT update();
    return true;
}

ProjectInterface::Result ProjectModel::getResultPath() const
{
    // Get the path of the directory containing the simulation results
    return _result;
}

bool ProjectModel::setLogOpSimulationManager(ProjectInterface::LogOpSimulationManager const &logOpSimulationManager)
{
    // Update the path to the directory containing the Log File of opSimulationManager
    _logOpSimulationManager = logOpSimulationManager;
    Q_EMIT update();
    return true;
}

ProjectInterface::LogOpSimulationManager ProjectModel::getLogOpSimulationManager() const
{
    // Get the path of the directory containing the Log File of opSimulationManager
    return _logOpSimulationManager;
}

bool ProjectModel::setLogSimulation(ProjectInterface::LogSimulation const &logSimulation)
{
    // Update the path to the directory containing the Log File of the Simulation
    _logSimulation = logSimulation;
    Q_EMIT update();
    return true;
}

ProjectInterface::LogSimulation ProjectModel::getLogSimulation() const
{
    // Get the path of the directory containing the Log File of the Simulation
    return _logSimulation;
}

bool ProjectModel::setLogLevel(ProjectInterface::LogLevel const &logLevel)
{
    // Set the log level
    _logLevel = logLevel;
    Q_EMIT update();
    return true;
}

ProjectInterface::LogLevel ProjectModel::getLogLevel() const
{
    // Get the log lavel
    return _logLevel;
}

bool ProjectModel::setSimulationExe(ProjectInterface::Simulation const &simulation)
{
    _simulation = simulation;
    Q_EMIT update();
    return true;
}

ProjectInterface::Simulation ProjectModel::getSimulationExe() const
{
    // Get the path of the application "opSimulation"
    return _simulation;
}

bool ProjectModel::getProjectStatus() const
{
    if ((!_logSimulation.isEmpty()) && (!_logOpSimulationManager.isEmpty()) && (!_result.isEmpty()) && (!_config.isEmpty()))
        return true;

    return false;
}
