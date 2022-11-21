/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef XMLSCENERY_H
#define XMLSCENERY_H

#include "XmlMarks.h"
#include "XmlTrajectory.h"
#include "XmlObject.h"
#include "XmlViewObject.h"
#include "XmlGlobalData.h"

/*!
 * \brief The xml scenery class.
 * This class is responsible to manage all data for the scenery config file.
 */
class XmlScenery: public XmlBaseClass
{
public:
    //-----------------------------------------------------------------------------
    //! Default Constructor
    //-----------------------------------------------------------------------------
    XmlScenery() = default;

    //-----------------------------------------------------------------------------
    //! Destructor
    //-----------------------------------------------------------------------------
    virtual ~XmlScenery();

    //-----------------------------------------------------------------------------
    //! Implementation of Interface function.
    //! @see base class for more information
    //-----------------------------------------------------------------------------
    bool WriteToXml(QXmlStreamWriter *xmlWriter);

    //-----------------------------------------------------------------------------
    //! Setter and Adder functions.
    //-----------------------------------------------------------------------------
    void AddMarks(const PCM_Marks *marks);
    void SetObject(const PCM_Object *object);
    void SetViewObject(const PCM_ViewObject *viewObject);
    void AddTrajectory(int agentId, const PCM_Trajectory *trajectory);
    void SetGlobalData(const PCM_GlobalData *globalData);

private:
    std::vector<XmlMarks *> marksVec;             //!< vector containing the information of PCM marks
    std::vector<XmlTrajectory *> trajectories;    //!< vector of all trajectories of all agents
    XmlObject *object = nullptr;                  //!< containing information of PCM_Object
    XmlViewObject *viewObject = nullptr;          //!< containing information of PCM_ViewObject
    XmlGlobalData *globalData = nullptr;          //!< containing information of PCM_GlobalData
};


#endif // XMLSCENERY_H
