/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "AgentConfigurationXMLSaveModel.h"

#include <QFile>

bool AgentConfigurationXMLSaveModel::save(const QString &filepath,
                                          const AgentConfigurationInterface * const agentConfiguration)
{
    QFile file(filepath);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QXmlStreamWriter xml(&file);

        xml.setAutoFormatting(true);
        xml.setAutoFormattingIndent(2);

        // Start Document
        xml.writeStartDocument();

        // Write root element
        xml.writeStartElement(KeyRoot);

        // Save Agent Profiles
        saveAgentProfiles(xml, agentConfiguration->getAgents());

        // Save Driver profiles
        saveDriverProfiles(xml, agentConfiguration->getDriverProfiles());

        // Save Vehicle Profiles
        saveVehicleProfiles(xml, agentConfiguration->getVehicleProfiles());

        xml.writeEndElement();
        xml.writeEndDocument();

        return true;
    }


    return false;
}

void AgentConfigurationXMLSaveModel::saveAgentProfiles(QXmlStreamWriter &xml,
                                                       AgentMapInterface *agentProfiles)
{
    xml.writeStartElement(KeyAgentProfiles);

    // save vehicle profiles
    for(auto profile : agentProfiles->values())
    {
        xml.writeStartElement(KeyAgentProfile);
        xml.writeAttribute("Name", profile->getName());
        xml.writeAttribute("Type", profile->getType());

        if(profile->getType() == "Dynamic")
        {
            saveAgentDriverProfiles(xml, profile->getDriverProfiles());
            saveAgentVehicleProfiles(xml, profile->getVehicleProfiles());
        }
        if(profile->getType() == "Static")
        {
            xml.writeStartElement(KeySystem);
            xml.writeTextElement(KeySystemFile, profile->getSystemConfig());
            xml.writeTextElement(KeySystemID, QString::number(profile->getSystemID()));
            xml.writeEndElement();

            xml.writeTextElement(KeyVehicleModel, profile->getVehicleModel());
        }
        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeComment("#############################################################################################");
}

void AgentConfigurationXMLSaveModel::saveAgentVehicleProfiles(QXmlStreamWriter & xml, AgentItemInterface::VehicleProfiles *vehicleProfiles)
{
    xml.writeStartElement(KeyVehicleProfiles);

    for(auto profile : vehicleProfiles->keys())
    {
        xml.writeStartElement(KeyVehicleProfile);
        xml.writeAttribute("Name", profile);
        xml.writeAttribute("Probability", QString::number((*vehicleProfiles)[profile]));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void AgentConfigurationXMLSaveModel::saveAgentDriverProfiles(QXmlStreamWriter & xml, AgentItemInterface::DriverProfiles *driverProfiles)
{
    xml.writeStartElement(KeyDriverProfiles);

    for(auto profile : driverProfiles->keys())
    {
        xml.writeStartElement(KeyDriverProfile);
        xml.writeAttribute("Name", profile);
        xml.writeAttribute("Probability", QString::number((*driverProfiles)[profile]));
        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void AgentConfigurationXMLSaveModel::saveVehicleProfiles(QXmlStreamWriter & xml,
                                                         VehicleProfileMapInterface *vehicleProfiles)
{
    xml.writeStartElement(KeyVehicleProfiles);

    for( auto profile : vehicleProfiles->values() )
    {
        xml.writeStartElement(KeyVehicleProfile);
        xml.writeAttribute(QString("Name"), profile->getName());

        xml.writeStartElement(KeyModel);
        xml.writeAttribute("Catalogue", profile->getModelCatalogue());
        xml.writeAttribute(QString("Name"), profile->getModel());
        xml.writeEndElement();

        xml.writeStartElement(KeySystemConfig);
        xml.writeAttribute(QString("Path"), profile->getSystemConfig());
        xml.writeEndElement();

        xml.writeStartElement(KeySystemProfiles);
        for(auto systems : profile->getSystemProfiles()->values())
        {
            xml.writeStartElement(KeySystemProfile);
            xml.writeAttribute(QString("Type"), profile->getSystemProfiles()->key(systems));
            saveSystemProfile(xml, systems);
            xml.writeEndElement();
        }
        xml.writeEndElement();

        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeComment("#############################################################################################");
}

void AgentConfigurationXMLSaveModel::saveSystemProfile(QXmlStreamWriter &xml,
                                                        VehicleProfileItemInterface::Systems *systems)
{
    for(auto system : systems->keys())
    {
        xml.writeStartElement(KeySystem);
        xml.writeAttribute(QString("Id"), QString::number(system.first));
        xml.writeAttribute(QString("Title"), system.second);
        xml.writeAttribute(QString("Probability"), QString::number(systems->value(system)));
        xml.writeEndElement();
    }
}

void AgentConfigurationXMLSaveModel::saveDriverProfiles(QXmlStreamWriter &xml, DriverProfileMapInterface *driverProfiles)
{
    xml.writeStartElement(KeyDriverProfiles);

    for(auto profile : driverProfiles->values())
    {
        xml.writeStartElement(KeyDriverProfile);
        xml.writeAttribute("Name", profile->getName());
        saveDriverProfileParameters(xml, profile->getParameters() );
        xml.writeEndElement();
    }
    xml.writeEndElement();
    xml.writeComment("#############################################################################################");
}

void AgentConfigurationXMLSaveModel::saveDriverProfileParameters(QXmlStreamWriter &xml,
                                                                 DriverProfileItemInterface::Parameters *parameters)
{
    // TODO: add units
    for(auto parameter : parameters->values())
    {
        xml.writeStartElement(parameter->getType());
        xml.writeAttribute("Key", parameter->getKey() );
        xml.writeAttribute("Value", parameter->getValue());
        xml.writeEndElement();
    }
}

