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

QString const ProjectXMLBaseModel::KeyMasterConfig = QStringLiteral("masterConfig");
QString const ProjectXMLBaseModel::KeySlaveConfigs = QStringLiteral("slaveConfigs");
QString const ProjectXMLBaseModel::KeySlaveConfig = QStringLiteral("slaveConfig");
QString const ProjectXMLBaseModel::KeyLibraryPath = QStringLiteral("libraries");
QString const ProjectXMLBaseModel::KeyResultPath = QStringLiteral("results");
QString const ProjectXMLBaseModel::KeyConfigPath = QStringLiteral("configurations");
QString const ProjectXMLBaseModel::KeyLogFileMaster = QStringLiteral("logFileMaster");
QString const ProjectXMLBaseModel::KeyLogFileSlave = QStringLiteral("logFileSlave");
QString const ProjectXMLBaseModel::KeyLogLevel = QStringLiteral("logLevel");
QString const ProjectXMLBaseModel::KeySlavePath = QStringLiteral("slave");

QList<QString> const ProjectXMLBaseModel::KeyListMasterConfig = {
    ProjectXMLBaseModel::KeyLogLevel,
    ProjectXMLBaseModel::KeyLogFileMaster,
    ProjectXMLBaseModel::KeySlavePath,
    ProjectXMLBaseModel::KeySlaveConfigs,
    ProjectXMLBaseModel::KeyLibraryPath
};

QList<QString> const ProjectXMLBaseModel::KeyListSlaveConfig = {
    ProjectXMLBaseModel::KeyLogFileSlave,
    ProjectXMLBaseModel::KeyConfigPath,
    ProjectXMLBaseModel::KeyResultPath,
};
