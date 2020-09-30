/*********************************************************************
* Copyright (c) 2017, 2018, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "XoscScenario.h"

bool XoscScenario::WriteToXml(QXmlStreamWriter *xmlWriter)
{

    QXmlStreamAttributes attrib;
    attrib.append("revMajor","1");
    attrib.append("revMinor","0");
    attrib.append("date","2020-01-01T00:00:00");
    attrib.append("description","openPASS default scenario");
    attrib.append("author","openPASS");

    xmlWriter->writeStartElement("FileHeader");
    xmlWriter->writeAttributes(attrib);
    xmlWriter->writeEndElement(); // FileHeader

    xmlWriter->writeStartElement("ParameterDeclarations");
    xmlWriter->writeStartElement("ParameterDeclaration");
    xmlWriter->writeAttribute("name","OP_OSC_SchemaVersion");
    xmlWriter->writeAttribute("parameterType", "string");
    xmlWriter->writeAttribute("value", "0.4.0");
    xmlWriter->writeEndElement(); // ParameterDeclaration
    xmlWriter->writeEndElement(); // ParameterDeclarations

    xmlWriter->writeStartElement("CatalogLocations");
    xmlWriter->writeStartElement("VehicleCatalog");
    xmlWriter->writeStartElement("Directory");
    xmlWriter->writeAttribute("path","VehicleModelsCatalog.xosc");
    xmlWriter->writeEndElement(); // Directory
    xmlWriter->writeEndElement(); // VehicleCatalog
    xmlWriter->writeStartElement("PedestrianCatalog");
    xmlWriter->writeStartElement("Directory");
    xmlWriter->writeAttribute("path","");
    xmlWriter->writeEndElement(); // Directory
    xmlWriter->writeEndElement(); // PedestrianCatalog
    xmlWriter->writeStartElement("ControllerCatalog");
    xmlWriter->writeStartElement("Directory");
    xmlWriter->writeAttribute("path","");
    xmlWriter->writeEndElement(); // Directory
    xmlWriter->writeEndElement(); // ControllerCatalog
    xmlWriter->writeStartElement("ManeuverCatalog");
    xmlWriter->writeStartElement("Directory");
    xmlWriter->writeAttribute("path","");
    xmlWriter->writeEndElement(); // Directory
    xmlWriter->writeEndElement(); // ManeuverCatalog
    xmlWriter->writeStartElement("MiscObjectCatalog");
    xmlWriter->writeStartElement("Directory");
    xmlWriter->writeAttribute("path","");
    xmlWriter->writeEndElement(); // Directory
    xmlWriter->writeEndElement(); // MiscObjectCatalog
    xmlWriter->writeStartElement("EnvironmentCatalog");
    xmlWriter->writeStartElement("Directory");
    xmlWriter->writeAttribute("path","");
    xmlWriter->writeEndElement(); // Directory
    xmlWriter->writeEndElement(); // EnvironmentCatalog
    xmlWriter->writeStartElement("TrajectoryCatalog");
    xmlWriter->writeStartElement("Directory");
    xmlWriter->writeAttribute("path","");
    xmlWriter->writeEndElement(); // Directory
    xmlWriter->writeEndElement(); // TrajectoryCatalog
    xmlWriter->writeStartElement("RouteCatalog");
    xmlWriter->writeStartElement("Directory");
    xmlWriter->writeAttribute("path","");
    xmlWriter->writeEndElement(); // Directory
    xmlWriter->writeEndElement(); // RouteCatalog
    xmlWriter->writeEndElement(); // CatalogLocations




    xmlWriter->writeStartElement("RoadNetwork");
    xmlWriter->writeStartElement("LogicFile");
    xmlWriter->writeAttribute("filepath","SceneryConfiguration.xodr");
    xmlWriter->writeEndElement(); // LogicFile
    xmlWriter->writeStartElement("SceneGraphFile");
    xmlWriter->writeAttribute("filepath","");
    xmlWriter->writeEndElement(); // SceneGraphFile
    xmlWriter->writeEndElement(); // RoadNetwork




    xmlWriter->writeStartElement("Entities");
    for (XoscTrajectory trajectory : trajectories)
    {
        xmlWriter->writeStartElement("ScenarioObject");
        xmlWriter->writeAttribute("name","Agent_" + QString::number(trajectory.agentId));
        xmlWriter->writeStartElement("CatalogReference");
        xmlWriter->writeAttribute("catalogName","ProfilesCatalog.xml");
        xmlWriter->writeAttribute("entryName","Agent_" + QString::number(trajectory.agentId));
        xmlWriter->writeEndElement(); // CatalogReference
        xmlWriter->writeEndElement(); // ScenarioObject
    }
    xmlWriter->writeEndElement(); // Entities

    xmlWriter->writeStartElement("Storyboard");
    xmlWriter->writeStartElement("Init");
    xmlWriter->writeStartElement("Actions");

    for (XoscTrajectory trajectory : trajectories)
    {
        xmlWriter->writeStartElement("Private");
        xmlWriter->writeAttribute("entityRef", "Agent_" + QString::number(trajectory.agentId));

        xmlWriter->writeStartElement("PrivateAction");
        xmlWriter->writeStartElement("TeleportAction");
        xmlWriter->writeStartElement("Position");
        xmlWriter->writeStartElement("WorldPosition");
        xmlWriter->writeAttribute("x", QString::number(trajectory.trajectory->GetXPosVec()->at(0), 'g', 10));
        xmlWriter->writeAttribute("y", QString::number(trajectory.trajectory->GetYPosVec()->at(0), 'g', 10));
        xmlWriter->writeAttribute("z", "0");
        xmlWriter->writeAttribute("h", QString::number(trajectory.trajectory->GetPsiVec()->at(0), 'g', 10));
        xmlWriter->writeAttribute("p", "0");
        xmlWriter->writeAttribute("r", "0");
        xmlWriter->writeEndElement(); // WorldPosition
        xmlWriter->writeEndElement(); // Position
        xmlWriter->writeEndElement(); // TeleportAction
        xmlWriter->writeEndElement(); // PrivateAction

        xmlWriter->writeStartElement("PrivateAction");
        xmlWriter->writeStartElement("LongitudinalAction");
        xmlWriter->writeStartElement("SpeedAction");
        xmlWriter->writeStartElement("SpeedActionDynamics");
        xmlWriter->writeAttribute("dynamicsShape", "linear");
        xmlWriter->writeAttribute("value", "0.0");
        xmlWriter->writeAttribute("dynamicsDimension", "rate");
        xmlWriter->writeEndElement(); // SpeedActionDynamics
        xmlWriter->writeStartElement("SpeedActionTarget");
        xmlWriter->writeStartElement("AbsoluteTargetSpeed");
        xmlWriter->writeAttribute("value", QString::number(trajectory.trajectory->GetStartVelocity(), 'g', 10));
        xmlWriter->writeEndElement(); // AbsoluteTargetSpeed
        xmlWriter->writeEndElement(); // SpeedActionTarget
        xmlWriter->writeEndElement(); // SpeedAction
        xmlWriter->writeEndElement(); // LongitudinalAction
        xmlWriter->writeEndElement(); // PrivateAction

        xmlWriter->writeEndElement(); // Private
    }
    xmlWriter->writeEndElement(); // Actions
    xmlWriter->writeEndElement(); // Init


    xmlWriter->writeStartElement("Story");
    xmlWriter->writeAttribute("name","TrajectoryStory");

    for (XoscTrajectory trajectory: trajectories)
    {
        trajectory.WriteToXml(xmlWriter);
    }
//    trajectories[0].WriteToXml(xmlWriter);
    xmlWriter->writeEndElement(); // Story


    xmlWriter->writeStartElement("StopTrigger");
    xmlWriter->writeStartElement("ConditionGroup");
    xmlWriter->writeStartElement("Condition");
    xmlWriter->writeAttribute("name","EndTime");
    xmlWriter->writeAttribute("delay","0");
    xmlWriter->writeAttribute("conditionEdge","rising");
    xmlWriter->writeStartElement("ByValueCondition");
    xmlWriter->writeStartElement("SimulationTimeCondition");
    xmlWriter->writeAttribute("value",QString::number((double)trajectories[0].trajectory->GetEndTime()/1000.0*2));
    xmlWriter->writeAttribute("rule","greaterThan");
    xmlWriter->writeEndElement(); // SimulationTimeCondition
    xmlWriter->writeEndElement(); // ByValueCondition
    xmlWriter->writeEndElement(); // Condition
    xmlWriter->writeEndElement(); // ConditionGroup
    xmlWriter->writeEndElement(); // StopTrigger


    xmlWriter->writeEndElement(); // Storyboard

    return true;
}

void XoscScenario::AddTrajectory(int agentId,
                                 PCM_Trajectory *trajectory)
{
    trajectories.push_back(XoscTrajectory(agentId, trajectory));
}

void XoscScenario::AddAgent(int id, int agentTypeRef, PCM_ParticipantData participant)
{
    agents.push_back( XmlAgent(id, agentTypeRef, participant) );
}

void XoscScenario::ReReferenceTrajectory(std::vector<PCM_ParticipantData> &participants)
{
    for (XoscTrajectory &traj: trajectories)
    {
        traj.trajectory->ReReference(participants[traj.agentId]);
    }
}

