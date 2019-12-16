/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include "openPASS-Project/ProjectInterface.h"

#include <QDir>

class ProjectModel : public ProjectInterface
{
    Q_OBJECT

public:
    explicit ProjectModel(QObject *parent = nullptr);
    virtual ~ProjectModel() = default;

public:
    virtual bool clear() override;
    virtual bool load(QString const &filepath) override;
    virtual bool save(QString const &filepath) const override;
    virtual bool simulate();

public:
    virtual bool setConfigPath(ProjectInterface::Config const &configs) override;
    virtual ProjectInterface::Config getConfigPath() const override;
    virtual QString relativeToConfigPath(QString const &filepath) const override;
    virtual QString absoluteToConfigPath(QString const &filepath) const override;

public:
    virtual bool setLibraryPath(ProjectInterface::Library const &library) override;
    virtual ProjectInterface::Library getLibraryPath() const override;
    virtual QDir getLibrary() const override;

public:
    virtual bool setResultPath(ProjectInterface::Result const &result) override;
    virtual ProjectInterface::Result getResultPath() const override;

public:
    virtual bool setLogMaster(ProjectInterface::LogMaster const &logMaster) override;
    virtual ProjectInterface::LogMaster getLogMaster() const override;

public:
    virtual bool setLogSlave(ProjectInterface::LogSlave const &logSlave) override;
    virtual ProjectInterface::LogSlave getLogSlave() const override;

public:
    virtual bool setLogLevel(ProjectInterface::LogLevel const &logLevel) override;
    virtual ProjectInterface::LogLevel getLogLevel() const override;

public:
    virtual bool setSlaveExe(ProjectInterface::Slave const &slave) override;
    virtual ProjectInterface::Slave getSlaveExe() const override;

public:
    virtual bool getProjectStatus() const override;

protected:
    static ProjectInterface::Config const DefaultConfig;
    static ProjectInterface::Library const DefaultLibrary;
    static ProjectInterface::Result const DefaultResult;
    static ProjectInterface::LogMaster const DefaultLogMaster;
    static ProjectInterface::LogSlave const DefaultLogSlave;
    static ProjectInterface::LogSlave const DefaultSlave;

protected:
    ProjectInterface::Config _config;
    ProjectInterface::Library _library;
    ProjectInterface::Result _result;
    ProjectInterface::LogMaster _logMaster;
    ProjectInterface::LogSlave _logSlave;
    ProjectInterface::LogLevel _logLevel;
    ProjectInterface::Slave _slave;
};

#endif // PROJECTMODEL_H
