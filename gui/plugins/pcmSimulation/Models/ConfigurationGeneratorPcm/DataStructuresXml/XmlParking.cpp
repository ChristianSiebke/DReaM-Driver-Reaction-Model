/*********************************************************************
* Copyright (c) 2017, 2018, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "XmlParking.h"

bool XmlParking::WriteToXml(QXmlStreamWriter *xmlWriter)
{
    xmlWriter->writeStartElement("PCM");

    globalData.WriteToXml(xmlWriter);

    xmlWriter->writeStartElement("Marks");
    for (XmlMarks marks : marksVec)
    {
        marks.WriteToXml(xmlWriter);
    }
    xmlWriter->writeEndElement();

    xmlWriter->writeStartElement("Objects");
    object.WriteToXml(xmlWriter);
    xmlWriter->writeEndElement();

    xmlWriter->writeStartElement("ViewObjects");
    viewObject.WriteToXml(xmlWriter);
    xmlWriter->writeEndElement();

    intendedCourse.WriteToXml(xmlWriter);

    xmlWriter->writeStartElement("Trajectories");
    for (XmlTrajectory trajectory : trajectories)
    {
        trajectory.WriteToXml(xmlWriter);
    }
    xmlWriter->writeEndElement();

    xmlWriter->writeEndElement();

    return true;
}

void XmlParking::AddMarks(PCM_Marks *marks)
{
    marksVec.push_back(XmlMarks(marks));
}

void XmlParking::AddObject(PCM_Object &object)
{
    this->object = XmlObject(&object);
}

void XmlParking::AddViewObject(PCM_ViewObject &viewObject)
{
    this->viewObject = XmlViewObject(&viewObject);
}

void XmlParking::AddTrajectory(int agentId, PCM_Trajectory *trajectory)
{
    trajectories.push_back(XmlTrajectory(agentId, trajectory));
}

void XmlParking::AddIntendedCourse(PCM_IntendedCourses &intendedCourse)
{
    this->intendedCourse = XmlIntendedCourses(intendedCourse);
}

void XmlParking::AddGlobalData(PCM_GlobalData &globalData)
{
    this->globalData = XmlGlobalData(&globalData);
}
