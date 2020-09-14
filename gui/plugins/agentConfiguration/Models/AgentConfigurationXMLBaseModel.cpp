/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "AgentConfigurationXMLBaseModel.h"

QString const AgentConfigurationXMLBaseModel::KeyRoot = QStringLiteral("Profiles");

QString const AgentConfigurationXMLBaseModel::KeyAgentProfiles = QStringLiteral("AgentProfiles");
QString const AgentConfigurationXMLBaseModel::KeyAgentProfile = QStringLiteral("AgentProfile");

QString const AgentConfigurationXMLBaseModel::KeyDriverProfiles = QStringLiteral("DriverProfiles");
QString const AgentConfigurationXMLBaseModel::KeyDriverProfile = QStringLiteral("DriverProfile");

QString const AgentConfigurationXMLBaseModel::KeyVehicleProfiles = QStringLiteral("VehicleProfiles");
QString const AgentConfigurationXMLBaseModel::KeyVehicleProfile = QStringLiteral("VehicleProfile");
QString const AgentConfigurationXMLBaseModel::KeySystemConfig = QStringLiteral("SystemConfig");
QString const AgentConfigurationXMLBaseModel::KeyModel = QStringLiteral("Model");
QString const AgentConfigurationXMLBaseModel::KeySystem = QStringLiteral("System");
QString const AgentConfigurationXMLBaseModel::KeySystemProfiles = QStringLiteral("SystemProfiles");
QString const AgentConfigurationXMLBaseModel::KeySystemProfile = QStringLiteral("SystemProfile");
QString const AgentConfigurationXMLBaseModel::KeySystemFile = QStringLiteral("File");
QString const AgentConfigurationXMLBaseModel::KeySystemID = QStringLiteral("Id");
QString const AgentConfigurationXMLBaseModel::KeyVehicleModel = QStringLiteral("VehicleModel");
QString const AgentConfigurationXMLBaseModel::KeyVehicleModelCatalogue = QStringLiteral("VehicleModelCatalogue");

QString const AgentConfigurationXMLBaseModel::KeyParameterDouble = QStringLiteral("Double");
QString const AgentConfigurationXMLBaseModel::KeyParameterBool = QStringLiteral("Bool");
QString const AgentConfigurationXMLBaseModel::KeyParameterInt = QStringLiteral("Int");
QString const AgentConfigurationXMLBaseModel::KeyParameterFloat = QStringLiteral("Float");
QString const AgentConfigurationXMLBaseModel::KeyParameterString = QStringLiteral("String");

QList<QString> const AgentConfigurationXMLBaseModel::KeyListProfiles =
    {
        AgentConfigurationXMLBaseModel::KeyAgentProfiles,
        AgentConfigurationXMLBaseModel::KeyDriverProfiles,
        AgentConfigurationXMLBaseModel::KeyVehicleProfiles,
};

QList<QString> const AgentConfigurationXMLBaseModel::KeyListDynamicAgentProfile =
    {
        AgentConfigurationXMLBaseModel::KeyDriverProfiles,
        AgentConfigurationXMLBaseModel::KeyVehicleProfiles,
};

QList<QString> const AgentConfigurationXMLBaseModel::KeyListStaticAgentProfile =
    {
        AgentConfigurationXMLBaseModel::KeySystem,
        AgentConfigurationXMLBaseModel::KeyVehicleModel,
};

QList<QString> const AgentConfigurationXMLBaseModel::KeyListSystem =
    {
        AgentConfigurationXMLBaseModel::KeySystemFile,
        AgentConfigurationXMLBaseModel::KeySystemID,
};

QList<QString> const AgentConfigurationXMLBaseModel::KeyListVehicleProfile =
    {
        AgentConfigurationXMLBaseModel::KeySystemConfig,
        AgentConfigurationXMLBaseModel::KeyModel,
        AgentConfigurationXMLBaseModel::KeySystemProfiles,
};

QList<QString> const AgentConfigurationXMLBaseModel::KeyListVehicleModel =
    {
        AgentConfigurationXMLBaseModel::KeyVehicleModelCatalogue,
        AgentConfigurationXMLBaseModel::KeyModel,
};
