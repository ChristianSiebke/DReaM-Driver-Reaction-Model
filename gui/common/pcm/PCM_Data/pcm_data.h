/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  pcm_data.h
//! @brief class to store all data of a pcm file
//!
//! This class is responsible to manage data of a pcm file.
//-----------------------------------------------------------------------------

#ifndef PCM_DATA_H
#define PCM_DATA_H

#include <vector>
#include "pcm_marks.h"
#include "pcm_object.h"
#include "pcm_viewObject.h"
#include "pcm_course.h"
#include "pcm_globalData.h"
#include "pcm_agent.h"

struct AgentDetection
{
    int egoId = -1;
    const PCM_Point *egoPoint = nullptr;
    double viewAngle = INFINITY;
    double range = INFINITY;

    int oppId = -1;
    PCM_Point oppPoint;
    PCM_LineSegment fullOppLineSegment;
    PCM_LineSegment subOppLineSegment;
    double distance = INFINITY;
};

class PCM_Data
{
public:
    //-----------------------------------------------------------------------------
    //! Default Constructor
    //-----------------------------------------------------------------------------
    PCM_Data() = default;

    // removing operators
    PCM_Data(const PCM_Data &) = delete;
    PCM_Data(PCM_Data &&) = delete;
    PCM_Data &operator=(const PCM_Data &) = delete;
    PCM_Data &operator=(PCM_Data &&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor
    //-----------------------------------------------------------------------------
    ~PCM_Data();

    //-----------------------------------------------------------------------------
    //! Function to add a marks to the data
    //!
    //! @param[in]     marks    PCM_Marks to add
    //-----------------------------------------------------------------------------
    void AddPCM_Marks(const PCM_Marks *marks);

    //-----------------------------------------------------------------------------
    //! Function to set the pcm object
    //!
    //! @param[in]     object   PCM_Object to be set
    //-----------------------------------------------------------------------------
    void SetPCM_Object(const PCM_Object *object);

    //-----------------------------------------------------------------------------
    //! Function to set the pcm viewObject
    //!
    //! @param[in]     viewObject   PCM_ViewObject to be set
    //-----------------------------------------------------------------------------
    void SetPCM_ViewObject(const PCM_ViewObject *viewObject);

    //-----------------------------------------------------------------------------
    //! Function to set the pcm global data
    //!
    //! @param[in]     globalData   PCM_GlobalData to be set
    //-----------------------------------------------------------------------------
    void SetPCM_GlobalData(const PCM_GlobalData *globalData);

    //-----------------------------------------------------------------------------
    //! Function to add a course to the data
    //!
    //! @param[in]     course   PCM_Course to add
    //-----------------------------------------------------------------------------
    void AddPCM_Course(const PCM_Course *course);

    //-----------------------------------------------------------------------------
    //! Get the vector of marks
    //!
    //! @return                 vector of marks
    //-----------------------------------------------------------------------------
    const std::vector<const PCM_Marks *> *GetMarksVec() const;

    //-----------------------------------------------------------------------------
    //! Get the PCM_Marks of a specific type.
    //!
    //! @return                 nullptr if not found
    //-----------------------------------------------------------------------------
    const PCM_Marks *GetMarksOfType(MarkType markType) const;

    //-----------------------------------------------------------------------------
    //! Function to add a agent as rectangle to the data.
    //!
    //! @param[in]      id          id of agent
    //! @param[in]      xPos        x position of cog of agent
    //! @param[in]      yPos        y position of cog of agent
    //! @param[in]      yawAngle    yaw angle of agent
    //! @param[in]      width       width of agent
    //! @param[in]      length      length of agent
    //!
    //! @return         true for success
    //-----------------------------------------------------------------------------
    bool AddPCM_Agent(int id, double xPos, double yPos, double yawAngle, double width, double length);

    //-----------------------------------------------------------------------------
    //! Clear the agent data.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    void ClearAgentData();

    //-----------------------------------------------------------------------------
    //! Clear all data.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    void Clear();

    //-----------------------------------------------------------------------------
    //! Retrieve the object
    //! @return                        object
    //-----------------------------------------------------------------------------
    const PCM_Object *GetObject() const;

    //-----------------------------------------------------------------------------
    //! Retrieve the view object
    //! @return                        viewObject
    //-----------------------------------------------------------------------------
    const PCM_ViewObject *GetViewObject() const;

    //-----------------------------------------------------------------------------
    //! Retrieve the intendedCourses
    //! @return                        intendedCourses
    //-----------------------------------------------------------------------------
    const std::vector<const PCM_Course *> *GetIntendedCourses() const;

    //-----------------------------------------------------------------------------
    //! Retrieve the global Data
    //! @return                        globalData
    //-----------------------------------------------------------------------------
    const PCM_GlobalData *GetGlobalData() const;

private:
    std::vector<const PCM_Marks *> marksVec;         //!< vector of marks
    const PCM_Object *object = nullptr;              //!< pcm object
    const PCM_ViewObject *viewObject = nullptr;      //!< pcm viewObject
    std::vector<const PCM_Course *> intendedCourses; //!< vector of pcm courses
    const PCM_GlobalData *globalData = nullptr;      //!< pcm global data
    std::vector<const PCM_Agent *> agentVec;         //!< vector of agents
};

#endif // PCM_DATA_H
