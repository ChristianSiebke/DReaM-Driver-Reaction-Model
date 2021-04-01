/*********************************************************************
* Copyright (c) 2017, 2018, 2020, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "XmlScenery.h"

XmlScenery::~XmlScenery()
{
    for (XmlMarks *marks : marksVec)
    {
        if (marks != nullptr)
        {
            delete marks;
        }
    }
    marksVec.clear();

    for (XmlTrajectory *trajectory : trajectories)
    {
        if (trajectory != nullptr)
        {
            delete trajectory;
        }
    }
    trajectories.clear();

    if (object != nullptr)
    {
        delete object;
    }

    if (viewObject != nullptr)
    {
        delete viewObject;
    }

    if (globalData != nullptr)
    {
        delete globalData;
    }
}

bool XmlScenery::WriteToXml(QXmlStreamWriter *xmlWriter)
{
    if (xmlWriter == nullptr)
    {
        return false;
    }

    xmlWriter->writeStartElement("PCM");

    if (!globalData->WriteToXml(xmlWriter))
    {
        return false;
    }

    xmlWriter->writeStartElement("Marks");
    for (XmlMarks *marks : marksVec)
    {
        if (!marks->WriteToXml(xmlWriter))
        {
            return false;
        }
    }
    xmlWriter->writeEndElement(); // Marks

    xmlWriter->writeStartElement("Objects");
    if (!object->WriteToXml(xmlWriter))
    {
        return false;
    }
    xmlWriter->writeEndElement(); // Objects

    xmlWriter->writeStartElement("ViewObjects");
    if (!viewObject->WriteToXml(xmlWriter))
    {
        return false;
    }
    xmlWriter->writeEndElement(); // ViewObjects

    xmlWriter->writeEndElement(); // PCM

    return true;
}

void XmlScenery::AddMarks(const PCM_Marks *marks)
{
    marksVec.push_back(new XmlMarks(marks));
}

void XmlScenery::SetObject(const PCM_Object *object)
{
    this->object = new XmlObject(object);
}

void XmlScenery::SetViewObject(const PCM_ViewObject *viewObject)
{
    this->viewObject = new XmlViewObject(viewObject);
}

void XmlScenery::AddTrajectory(int agentId, const PCM_Trajectory *trajectory)
{
    trajectories.push_back(new XmlTrajectory(agentId, trajectory));
}

void XmlScenery::SetGlobalData(const PCM_GlobalData *globalData)
{
    this->globalData = new XmlGlobalData(globalData);
}
