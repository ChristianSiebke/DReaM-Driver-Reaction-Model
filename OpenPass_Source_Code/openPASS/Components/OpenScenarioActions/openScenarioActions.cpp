/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  OpenScenarioActions.cpp */
//-----------------------------------------------------------------------------

#include "openScenarioActions.h"
#include "openScenarioActionsImplementation.h"

const std::string Version = "0.1.0";
static const CallbackInterface *Callbacks = nullptr;

extern "C" OPENSCENARIO_ACTIONS_SHARED_EXPORT const std::string &OpenPASS_GetVersion()
{
    return Version;
}

extern "C" OPENSCENARIO_ACTIONS_SHARED_EXPORT ModelInterface *OpenPASS_CreateInstance(
        std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        PublisherInterface * const publisher,
        AgentInterface *agent,
        const CallbackInterface *callbacks,
        SimulationSlave::EventNetworkInterface * const eventNetwork)
{
    Callbacks = callbacks;

    try
    {
        return (ModelInterface*)(new (std::nothrow) OpenScenarioActionsImplementation(
                                      componentName,
                                      isInit,
                                      priority,
                                      offsetTime,
                                      responseTime,
                                      cycleTime,
                                      stochastics,
                                      world,
                                      parameters,
                                      publisher,
                                      callbacks,
                                      agent,
                                      eventNetwork));
    }
    catch(const std::runtime_error &ex)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return nullptr;
    }
    catch(...)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return nullptr;
    }
}

extern "C" OPENSCENARIO_ACTIONS_SHARED_EXPORT void OpenPASS_DestroyInstance(ModelInterface *implementation)
{
    delete (OpenScenarioActionsImplementation*)implementation;
}

extern "C" OPENSCENARIO_ACTIONS_SHARED_EXPORT bool OpenPASS_UpdateInput(ModelInterface *implementation,
                                                                        int localLinkId,
                                                                        const std::shared_ptr<SignalInterface const> &data,
                                                                        int time)
{
    try
    {
        implementation->UpdateInput(localLinkId, data, time);
    }
    catch(const std::runtime_error &ex)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch(...)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}

extern "C" OPENSCENARIO_ACTIONS_SHARED_EXPORT bool OpenPASS_UpdateOutput(ModelInterface *implementation,
                                                                         int localLinkId,
                                                                         std::shared_ptr<SignalInterface const> &data,
                                                                         int time)
{
    try
    {
        implementation->UpdateOutput(localLinkId, data, time);
    }
    catch(const std::runtime_error &ex)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch(...)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}

extern "C" OPENSCENARIO_ACTIONS_SHARED_EXPORT bool OpenPASS_Trigger(ModelInterface *implementation,
                                                                    int time)
{
    try
    {
        implementation->Trigger(time);
    }
    catch(const std::runtime_error &ex)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch(...)
    {

        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}
