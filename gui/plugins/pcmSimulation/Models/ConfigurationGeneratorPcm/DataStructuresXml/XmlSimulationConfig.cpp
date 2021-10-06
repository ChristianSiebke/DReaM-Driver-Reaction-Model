/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "XmlSimulationConfig.h"

XmlSimulationConfig::XmlSimulationConfig(int startTime, QString endTime, int nInvocations, QString weekday,
                           int timeOfDay, int randomSeed):
    startTime(startTime), endTime(endTime), numberInvocations(nInvocations), weekday(weekday),
    timeOfDay(timeOfDay), randomSeed(randomSeed)
{}

XmlSimulationConfig::~XmlSimulationConfig()
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

void XmlSimulationConfig::AddSpawnPoint(XmlSpawnPoint *sp)
{
    spawnPoints.push_back(sp);
}

void XmlSimulationConfig::AddAgent(int id, int agentTypeRef, PCM_ParticipantData *participant)
{
    agents.push_back( XmlAgent(id, agentTypeRef, participant) );
}

void XmlSimulationConfig::AddObservation(XmlObservation *observation)
{
    observations.push_back(observation);
}

bool XmlSimulationConfig::WriteToXml(QXmlStreamWriter *xmlWriter)
{
    if (xmlWriter == nullptr)
    {
        return false;
    }

    xmlWriter->writeTextElement( "ProfilesCatalog", "ProfilesCatalog.xml");

    xmlWriter->writeStartElement("Experiment");
    xmlWriter->writeTextElement("ExperimentID", QString::number(0));
    xmlWriter->writeTextElement("NumberOfInvocations",QString::number(numberInvocations));
    xmlWriter->writeTextElement("RandomSeed",QString::number(randomSeed));
    xmlWriter->writeStartElement("Libraries");
    xmlWriter->writeTextElement("WorldLibrary", worldLibrary);
    xmlWriter->writeEndElement(); // Libraries
    xmlWriter->writeEndElement(); // Experiment

    xmlWriter->writeStartElement("Scenario");
    xmlWriter->writeTextElement("OpenScenarioFile", scenarioFile);
    xmlWriter->writeEndElement(); // Scenario

    xmlWriter->writeStartElement("Environment");
    xmlWriter->writeStartElement("TimeOfDays");
    xmlWriter->writeStartElement("TimeOfDay");
    xmlWriter->writeAttribute("Probability","1");
    xmlWriter->writeAttribute("Value","15");
    xmlWriter->writeEndElement(); // TimeOfDay
    xmlWriter->writeEndElement(); // TimeOfDays
    xmlWriter->writeStartElement("VisibilityDistances");
    xmlWriter->writeStartElement("VisibilityDistance");
    xmlWriter->writeAttribute("Probability","1");
    xmlWriter->writeAttribute("Value","400");
    xmlWriter->writeEndElement(); // VisibilityDistance
    xmlWriter->writeEndElement(); // VisibilityDistances
    xmlWriter->writeStartElement("Frictions");
    xmlWriter->writeStartElement("Friction");
    xmlWriter->writeAttribute("Probability","1");
    xmlWriter->writeAttribute("Value","1.0");
    xmlWriter->writeEndElement(); // Friction
    xmlWriter->writeEndElement(); // Frictions
    xmlWriter->writeStartElement("Weathers");
    xmlWriter->writeStartElement("Weather");
    xmlWriter->writeAttribute("Probability","1");
    xmlWriter->writeAttribute("Value","Clear");
    xmlWriter->writeEndElement(); // Weather
    xmlWriter->writeEndElement(); // Weathers
    xmlWriter->writeTextElement("TrafficRules", "Germany");
    xmlWriter->writeEndElement(); // Environment


    xmlWriter->writeStartElement("Observations");
    xmlWriter->writeStartElement("Observation");
    xmlWriter->writeTextElement("Library","Observation_LogAgent");
    xmlWriter->writeStartElement("Parameters");
    xmlWriter->writeStartElement("String");
    xmlWriter->writeAttribute("Key","OutputFilename");
    xmlWriter->writeAttribute("Value","simulationOutput.xml");
    xmlWriter->writeEndElement(); // String
    xmlWriter->writeStartElement("Bool");
    xmlWriter->writeAttribute("Key","LoggingCyclicsToCsv");
    xmlWriter->writeAttribute("Value","true");
    xmlWriter->writeEndElement(); // Bool
    xmlWriter->writeStartElement("StringVector");
    xmlWriter->writeAttribute("Key","LoggingGroup_Trace");
    xmlWriter->writeAttribute("Value","XPosition,YPosition,YawAngle");
    xmlWriter->writeEndElement(); // StringVector
    xmlWriter->writeStartElement("StringVector");
    xmlWriter->writeAttribute("Key","LoggingGroup_RoadPosition");
    xmlWriter->writeAttribute("Value","AgentInFront,Lane,PositionRoute,Road,TCoordinate");
    xmlWriter->writeEndElement(); // StringVector
    xmlWriter->writeStartElement("StringVector");
    xmlWriter->writeAttribute("Key","LoggingGroup_RoadPositionExtended");
    xmlWriter->writeAttribute("Value","SecondaryLanes");
    xmlWriter->writeEndElement(); // StringVector
    xmlWriter->writeStartElement("StringVector");
    xmlWriter->writeAttribute("Key","LoggingGroup_Sensor");
    xmlWriter->writeAttribute("Value","Sensor*_DetectedAgents,Sensor*_VisibleAgents");
    xmlWriter->writeEndElement(); // StringVector
    xmlWriter->writeStartElement("StringVector");
    xmlWriter->writeAttribute("Key","LoggingGroup_Vehicle");
    xmlWriter->writeAttribute("Value","AccelerationPedalPosition,BrakePedalPosition,EngineMoment,Gear,SteeringAngle,TotalDistanceTraveled,YawRate");
    xmlWriter->writeEndElement(); // StringVector
    xmlWriter->writeStartElement("StringVector");
    xmlWriter->writeAttribute("Key","LoggingGroup_Visualization");
    xmlWriter->writeAttribute("Value","AccelerationEgo,BrakeLight,IndicatorState,LightStatus,VelocityEgo");
    xmlWriter->writeEndElement(); // StringVector
    xmlWriter->writeStartElement("StringVector");
    xmlWriter->writeAttribute("Key","LoggingGroups");
    xmlWriter->writeAttribute("Value","Trace,Visualization");
    xmlWriter->writeEndElement(); // StringVector
    xmlWriter->writeEndElement(); // Parameters
    xmlWriter->writeEndElement(); // Observation
    xmlWriter->writeEndElement(); // Observations


    xmlWriter->writeStartElement("Spawners");
    xmlWriter->writeStartElement("Spawner");
    xmlWriter->writeTextElement("Library","SpawnerScenario");
    xmlWriter->writeTextElement("Type","PreRun");
    xmlWriter->writeTextElement("Priority","1");
    xmlWriter->writeEndElement(); // Spawner
    xmlWriter->writeEndElement(); // Spawners


    return true;
}
