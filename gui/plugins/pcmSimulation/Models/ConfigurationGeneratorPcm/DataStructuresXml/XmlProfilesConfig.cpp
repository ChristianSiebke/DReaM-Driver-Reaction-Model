/*********************************************************************
* Copyright (c) 2017, 2018, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "XmlProfilesConfig.h"

XmlProfilesConfig::XmlProfilesConfig(int startTime, QString endTime, int nInvocations, QString weekday,
                                     int timeOfDay, int randomSeed):
    startTime(startTime), endTime(endTime), numberInvocations(nInvocations), weekday(weekday),
    timeOfDay(timeOfDay), randomSeed(randomSeed)
{}

XmlProfilesConfig::~XmlProfilesConfig()
{
    for ( XmlSpawnPoint *sp : spawnPoints)
    {
        delete sp;
    }

    for ( XmlObservation *observation : observations)
    {
        delete observation;
    }
}

void XmlProfilesConfig::AddSpawnPoint(XmlSpawnPoint *sp)
{
    spawnPoints.push_back(sp);
}

void XmlProfilesConfig::AddAgent(int id, int agentTypeRef, PCM_ParticipantData participant)
{
    agents.push_back( XmlAgent(id, agentTypeRef, participant) );
}

void XmlProfilesConfig::AddModel(int id, int agentTypeRef, PCM_ParticipantData participant)
{
    models.push_back( XmlModel(id, agentTypeRef, participant) );
}

void XmlProfilesConfig::AddObservation(XmlObservation *observation)
{
    observations.push_back(observation);
}

bool XmlProfilesConfig::WriteToXml(QXmlStreamWriter *xmlWriter)
{



    xmlWriter->writeStartElement("AgentProfiles");
    int ia=0;
    for (XmlAgent agent : agents)
    {
        xmlWriter->writeStartElement("AgentProfile");
        xmlWriter->writeAttribute("Name","Agent_" + QString::number(agent._id));
        xmlWriter->writeAttribute("Type","Static");
        xmlWriter->writeStartElement("System");
        xmlWriter->writeTextElement("File","SystemConfig.xml");
        xmlWriter->writeTextElement("Id",QString::number(agent._id));
        xmlWriter->writeEndElement(); // System
        xmlWriter->writeTextElement("VehicleModel",models[ia].modelName);
        xmlWriter->writeEndElement(); // AgentProfile
        ia++;
    }
    xmlWriter->writeEndElement(); // AgentProfiles

    xmlWriter->writeStartElement("VehicleProfiles");
    for (XmlModel model : models)
    {
        xmlWriter->writeStartElement("VehicleProfile");
        xmlWriter->writeAttribute("Name",model.modelName);
        xmlWriter->writeStartElement("Model");
        xmlWriter->writeAttribute("Name",model.modelName);
        xmlWriter->writeEndElement(); // Model
        xmlWriter->writeStartElement("Components");
        xmlWriter->writeEndElement(); // Components
        xmlWriter->writeStartElement("Sensors");
        xmlWriter->writeEndElement(); // Sensors
        xmlWriter->writeEndElement(); // VehicleProfile
    }
    xmlWriter->writeEndElement(); // VehicleProfiles

    xmlWriter->writeStartElement("ProfileGroup");
    xmlWriter->writeAttribute("Type","Driver");
    xmlWriter->writeStartElement("Profile");
    xmlWriter->writeAttribute("Name","Regular");
    xmlWriter->writeEndElement(); // Profile
    xmlWriter->writeEndElement(); // ProfileGroup

    return true;
}
