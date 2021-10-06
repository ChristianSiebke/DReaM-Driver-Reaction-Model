/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef AGENTCONFIGURATIONCOMBINATIONXMLSAVEMODEL_H
#define AGENTCONFIGURATIONCOMBINATIONXMLSAVEMODEL_H

#include <QFile>
#include <QString>
#include <QXmlStreamWriter>

#include "AgentConfigurationCombinationXMLBaseModel.h"
#include "openPASS-AgentConfiguration/AgentConfigurationInterface.h"

class AgentConfigurationCombinationXMLSaveModel : public AgentConfigurationCombinationXMLBaseModel
{
public:
    explicit AgentConfigurationCombinationXMLSaveModel() = delete;
    ~AgentConfigurationCombinationXMLSaveModel() = default;

public:
    static bool save(const QString &filepath,
                     const AgentConfigurationInterface *const agentConfiguration);

    static void saveVehicleProfiles(QXmlStreamWriter &xml,
                                    VehicleProfileMapInterface *vehicleProfiles);

    static void saveComponents(QXmlStreamWriter &xml,
                               VehicleComponentMapInterface *components);

    static void saveIncludedComponentProfiles(QXmlStreamWriter &xml,
                                              VehicleComponentItemInterface::Profiles *profiles);

    static void saveSensorLinks(QXmlStreamWriter &xml,
                                VehicleComponentItemInterface::SensorLinks *sensorLinks);

    static void saveSensors(QXmlStreamWriter &xml,
                            SensorMapInterface *sensors);

    static void saveSensorPosition(QXmlStreamWriter &xml,
                                   SensorPosition position);

public:
    static void saveVehicleComponentProfiles(QXmlStreamWriter &xml,
                                             VehicleComponentProfileMapInterface *componentProfiles);

    static void saveVehicleComponentProfileParameters(QXmlStreamWriter &xml,
                                                      ParameterMapInterface *parameters);

    static void saveSensorProfiles(QXmlStreamWriter &xml,
                                   SensorProfileMapInterface *sensorProfiles);

    static void saveSensorProfileParameters(QXmlStreamWriter &xml,
                                            ParameterMapInterface *parameters);

private:
    static QString toLiteral(QString string);
};

#endif // AGENTCONFIGURATIONCOMBINATIONXMLSAVEMODEL_H
