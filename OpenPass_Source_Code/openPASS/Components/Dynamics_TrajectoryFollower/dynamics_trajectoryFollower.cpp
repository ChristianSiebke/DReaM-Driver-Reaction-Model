/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  dynamics_trajectoryFollower.cpp */
//-----------------------------------------------------------------------------

#include "dynamics_trajectoryFollower.h"

#include <QCoreApplication>

#include "Interfaces/parameterInterface.h"
#include "trajectoryFollowerImplementation.h"

const static std::string VERSION = "0.2.0";
static const CallbackInterface *Callbacks = nullptr;

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT const std::string &OpenPASS_GetVersion()
{
    return VERSION;
}

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT ModelInterface *OpenPASS_CreateInstance(
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
    const CallbackInterface *callbacks)
{
    Callbacks = callbacks;

    try
    {
        return (ModelInterface *)(new (std::nothrow) TrajectoryFollowerImplementation(
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
            agent));
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

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT void OpenPASS_DestroyInstance(ModelInterface *implementation)
{
    delete implementation;
}

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT bool OpenPASS_UpdateInput(
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

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT bool OpenPASS_UpdateOutput(
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

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT bool OpenPASS_Trigger(
    ModelInterface *implementation,
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
