/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "XmlModelsConfig.h"

XmlModelsConfig::XmlModelsConfig(int startTime, QString endTime, int nInvocations, QString weekday,
                           int timeOfDay, int randomSeed):
    startTime(startTime), endTime(endTime), numberInvocations(nInvocations), weekday(weekday),
    timeOfDay(timeOfDay), randomSeed(randomSeed)
{}

XmlModelsConfig::~XmlModelsConfig()
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

void XmlModelsConfig::AddSpawnPoint(XmlSpawnPoint *sp)
{
    spawnPoints.push_back(sp);
}

void XmlModelsConfig::AddAgent(int id, int agentTypeRef, PCM_ParticipantData *participant)
{
    agents.push_back( XmlAgent(id, agentTypeRef, participant) );
}

void XmlModelsConfig::AddObservation(XmlObservation *observation)
{
    observations.push_back(observation);
}

bool XmlModelsConfig::WriteToXml(QXmlStreamWriter *xmlWriter)
{
    if (xmlWriter == nullptr)
    {
        return false;
    }

    QXmlStreamAttributes attrib;
    attrib.append("revMajor","1");
    attrib.append("revMinor","0");
    attrib.append("date","2020-01-01T00:00:00");
    attrib.append("description","openPASS vehicle models");
    attrib.append("author","openPASS");

    xmlWriter->writeStartElement("FileHeader");
    xmlWriter->writeAttributes(attrib);
    xmlWriter->writeEndElement(); // FileHeader

    xmlWriter->writeStartElement("Catalog");
    xmlWriter->writeAttribute("name","VehicleCatalog");
    for (XmlAgent agent : agents)
    {
        xmlWriter->writeStartElement("Vehicle");
        xmlWriter->writeAttribute("name","Agent_" + QString::number(agent.id));
        xmlWriter->writeAttribute("vehicleCategory", "car"); // always "car"
        xmlWriter->writeStartElement("Properties");
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","AirDragCoefficient");
        xmlWriter->writeAttribute("value","0.3");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","AxleRatio");
        xmlWriter->writeAttribute("value","1.0");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","DecelerationFromPowertrainDrag");
        xmlWriter->writeAttribute("value","0.5");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","FrictionCoefficient");
        xmlWriter->writeAttribute("value",agent.frictionCoeff);
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","FrontSurface");
        xmlWriter->writeAttribute("value","1.0");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","GearRatio1");
        xmlWriter->writeAttribute("value","1.0");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","Mass");
        xmlWriter->writeAttribute("value",agent.weight);
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","MaximumEngineSpeed");
        xmlWriter->writeAttribute("value","10000.0");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","MaximumEngineTorque");
        xmlWriter->writeAttribute("value","500.0");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","MinimumEngineSpeed");
        xmlWriter->writeAttribute("value","1.0");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","MinimumEngineTorque");
        xmlWriter->writeAttribute("value","-500.0");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","MomentInertiaPitch");
        xmlWriter->writeAttribute("value",agent.momentInertiaPitch);
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","MomentInertiaRoll");
        xmlWriter->writeAttribute("value",agent.momentInertiaRoll);
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","MomentInertiaYaw");
        xmlWriter->writeAttribute("value",agent.momentInertiaYaw);
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","NumberOfGears");
        xmlWriter->writeAttribute("value","1");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeStartElement("Property");
        xmlWriter->writeAttribute("name","SteeringRatio");
        xmlWriter->writeAttribute("value","1.0");
        xmlWriter->writeEndElement(); // Property
        xmlWriter->writeEndElement(); // Properties
        xmlWriter->writeStartElement("BoundingBox");
        xmlWriter->writeStartElement("Center");
        xmlWriter->writeAttribute("x", QString::number(agent.length.toDouble() - agent.distCOGtoLeadingEdge.toDouble()));
        xmlWriter->writeAttribute("y",QString::number(0.0));
        xmlWriter->writeAttribute("z",agent.heightCOG);
        xmlWriter->writeEndElement(); // Center
        xmlWriter->writeStartElement("Dimensions");
        xmlWriter->writeAttribute("width",agent.width);
        xmlWriter->writeAttribute("length",agent.length);
        xmlWriter->writeAttribute("height",QString::number(agent.heightCOG.toDouble()*2.0));
        xmlWriter->writeEndElement(); // Dimensions
        xmlWriter->writeEndElement(); // BoundingBox
        xmlWriter->writeStartElement("Performance");
        xmlWriter->writeAttribute("maxSpeed",QString::number(100.0));
        xmlWriter->writeAttribute("maxAcceleration",QString::number(10.0));
        xmlWriter->writeAttribute("maxDeceleration",QString::number(10.0));
        xmlWriter->writeEndElement(); // Performance
        xmlWriter->writeStartElement("Axles");
        xmlWriter->writeStartElement("FrontAxle");
        xmlWriter->writeAttribute("maxSteering",QString::number(1.0));
        xmlWriter->writeAttribute("wheelDiameter",QString::number(0.6));
        xmlWriter->writeAttribute("trackWidth",agent.trackWidth);
        xmlWriter->writeAttribute("positionX",agent.wheelbase);
        xmlWriter->writeAttribute("positionZ",QString::number(0.3));
        xmlWriter->writeEndElement(); // FrontAxle
        xmlWriter->writeStartElement("RearAxle");
        xmlWriter->writeAttribute("maxSteering",QString::number(0.0));
        xmlWriter->writeAttribute("wheelDiameter",QString::number(0.6));
        xmlWriter->writeAttribute("trackWidth",agent.trackWidth);
        xmlWriter->writeAttribute("positionX",QString::number(0.0));
        xmlWriter->writeAttribute("positionZ",QString::number(0.3));
        xmlWriter->writeEndElement(); // RearAxle
        xmlWriter->writeEndElement(); // Axles
        xmlWriter->writeEndElement(); // Vehicle
    }
    xmlWriter->writeEndElement(); // Catalog

    return true;
}
