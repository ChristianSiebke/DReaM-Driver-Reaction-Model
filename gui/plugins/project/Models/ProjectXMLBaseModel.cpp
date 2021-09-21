/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/

#include "ProjectXMLBaseModel.h"

QString const ProjectXMLBaseModel::KeyOpSimulationManager = QStringLiteral("opSimulationManager");
QString const ProjectXMLBaseModel::KeySimulationConfigs = QStringLiteral("simulationConfigs");
QString const ProjectXMLBaseModel::KeySimulationConfig = QStringLiteral("simulationConfig");
QString const ProjectXMLBaseModel::KeyLibraryPath = QStringLiteral("libraries");
QString const ProjectXMLBaseModel::KeyResultPath = QStringLiteral("results");
QString const ProjectXMLBaseModel::KeyConfigPath = QStringLiteral("configurations");
QString const ProjectXMLBaseModel::KeyLogFileOpSimulationManager = QStringLiteral("logFileSimulationManager");
QString const ProjectXMLBaseModel::KeyLogFileSimulation = QStringLiteral("logFileSimulation");
QString const ProjectXMLBaseModel::KeyLogLevel = QStringLiteral("logLevel");
QString const ProjectXMLBaseModel::KeySimulationPath = QStringLiteral("simulation");

QList<QString> const ProjectXMLBaseModel::KeyListOpSimulationManager = {
    ProjectXMLBaseModel::KeyLogLevel,
    ProjectXMLBaseModel::KeyLogFileOpSimulationManager,
    ProjectXMLBaseModel::KeySimulationPath,
    ProjectXMLBaseModel::KeySimulationConfigs,
    ProjectXMLBaseModel::KeyLibraryPath};

QList<QString> const ProjectXMLBaseModel::KeyListSimulationConfig = {
    ProjectXMLBaseModel::KeyLogFileSimulation,
    ProjectXMLBaseModel::KeyConfigPath,
    ProjectXMLBaseModel::KeyResultPath,
};
