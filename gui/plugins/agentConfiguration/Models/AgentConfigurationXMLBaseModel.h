/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef AGENTCONFIGURATIONXMLBASEMODEL_H
#define AGENTCONFIGURATIONXMLBASEMODEL_H

#include <QList>
#include <QString>

class AgentConfigurationXMLBaseModel
{
public:
    explicit AgentConfigurationXMLBaseModel() = delete;
    virtual ~AgentConfigurationXMLBaseModel() = default;

protected:
    static QString const KeyRoot;

protected:
    static QString const KeyAgentProfiles;
    static QString const KeyAgentProfile;

protected:
    static QString const KeyDriverProfiles;
    static QString const KeyDriverProfile;

protected:
    static QString const KeyVehicleProfiles;
    static QString const KeyVehicleProfile;
    static QString const KeySystemConfig;
    static QString const KeyModel;
    static QString const KeySystems;
    static QString const KeySystem;
    static QString const KeySystemFile;
    static QString const KeySystemID;
    static QString const KeySystemProfiles;
    static QString const KeySystemProfile;
    static QString const KeyVehicleModel;
    static QString const KeyVehicleModelCatalogue;

protected:
    static QList<QString> const KeyListProfiles;
    static QList<QString> const KeyListSystem;
    static QList<QString> const KeyListDynamicAgentProfile;
    static QList<QString> const KeyListStaticAgentProfile;
    static QList<QString> const KeyListVehicleProfile;
    static QList<QString> const KeyListVehicleModel;

protected:
    static QString const KeyParameterDouble;
    static QString const KeyParameterBool;
    static QString const KeyParameterInt;
    static QString const KeyParameterFloat;
    static QString const KeyParameterString;
};

#endif // AGENTCONFIGURATIONXMLBASEMODEL_H
