/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** @file  EventDetectorCommonBase.cpp */
//-----------------------------------------------------------------------------

#include "EventDetectorCommonBase.h"

int EventDetectorCommonBase::GetCycleTime()
{
    return cycleTime;
}

void EventDetectorCommonBase::Reset()
{
    return;
}

void EventDetectorCommonBase::Log(CbkLogLevel logLevel,
         const char *file,
         int line,
         const std::string &message)
{
    if(callbacks)
    {
        callbacks->Log(logLevel,
                       file,
                       line,
                       message);
    }
}

bool UniqueEval(std::vector<AgentInterface *>::iterator firstIt, std::vector<AgentInterface *>::iterator secondIt)
{
    return (*firstIt)->GetId() == (*secondIt)->GetId();
}
