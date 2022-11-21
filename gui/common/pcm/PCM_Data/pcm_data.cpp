/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "pcm_data.h"

PCM_Data::~PCM_Data()
{
    Clear();
}

void PCM_Data::AddPCM_Marks(const PCM_Marks *marks)
{
    marksVec.push_back(marks);
}

void PCM_Data::SetPCM_Object(const PCM_Object *object)
{
    this->object = object;
}

void PCM_Data::SetPCM_ViewObject(const PCM_ViewObject *viewObject)
{
    this->viewObject = viewObject;
}

void PCM_Data::SetPCM_GlobalData(const PCM_GlobalData *globalData)
{
    this->globalData = globalData;
}

void PCM_Data::AddPCM_Course(const PCM_Course *course)
{
    intendedCourses.push_back(course);
}

const std::vector<const PCM_Marks *> *PCM_Data::GetMarksVec() const
{
    return &marksVec;
}

const PCM_Marks *PCM_Data::GetMarksOfType(MarkType markType) const
{
    for (const PCM_Marks *marks : marksVec)
    {
        if (marks != nullptr)
        {
            if (marks->GetMarkType() == markType)
            {
                return marks;
            }
        }
    }
    return nullptr;
}

bool PCM_Data::AddPCM_Agent(int id, double xPos, double yPos, double yawAngle, double width,
                            double length)
{
    PCM_Agent *agent = new PCM_Agent(id);
    PCM_Line *line = new PCM_Line(0);

    std::vector<Common::Vector2d> boundaryPoints{
        {width * 0.5, length * 0.5},
        {-width * 0.5, length * 0.5},
        {-width * 0.5, -length * 0.5},
        {width * 0.5, -length * 0.5}};

    int count = 0;
    for (Common::Vector2d point : boundaryPoints)
    {
        point.Rotate(yawAngle);      //from vehicle to world coordinates
        point.Translate(xPos, yPos); //from vehicle to world coordinates
        line->AddPCM_Point(new PCM_Point(count++, point.x, point.y));
    }

    agent->AddPCM_Line(line);
    agentVec.push_back(agent);

    return true;
}

void PCM_Data::ClearAgentData()
{
    for (const PCM_Agent *agent : agentVec)
    {
        delete agent;
    }
    agentVec.clear();
}

void PCM_Data::Clear()
{
    for (const PCM_Marks *marks : marksVec)
    {
        if (marks != nullptr)
        {
            delete marks;
        }
    }
    marksVec.clear();

    if (object != nullptr)
    {
        delete object;
        object = nullptr;
    }

    if (viewObject != nullptr)
    {
        delete viewObject;
        viewObject = nullptr;
    }

    for (const PCM_Course *course : intendedCourses)
    {
        if (course != nullptr)
        {
            delete course;
        }
    }

    intendedCourses.clear();

    if (globalData != nullptr)
    {
        delete globalData;
        globalData = nullptr;
    }

    ClearAgentData();
}

const PCM_Object *PCM_Data::GetObject() const
{
    return object;
}

const PCM_ViewObject *PCM_Data::GetViewObject() const
{
    return viewObject;
}

const std::vector<const PCM_Course *> *PCM_Data::GetIntendedCourses() const
{
    return &intendedCourses;
}

const PCM_GlobalData *PCM_Data::GetGlobalData() const
{
    return globalData;
}
