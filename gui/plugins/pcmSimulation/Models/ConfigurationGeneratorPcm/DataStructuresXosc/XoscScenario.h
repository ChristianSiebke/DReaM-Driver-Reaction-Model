/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef XMLSCENARIO_H
#define XMLSCENARIO_H

#include "XoscTrajectory.h"
#include "pcm_trajectory.h"
#include "../DataStructuresXml/XmlAgent.h"

class XoscScenario : public XmlBaseClass
{
public:
    XoscScenario() = default;
    virtual ~XoscScenario() = default;

    bool WriteToXml( QXmlStreamWriter *xmlWriter );

    void AddTrajectory(int agentId,
                       const PCM_Trajectory *trajectory);
    void AddAgent(int id, int agentTypeRef, PCM_ParticipantData *participant);

private:
    std::vector<XoscTrajectory> trajectories;     //!< vector of all trajectories of all agents
    std::vector<XmlAgent>       agents;        //!< vector of all agents of the situation

};

#endif // XMLSCENARIO_H
