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
/** \file  ManipulatorCommonBase.cpp */
//-----------------------------------------------------------------------------

#include "ManipulatorCommonBase.h"

ManipulatorCommonBase::ManipulatorCommonBase(WorldInterface *world,
                                             core::EventNetworkInterface *eventNetwork,
                                             const CallbackInterface *callbacks,
                                             const std::string &eventName) :
    world(world),
    parameters(nullptr),
    eventNetwork(eventNetwork),
    callbacks(callbacks),
    eventName(eventName)
{}

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
