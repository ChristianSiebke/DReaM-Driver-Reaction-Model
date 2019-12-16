/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  ManipulatorCommonBase.cpp */
//-----------------------------------------------------------------------------

#include "ManipulatorCommonBase.h"

ManipulatorCommonBase::ManipulatorCommonBase(WorldInterface *world,
                                             std::shared_ptr<ScenarioActionInterface> action,
                                             SimulationSlave::EventNetworkInterface *eventNetwork,
                                             const CallbackInterface *callbacks) :
    world(world),
    parameters(nullptr),
    eventNetwork(eventNetwork),
    callbacks(callbacks)
{
    sequenceName = action->GetSequenceName();

}

int ManipulatorCommonBase::GetCycleTime()
{
    return cycleTime;
}


void ManipulatorCommonBase::Log(CbkLogLevel logLevel,
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
