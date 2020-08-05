/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "AgentConfigurationCombinationXMLSaveModel.h"
#include "AgentConfigurationXMLSaveModel.h"

#include <QRegularExpression>

bool AgentConfigurationCombinationXMLSaveModel::save(const QString &filepath,
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

        // Write root tag
        xml.writeStartElement(KeyRoot);
        xml.writeAttribute("SchemaVersion", "0.2.1");

        // Save agent profiles
        AgentConfigurationXMLSaveModel::saveAgentProfiles(xml, agentConfiguration->getAgents());

        // save driver profiles
        AgentConfigurationXMLSaveModel::saveDriverProfiles(xml, agentConfiguration->getDriverProfiles());

        // Save Vehicle Profiles
        saveVehicleProfiles(xml, agentConfiguration->getVehicleProfiles());

        // Save Vehicle Component Profiles
        saveVehicleComponentProfiles(xml, agentConfiguration->getVehicleComponentProfiles());

        // Save Sensor Profiles
        saveSensorProfiles(xml, agentConfiguration->getSensorProfiles());

        xml.writeEndElement();
        xml.writeEndDocument();

        return true;
    }


    return false;
}


void AgentConfigurationCombinationXMLSaveModel::saveVehicleProfiles(QXmlStreamWriter & xml,
                                                         VehicleProfileMapInterface *vehicleProfiles)
{
    xml.writeStartElement(KeyVehicleProfiles);

    for( auto profile : vehicleProfiles->values() )
    {
        xml.writeStartElement(KeyVehicleProfile);
        xml.writeAttribute(QString("Name"), profile->getName());

        xml.writeStartElement(KeyModel);
        xml.writeAttribute(QString("Name"), profile->getModel());
        xml.writeEndElement();

        saveComponents(xml, profile->getVehicleComponents());

        saveSensors(xml, profile->getSensors());

        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeComment("#############################################################################################");
}


void AgentConfigurationCombinationXMLSaveModel::saveComponents(QXmlStreamWriter &xml,
                                                    VehicleComponentMapInterface *components)
{
    xml.writeStartElement(KeyComponents);

    for(auto component : components->values())
    {
        xml.writeStartElement(KeyComponent);
        xml.writeAttribute(QString("Type"), component->getType());

        // 1. Save Profiles
        saveIncludedComponentProfiles(xml, component->getProfiles());

        // 2. Save Sensor Links
        saveSensorLinks(xml, component->getSensorLinks());

        xml.writeEndElement();
    }

    xml.writeEndElement();
}


void AgentConfigurationCombinationXMLSaveModel::saveIncludedComponentProfiles(QXmlStreamWriter &xml,
                                                           VehicleComponentItemInterface::Profiles * profiles)
{
    xml.writeStartElement(KeyProfiles);

    for(auto profile : profiles->keys() )
    {
        xml.writeStartElement(KeyProfile);

        xml.writeAttribute(QString("Name"), profile->getName());
        xml.writeAttribute(QString("Probability"), QString::number(profiles->value(profile)));

        xml.writeEndElement();
    }

    xml.writeEndElement();
}


void AgentConfigurationCombinationXMLSaveModel::saveSensorLinks(QXmlStreamWriter &xml,
                                                     VehicleComponentItemInterface::SensorLinks * sensorLinks)
{
    xml.writeStartElement(KeySensorLinks);

    for(auto link : sensorLinks->keys())
    {
        xml.writeStartElement(KeySensorLink);

        xml.writeAttribute(QString("SensorId"), QString::number(link));
        xml.writeAttribute(QString("InputId"), sensorLinks->value(link) );

        xml.writeEndElement();
    }

    xml.writeEndElement();
}


void AgentConfigurationCombinationXMLSaveModel::saveSensors(QXmlStreamWriter &xml,
                                                 SensorMapInterface *sensors)
{
    xml.writeStartElement(KeySensors);

    for(auto sensor : sensors->values())
    {
        xml.writeStartElement(KeySensor);
        xml.writeAttribute(QString("Id"), QString::number(sensor->getID()));

        saveSensorPosition(xml, sensor->getSensorPosition());

        xml.writeStartElement(KeyProfile);
        xml.writeAttribute(QString("Name"), sensor->getSensorProfile()->getName());
        xml.writeAttribute(QString("Type"), sensor->getSensorProfile()->getType());
        xml.writeEndElement();


        xml.writeEndElement();
    }

    xml.writeEndElement();
}

void AgentConfigurationCombinationXMLSaveModel::saveSensorPosition(QXmlStreamWriter &xml,
                                                        SensorPosition position)
{
     xml.writeStartElement(KeyPosition);
     xml.writeAttribute(QString("Name"), position.Name);

     xml.writeAttribute(QString("Longitudinal"), QString::number(position.longitudinal));
     xml.writeAttribute(QString("Lateral"), QString::number(position.lateral));
     xml.writeAttribute(QString("Height"), QString::number(position.height));

     xml.writeAttribute(QString("Pitch"), QString::number(position.pitch));
     xml.writeAttribute(QString("Yaw"), QString::number(position.yaw));
     xml.writeAttribute(QString("Roll"), QString::number(position.roll));


     xml.writeEndElement();
}


void AgentConfigurationCombinationXMLSaveModel::saveVehicleComponentProfiles(QXmlStreamWriter &xml,
                                                                  VehicleComponentProfileMapInterface *componentProfiles)
{
    xml.writeStartElement(KeyVehicleComponentProfiles);

    for(auto profile : componentProfiles->values())
    {
        xml.writeStartElement(KeyVehicleComponentProfile);
        xml.writeAttribute(QString("Name"), profile->getName());
        xml.writeAttribute(QString("Type"), profile->getType());

        saveVehicleComponentProfileParameters(xml, profile->getParameters());

        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeComment("#############################################################################################");
}


void AgentConfigurationCombinationXMLSaveModel::saveVehicleComponentProfileParameters(QXmlStreamWriter &xml,
                                                                           ParameterMapInterface *parameters)
{
    for(auto parameter : parameters->values())
    {
        QString Type = toLiteral(parameter->getType());
        xml.writeStartElement(Type);
        xml.writeAttribute(QString("Key"), parameter->getKey());

       /* if(Type == "Bool")
        {
            QString value = parameter->getValue() == "1" ? "true" : "false";
            xml.writeAttribute(QString("Value"), value);
        }*/
        if(Type == "NormalDistribution")
        {
            QString value = parameter->getValue();
            xml.writeAttribute(QString("Mean"), value.section(",",0,0));
            xml.writeAttribute(QString("SD"), value.section(",",1,1));
            xml.writeAttribute(QString("Min"), value.section(",",2,2));
            xml.writeAttribute(QString("Max"), value.section(",",3,3));
        }
        else
            xml.writeAttribute(QString("Value"), parameter->getValue());

        xml.writeEndElement();
    }
}

void AgentConfigurationCombinationXMLSaveModel::saveSensorProfiles(QXmlStreamWriter &xml,
                                                        SensorProfileMapInterface *sensorProfiles)
{
    xml.writeStartElement(KeySensorProfiles);

    for(auto profile : sensorProfiles->values())
    {
        xml.writeStartElement(KeySensorProfile);
        xml.writeAttribute(QString("Name"), profile->getName());
        xml.writeAttribute(QString("Type"), profile->getType());

        saveSensorProfileParameters(xml, profile->getParameters());

        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeComment("#############################################################################################");
}


void AgentConfigurationCombinationXMLSaveModel::saveSensorProfileParameters(QXmlStreamWriter &xml,
                                                                 ParameterMapInterface *parameters)
{
    for(auto parameter: parameters->values())
    {
        QString Type = toLiteral(parameter->getType());
        xml.writeStartElement(Type);
        xml.writeAttribute(QString("Key"), parameter->getKey());

        if(Type == "NormalDistribution")
        {
            QString value = parameter->getValue();
            xml.writeAttribute(QString("Mean"), value.section(",",0,0));
            xml.writeAttribute(QString("SD"), value.section(",",1,1));
            xml.writeAttribute(QString("Min"), value.section(",",2,2));
            xml.writeAttribute(QString("Max"), value.section(",",3,3));
        }
        else
            xml.writeAttribute(QString("Value"), parameter->getValue());

        xml.writeEndElement();
    }
}

QString AgentConfigurationCombinationXMLSaveModel::toLiteral(QString string)
{
    // Replace ä, ö, ü with ae, oe, ue
    string.replace(QStringLiteral("ä"), QStringLiteral("ae"));
    string.replace(QStringLiteral("ö"), QStringLiteral("oe"));
    string.replace(QStringLiteral("ü"), QStringLiteral("ue"));

    // Replace Ä, Ö, Ü with AE, OE, UE
    string.replace(QStringLiteral("Ä"), QStringLiteral("AE"));
    string.replace(QStringLiteral("Ö"), QStringLiteral("OE"));
    string.replace(QStringLiteral("Ü"), QStringLiteral("UE"));

    // Split string in single words
    QStringList parts = string.split(
                QRegularExpression(QStringLiteral("[\\W_]+")),
                QString::SkipEmptyParts);

    // Upper first character of each word
    for (QString & part : parts)
    {
        part.replace(0, 1, part[0].toUpper());
    }

    // Join words to literal
    return parts.join(QStringLiteral(""));
}
