/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef AGENTCONFIGURATIONCOMBINATIONXMLBASEMODEL_H
#define AGENTCONFIGURATIONCOMBINATIONXMLBASEMODEL_H

#include <QString>

class AgentConfigurationCombinationXMLBaseModel
{

public:
    explicit AgentConfigurationCombinationXMLBaseModel() = delete;
    ~AgentConfigurationCombinationXMLBaseModel() =default;

protected:
    static QString const KeyRoot;

protected:
    static QString const KeyVehicleProfiles;
    static QString const KeyVehicleProfile;
    static QString const KeyModel;
    static QString const KeyComponents;
    static QString const KeyComponent;
    static QString const KeyProfiles;
    static QString const KeyProfile;
    static QString const KeySensorLinks;
    static QString const KeySensorLink;
    static QString const KeySensors;
    static QString const KeySensor;
    static QString const KeyPosition;

protected:
    static QString const KeyVehicleComponentProfiles;
    static QString const KeyVehicleComponentProfile;

protected:
    static QString const KeySensorProfiles;
    static QString const KeySensorProfile;

protected:
    static QString const KeyParameterDouble;
    static QString const KeyParameterBool;
    static QString const KeyParameterInt;
    static QString const KeyParameterFloat;
    static QString const KeyParameterString;

};

#endif // AGENTCONFIGURATIONCOMBINATIONXMLBASEMODEL_H
