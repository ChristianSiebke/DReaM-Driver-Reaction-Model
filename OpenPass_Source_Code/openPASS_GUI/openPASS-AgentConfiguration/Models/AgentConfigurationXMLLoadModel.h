/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef AGENTCONFIGURATIONXMLLOADMODEL_H
#define AGENTCONFIGURATIONXMLLOADMODEL_H

#include "openPASS-AgentConfiguration/AgentConfigurationInterface.h"

#include "Models/AgentConfigurationXMLBaseModel.h"

#include <QXmlStreamReader>

class AgentConfigurationXMLLoadModel : public AgentConfigurationXMLBaseModel
{

public:
    explicit AgentConfigurationXMLLoadModel() = delete;
    ~AgentConfigurationXMLLoadModel() = default;

public:
    static bool load(const QString &filepath,
                     const AgentConfigurationInterface * const agentConfiguration);

    static bool loadAgentProfiles(QXmlStreamReader & xml,
                                  AgentMapInterface * agentProfiles);

    static bool loadAgentProfile(QXmlStreamReader & xml,
                                 AgentMapInterface *agentProfiles);

    static bool loadAgentVehicleProfiles(QXmlStreamReader & xml,
                                         AgentItemInterface::VehicleProfiles *vehicleProfiles);

    static bool loadAgentDriverProfiles(QXmlStreamReader & xml,
                                         AgentItemInterface::DriverProfiles *driverProfiles);

    static bool loadVehicleProfiles(QXmlStreamReader & xml,
                                    VehicleProfileMapInterface *vehicleProfiles);

    static bool loadVehicleProfile(QXmlStreamReader &xml,
                                   VehicleProfileItemInterface *profile);

    static bool loadDriverProfiles(QXmlStreamReader & xml,
                                   DriverProfileMapInterface *driverProfiles);

    static bool loadDriverProfileParameters(QXmlStreamReader &xml,
                                            DriverProfileItemInterface::Parameters *parameters);

    static bool loadAgentSystem(QXmlStreamReader & xml, AgentItemInterface *agent);

    static bool loadAgentVehicleModel(QXmlStreamReader &xml, AgentItemInterface *agent);

    static bool loadSystemProfiles(QXmlStreamReader & xml,
                                    VehicleProfileItemInterface::SystemProfiles * systemProfiles);

    static bool loadSystemProfile(QXmlStreamReader & xml,
                                  VehicleProfileItemInterface::Systems * systems);

    static bool loadTitlesfromSystemConfig(AgentItemInterface::SystemConfig,
                                            QMap<AgentItemInterface::SystemID, AgentItemInterface::SystemTitle> &Titles);

};

#endif // AGENTCONFIGURATIONXMLLOADMODEL_H
