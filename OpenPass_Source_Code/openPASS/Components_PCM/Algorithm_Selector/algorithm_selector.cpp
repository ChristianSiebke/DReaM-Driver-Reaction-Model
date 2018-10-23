//*****************************************************************************
// Copyright (c) 2017 ITK Engineering GmbH.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v1.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v10.html
//*****************************************************************************

//-----------------------------------------------------------------------------
//! @file  algorithm_selector.cpp
//! @brief This file contains the DLL wrapper.
//-----------------------------------------------------------------------------

#include "algorithm_selector.h"
#include "modelInterface.h"
#include "algorithm_selector_implementation.h"

const std::string Version = "0.0.1";
static const CallbackInterface *Callbacks = nullptr;

extern "C" ALGORITHM_SELECTOR_SHARED_EXPORT const std::string &OpenPASS_GetVersion()
{
    return Version;
}

extern "C" ALGORITHM_SELECTOR_SHARED_EXPORT ModelInterface *OpenPASS_CreateInstance(int componentId,
                                                                                    bool isInit,
                                                                                    int priority,
                                                                                    int offsetTime,
                                                                                    int responseTime,
                                                                                    int cycleTime,
                                                                                    StochasticsInterface *stochastics,
                                                                                    WorldInterface *world,
                                                                                    const ParameterInterface *parameters,
                                                                                    const std::map<int, ObservationInterface *> *observations,
                                                                                    AgentInterface *agent,
                                                                                    const CallbackInterface *callbacks)
{
    Q_UNUSED(world);
    Callbacks = callbacks;

    try
    {
        if (priority == 0)
        {
            priority = 100;
        }
        return (ModelInterface *)(new (std::nothrow) Algorithm_Selector_Implementation(componentId,
                                                                                       isInit,
                                                                                       priority,
                                                                                       offsetTime,
                                                                                       responseTime,
                                                                                       cycleTime,
                                                                                       stochastics,
                                                                                       parameters,
                                                                                       observations,
                                                                                       callbacks,
                                                                                       agent->GetAgentId()));
    }
    catch (const std::runtime_error &ex)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return nullptr;
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

extern "C" ALGORITHM_SELECTOR_SHARED_EXPORT void OpenPASS_DestroyInstance(
    ModelInterface *implementation)
{
    delete (Algorithm_Selector_Implementation *)implementation;
}

extern "C" ALGORITHM_SELECTOR_SHARED_EXPORT bool OpenPASS_UpdateInput(
    ModelInterface *implementation,
    int localLinkId,
    const std::shared_ptr<SignalInterface const> &data,
    int time)
{
    try
    {
        implementation->UpdateInput(localLinkId, data, time);
    }
    catch (const std::runtime_error &ex)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}

extern "C" ALGORITHM_SELECTOR_SHARED_EXPORT bool OpenPASS_UpdateOutput(
    ModelInterface *implementation,
    int localLinkId,
    std::shared_ptr<SignalInterface const> &data,
    int time)
{
    try
    {
        implementation->UpdateOutput(localLinkId, data, time);
    }
    catch (const std::runtime_error &ex)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}

extern "C" ALGORITHM_SELECTOR_SHARED_EXPORT bool OpenPASS_Trigger(ModelInterface *implementation,
                                                                  int time)
{
    try
    {
        implementation->Trigger(time);
    }
    catch (const std::runtime_error &ex)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}
