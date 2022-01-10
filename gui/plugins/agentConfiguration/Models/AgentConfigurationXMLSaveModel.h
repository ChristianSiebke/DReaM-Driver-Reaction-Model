/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef AGENTCONFIGURATIONXMLSAVEMODEL_H
#define AGENTCONFIGURATIONXMLSAVEMODEL_H

#include <QXmlStreamWriter>

#include "AgentConfigurationXMLBaseModel.h"
#include "openPASS-AgentConfiguration/AgentConfigurationInterface.h"

class AgentConfigurationXMLSaveModel : public AgentConfigurationXMLBaseModel
{
public:
    explicit AgentConfigurationXMLSaveModel() = delete;
    ~AgentConfigurationXMLSaveModel() = default;

public:
    static bool save(const QString &filepath,
                     const AgentConfigurationInterface *const agentConfiguration);

    static void saveAgentProfiles(QXmlStreamWriter &xml,
                                  AgentMapInterface *agentProfiles);

    static void saveAgentVehicleProfiles(QXmlStreamWriter &xml,
                                         AgentItemInterface::VehicleProfiles *vehicleProfiles);

    static void saveAgentDriverProfiles(QXmlStreamWriter &xml,
                                        AgentItemInterface::DriverProfiles *driverProfiles);

    static void saveVehicleProfiles(QXmlStreamWriter &xml,
                                    VehicleProfileMapInterface *vehicleProfiles);

    static void saveDriverProfiles(QXmlStreamWriter &xml,
                                   DriverProfileMapInterface *driverProfiles);

    static void saveDriverProfileParameters(QXmlStreamWriter &xml,
                                            DriverProfileItemInterface::Parameters *parameters);

    static void saveSystemProfile(QXmlStreamWriter &xml,
                                  VehicleProfileItemInterface::Systems *systems);
};

#endif // AGENTCONFIGURATIONXMLSAVEMODEL_H
