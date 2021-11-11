/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "XoscTrajectory.h"

XoscTrajectory::XoscTrajectory(int agentId,
                               const PCM_Trajectory *trajectory)
{
    this->agentId = agentId;
    this->trajectory = trajectory;
}

bool XoscTrajectory::WriteToXml(QXmlStreamWriter *xmlWriter)
{

    xmlWriter->writeStartElement("Act");
    xmlWriter->writeAttribute("name","Act_" + QString::number(agentId));
    xmlWriter->writeStartElement("ManeuverGroup");
    xmlWriter->writeAttribute("maximumExecutionCount","1");
    xmlWriter->writeAttribute("name","TrajectorySequence");
    xmlWriter->writeStartElement("Actors");
    xmlWriter->writeAttribute("selectTriggeringEntities","false");
    xmlWriter->writeStartElement("EntityRef");
    xmlWriter->writeAttribute("entityRef","Agent_" + QString::number(agentId));
    xmlWriter->writeEndElement(); // EntityRef
    xmlWriter->writeEndElement(); // Actors
    xmlWriter->writeStartElement("Maneuver");
    xmlWriter->writeAttribute("name","TrajectoryManeuver");
    xmlWriter->writeStartElement("Event");
    xmlWriter->writeAttribute("name","TrajectoryEvent");
    xmlWriter->writeAttribute("priority","overwrite");
    xmlWriter->writeStartElement("Action");
    xmlWriter->writeAttribute("name","Trajectory");


    xmlWriter->writeStartElement("PrivateAction");
    xmlWriter->writeStartElement("RoutingAction");
    xmlWriter->writeStartElement("FollowTrajectoryAction");
    xmlWriter->writeStartElement("Trajectory");
    xmlWriter->writeAttribute("name","LaneChange");
    xmlWriter->writeAttribute("closed","false");

    xmlWriter->writeStartElement("Shape");
    xmlWriter->writeStartElement("Polyline");

    if (trajectory != nullptr)
    {
        for (unsigned int i = 0; i < trajectory->GetTimeVec()->size(); i++)
        {
            xmlWriter->writeStartElement("Vertex");
            xmlWriter->writeAttribute("time", QString::number(trajectory->GetTimeVec()->at(i)));
            xmlWriter->writeStartElement("Position");
            xmlWriter->writeStartElement("WorldPosition");
            xmlWriter->writeAttribute("x", QString::number(trajectory->GetXPosVec()->at(i), 'g', 10));
            xmlWriter->writeAttribute("y", QString::number(trajectory->GetYPosVec()->at(i), 'g', 10));
            xmlWriter->writeAttribute("z", "0");
            xmlWriter->writeAttribute("h", QString::number(trajectory->GetPsiVec()->at(i), 'g', 10));
            xmlWriter->writeAttribute("p", "0");
            xmlWriter->writeAttribute("r", "0");
            xmlWriter->writeEndElement(); // WorldPosition
            xmlWriter->writeEndElement(); // Position
            xmlWriter->writeEndElement(); // Vertex
        }
    }

    xmlWriter->writeEndElement(); // Polyline
    xmlWriter->writeEndElement(); // Shape

    xmlWriter->writeEndElement(); // Trajectory

    xmlWriter->writeStartElement("TimeReference");
    xmlWriter->writeEmptyElement("None");
    xmlWriter->writeEndElement(); // TimeReference

    xmlWriter->writeStartElement("TrajectoryFollowingMode");
    xmlWriter->writeAttribute("followingMode", "position");
    xmlWriter->writeEndElement(); // TrajectoryFollowingMode


    xmlWriter->writeEndElement(); // FollowTrajectoryAction
    xmlWriter->writeEndElement(); // RoutingAction
    xmlWriter->writeEndElement(); // PrivateAction


    xmlWriter->writeEndElement(); // Action



    xmlWriter->writeStartElement("StartTrigger");
    xmlWriter->writeStartElement("ConditionGroup");
    xmlWriter->writeStartElement("Condition");
    xmlWriter->writeAttribute("name", "Conditional");
    xmlWriter->writeStartElement("ByValueCondition");
    xmlWriter->writeStartElement("SimulationTimeCondition");
    xmlWriter->writeAttribute("value", "-1");
    xmlWriter->writeAttribute("rule", "greaterThan");
    xmlWriter->writeEndElement(); // SimulationTimeCondition
    xmlWriter->writeEndElement(); // ByValueCondition
    xmlWriter->writeEndElement(); // Condition
    xmlWriter->writeEndElement(); // ConditionGroup
    xmlWriter->writeEndElement(); // StartTrigger




    xmlWriter->writeEndElement(); // Event
    xmlWriter->writeEndElement(); // Maneuver
    xmlWriter->writeEndElement(); // ManeuverGroup
    xmlWriter->writeEndElement(); // Act

    return true;
}
