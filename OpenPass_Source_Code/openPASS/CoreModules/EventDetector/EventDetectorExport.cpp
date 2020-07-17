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
/** @file  EventDetectorExport.cpp */
//-----------------------------------------------------------------------------

#include "EventDetectorExport.h"

#include "CoreFramework/CoreShare/parameters.h"

//Different detectors
#include "CollisionDetector.h"
#include "ConditionalEventDetector.h"

const std::string version = "0.0.1";
static const CallbackInterface* Callbacks = nullptr;

extern "C" EVENT_DETECTOR_SHARED_EXPORT const std::string& OpenPASS_GetVersion()
{
    return version;
}

extern "C" EVENT_DETECTOR_SHARED_EXPORT EventDetectorInterface* OpenPASS_CreateCollisionDetectorInstance(
    WorldInterface* world,
    SimulationSlave::EventNetworkInterface* eventNetwork,
    const CallbackInterface* callbacks,
    StochasticsInterface* stochastics)
{
    Callbacks = callbacks;

    try
    {
        return static_cast<EventDetectorInterface*>(new CollisionDetector(world,
                                                                          eventNetwork,
                                                                          callbacks,
                                                                          stochastics));
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return nullptr;
    }
}
extern "C" EVENT_DETECTOR_SHARED_EXPORT EventDetectorInterface* OpenPASS_CreateConditionalDetectorInstance(
    WorldInterface* world,
    const openScenario::ConditionalEventDetectorInformation& eventDetectorInformation,
    SimulationSlave::EventNetworkInterface* eventNetwork,
    const CallbackInterface* callbacks,
    StochasticsInterface* stochastics)
{
    Callbacks = callbacks;

    try
    {
        return static_cast<EventDetectorInterface*>(new ConditionalEventDetector(world,
                                                                                 eventDetectorInformation,
                                                                                 eventNetwork,
                                                                                 callbacks,
                                                                                 stochastics));
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return nullptr;
    }
}

extern "C" EVENT_DETECTOR_SHARED_EXPORT void OpenPASS_DestroyInstance(EventDetectorInterface* implementation)
{
    delete implementation;
}

