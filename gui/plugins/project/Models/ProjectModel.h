/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QDir>

#include "openPASS-Project/ProjectInterface.h"

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
    virtual bool setLogOpSimulationManager(ProjectInterface::LogOpSimulationManager const &logOpSimulationManager) override;
    virtual ProjectInterface::LogOpSimulationManager getLogOpSimulationManager() const override;

public:
    virtual bool setLogSimulation(ProjectInterface::LogSimulation const &logSimulation) override;
    virtual ProjectInterface::LogSimulation getLogSimulation() const override;

public:
    virtual bool setLogLevel(ProjectInterface::LogLevel const &logLevel) override;
    virtual ProjectInterface::LogLevel getLogLevel() const override;

public:
    virtual bool setSimulationExe(ProjectInterface::Simulation const &simulation) override;
    virtual ProjectInterface::Simulation getSimulationExe() const override;

public:
    virtual bool getProjectStatus() const override;

protected:
    static ProjectInterface::Config const DefaultConfig;
    static ProjectInterface::Library const DefaultLibrary;
    static ProjectInterface::Result const DefaultResult;
    static ProjectInterface::LogOpSimulationManager const DefaultLogOpSimulationManager;
    static ProjectInterface::LogSimulation const DefaultLogSimulation;
    static ProjectInterface::LogSimulation const DefaultSimulation;

protected:
    ProjectInterface::Config _config;
    ProjectInterface::Library _library;
    ProjectInterface::Result _result;
    ProjectInterface::LogOpSimulationManager _logOpSimulationManager;
    ProjectInterface::LogSimulation _logSimulation;
    ProjectInterface::LogLevel _logLevel;
    ProjectInterface::Simulation _simulation;
};

#endif // PROJECTMODEL_H
