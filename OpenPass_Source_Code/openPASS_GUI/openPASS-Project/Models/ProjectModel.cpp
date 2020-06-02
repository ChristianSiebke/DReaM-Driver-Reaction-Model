/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* Copyright (c) 2020 HLRS, University of Stuttgart.
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "Models/ProjectModel.h"

#include "Models/ProjectXMLLoadModel.h"
#include "Models/ProjectXMLSaveModel.h"

#include <QCoreApplication>
#include <QDir>
#include <QList>
#include <QProcess>
#include <QStandardPaths>
#include <QString>

ProjectInterface::Library const ProjectModel::DefaultLibrary = QCoreApplication::applicationDirPath().append( QStringLiteral("/../lib"));      
ProjectInterface::Config const ProjectModel::DefaultConfig = QCoreApplication::applicationDirPath().append(QStringLiteral("/../configs"));
ProjectInterface::Result const ProjectModel::DefaultResult = QCoreApplication::applicationDirPath().append(QStringLiteral("/.."));
ProjectInterface::LogMaster const ProjectModel::DefaultLogMaster = QCoreApplication::applicationDirPath().append( QStringLiteral("/../OpenPassMaster.log"));
ProjectInterface::LogSlave const ProjectModel::DefaultLogSlave = QCoreApplication::applicationDirPath().append( QStringLiteral("/../OpenPassSlave.log"));
ProjectInterface::Slave const ProjectModel::DefaultSlave = QCoreApplication::applicationDirPath().append( QStringLiteral("/OpenPassSlave.exe"));

ProjectModel::ProjectModel(QObject *parent)
    : ProjectInterface(parent)
    , _config(DefaultConfig)
    , _library(DefaultLibrary)
    , _result(DefaultResult)
    , _logMaster(DefaultLogMaster)
    , _logSlave(DefaultLogSlave)
    , _logLevel(0)
    , _slave(DefaultSlave)
{
}

bool ProjectModel::clear()
{
    setLibraryPath(DefaultLibrary);
    setConfigPath(DefaultConfig);
    setResultPath(DefaultResult);
    setLogMaster(DefaultLogMaster);
    setLogSlave(DefaultLogSlave);
    setSlaveExe(DefaultSlave);
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
 /*   QDir const root = QDir(QCoreApplication::applicationDirPath());
    QString master = (root.canonicalPath()).append("/OpenPassMaster.exe");
    QProcess *process = new QProcess(this);
    process->start(master);*/

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

bool ProjectModel::setLogMaster(ProjectInterface::LogMaster const &logMaster)
{
    // Update the path to the directory containing the Log File of the Master
    _logMaster = logMaster;
    Q_EMIT update();
    return true;
}

ProjectInterface::LogMaster ProjectModel::getLogMaster() const
{
    // Get the path of the directory containing the Log File of the Master
    return _logMaster;
}

bool ProjectModel::setLogSlave(ProjectInterface::LogSlave const &logSlave)
{
    // Update the path to the directory containing the Log File of the Slave
    _logSlave = logSlave;
    Q_EMIT update();
    return true;
}

ProjectInterface::LogSlave ProjectModel::getLogSlave() const
{
    // Get the path of the directory containing the Log File of the Slave
    return _logSlave;
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

bool ProjectModel::setSlaveExe(ProjectInterface::Slave const &slave)
{
    _slave = slave;
    Q_EMIT update();
    return true;
}

ProjectInterface::Slave ProjectModel::getSlaveExe() const
{
    // Get the path of the application "OpenPassSlave"
    return _slave;
}

bool ProjectModel::getProjectStatus() const
{
    if ((!_logSlave.isEmpty())&&(!_logMaster.isEmpty())
            &&(!_result.isEmpty())&&(!_config.isEmpty()))
        return true;

    return false;
}
