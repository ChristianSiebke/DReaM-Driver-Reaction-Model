/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  observation_Log.cpp */
//-----------------------------------------------------------------------------

#include "observation_log.h"
#include "observation_logImplementation.h"

class DataBufferReadInterface;

const std::string Version = "0.2.0";    //!< The version of the current module - has to be incremented manually
static const CallbackInterface* Callbacks = nullptr;

//-----------------------------------------------------------------------------
//! dll-function to obtain the version of the current module
//!
//! @return                       Version of the current module
//-----------------------------------------------------------------------------
extern "C" OBSERVATION_LOG_SHARED_EXPORT const std::string& OpenPASS_GetVersion()
{
    return Version;
}

//-----------------------------------------------------------------------------
//! dll-function to create an instance of the module
//!
//! @param[in]     stochastics    Pointer to the stochastics class loaded by the framework
//! @param[in]     world          Pointer to the world
//! @param[in]     parameters     Pointer to the parameters of the module
//! @param[in]     callbacks      Pointer to the callbacks
//! @return                       A pointer to the created module instance
//-----------------------------------------------------------------------------
extern "C" OBSERVATION_LOG_SHARED_EXPORT ObservationInterface* OpenPASS_CreateInstance(
    StochasticsInterface* stochastics,
    WorldInterface* world,
    core::EventNetworkInterface* eventNetwork,
    const ParameterInterface* parameters,
    const CallbackInterface* callbacks,
    DataBufferReadInterface* dataBuffer)
{
    Callbacks = callbacks;

    try
    {
        return (ObservationInterface*)(new (std::nothrow) ObservationLogImplementation(eventNetwork,
                                                                                         stochastics,
                                                                                         world,
                                                                                         parameters,
                                                                                         callbacks,
                                                                                         dataBuffer));
    }
    catch (const std::runtime_error& ex)
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

//-----------------------------------------------------------------------------
//! dll-function to destroy/delete an instance of the module
//!
//! @param[in]     implementation    The instance that should be freed
//-----------------------------------------------------------------------------
extern "C" OBSERVATION_LOG_SHARED_EXPORT void OpenPASS_DestroyInstance(ObservationInterface* implementation)
{
    delete implementation;
}

extern "C" OBSERVATION_LOG_SHARED_EXPORT bool OpenPASS_OpSimulationManagerPreHook(ObservationInterface* implementation)
{
    try
    {
        implementation->OpSimulationManagerPreHook();
    }
    catch (const std::runtime_error& ex)
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

extern "C" OBSERVATION_LOG_SHARED_EXPORT bool OpenPASS_OpSimulationManagerPostHook(ObservationInterface* implementation,
        const std::string& filename)
{
    try
    {
        implementation->OpSimulationManagerPostHook(filename);
    }
    catch (const std::runtime_error& ex)
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

extern "C" OBSERVATION_LOG_SHARED_EXPORT bool OpenPASS_OpSimulationPreHook(ObservationInterface* implementation)
{
    try
    {
        implementation->OpSimulationPreHook();
    }
    catch (const std::runtime_error& ex)
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

extern "C" OBSERVATION_LOG_SHARED_EXPORT bool OpenPASS_OpSimulationPreRunHook(ObservationInterface* implementation)
{
    try
    {
        implementation->OpSimulationPreRunHook();
    }
    catch (const std::runtime_error& ex)
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

extern "C" OBSERVATION_LOG_SHARED_EXPORT bool OpenPASS_OpSimulationUpdateHook(ObservationInterface* implementation,
        int time,
        RunResultInterface& runResult)
{
    try
    {
        implementation->OpSimulationUpdateHook(time, runResult);
    }
    catch (const std::runtime_error& ex)
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

extern "C" OBSERVATION_LOG_SHARED_EXPORT bool OpenPASS_OpSimulationPostRunHook(ObservationInterface* implementation,
        const RunResultInterface& runResult)
{
    try
    {
        implementation->OpSimulationPostRunHook(runResult);
    }
    catch (const std::runtime_error& ex)
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

extern "C" OBSERVATION_LOG_SHARED_EXPORT bool OpenPASS_OpSimulationPostHook(ObservationInterface* implementation)
{
    try
    {
        implementation->OpSimulationPostHook();
    }
    catch (const std::runtime_error& ex)
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

extern "C" OBSERVATION_LOG_SHARED_EXPORT const std::string OpenPASS_OpSimulationResultFile(
    ObservationInterface* implementation)
{
    try
    {
        return implementation->OpSimulationResultFile();
    }
    catch (const std::runtime_error& ex)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return "";
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return "";
    }
}
