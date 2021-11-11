/********************************************************************************
 * Copyright (c) 2016 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/


//-----------------------------------------------------------------------------
//! @file  observation_ttc.cpp
//! @brief This file contains the DLL wrapper.
//-----------------------------------------------------------------------------

#include "observation_ttc.h"
#include "observationInterface.h"
#include "observation_ttc_implementation.h"

const std::string Version = "0.0.1";    //!< The version of the current module - has to be incremented manually
static const CallbackInterface *Callbacks = nullptr;

//-----------------------------------------------------------------------------
//! dll-function to obtain the version of the current module
//!
//! @return                       Version of the current module
//-----------------------------------------------------------------------------
extern "C" OBSERVATION_TTCSHARED_EXPORT const std::string &OpenPASS_GetVersion()
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
extern "C" OBSERVATION_TTCSHARED_EXPORT ObservationInterface *OpenPASS_CreateInstance(StochasticsInterface *stochastics,
                                                                                     WorldInterface *world,
                                                                                     const ParameterInterface *parameters,
                                                                                     const CallbackInterface *callbacks)
{
    Callbacks = callbacks;

    try
    {
        return (ObservationInterface*)(new (std::nothrow) Observation_Ttc_Implementation(stochastics,
                                                                                         world,
                                                                                         parameters,
                                                                                         callbacks));
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

//-----------------------------------------------------------------------------
//! dll-function to destroy/delete an instance of the module
//!
//! @param[in]     implementation    The instance that should be freed
//-----------------------------------------------------------------------------
extern "C" OBSERVATION_TTCSHARED_EXPORT void OpenPASS_DestroyInstance(ObservationInterface *implementation)
{
    delete implementation;
}

extern "C" OBSERVATION_TTCSHARED_EXPORT bool OpenPASS_OpSimulationManagerPreHook(ObservationInterface *implementation)
{
    try
    {
        implementation->OpSimulationManagerPreHook();
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

extern "C" OBSERVATION_TTCSHARED_EXPORT bool OpenPASS_OpSimulationManagerPostHook(ObservationInterface *implementation,
                                                                    const std::string &filename)
{
    try
    {
        implementation->OpSimulationManagerPostHook(filename);
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

extern "C" OBSERVATION_TTCSHARED_EXPORT bool OpenPASS_Op_SimulationPreHook(ObservationInterface *implementation,
                                                                  const std::string &path)
{
    try
    {
        implementation->Op_SimulationPreHook(path);
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

extern "C" OBSERVATION_TTCSHARED_EXPORT bool OpenPASS_Op_SimulationPreRunHook(ObservationInterface *implementation)
{
    try
    {
        implementation->Op_SimulationPreRunHook();
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

extern "C" OBSERVATION_TTCSHARED_EXPORT bool OpenPASS_Op_SimulationUpdateHook(ObservationInterface *implementation,
                                                                     int time,
                                                                     RunResultInterface &runResult)
{
    try
    {
        implementation->Op_SimulationUpdateHook(time, runResult);
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

extern "C" OBSERVATION_TTCSHARED_EXPORT bool OpenPASS_Op_SimulationPostRunHook(ObservationInterface *implementation,
                                                                      const RunResultInterface &runResult)
{
    try
    {
        implementation->Op_SimulationPostRunHook(runResult);
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

extern "C" OBSERVATION_TTCSHARED_EXPORT bool OpenPASS_Op_SimulationPostHook(ObservationInterface *implementation)
{
    try
    {
        implementation->Op_SimulationPostHook();
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

extern "C" OBSERVATION_TTCSHARED_EXPORT const std::string OpenPASS_Op_SimulationResultFile(ObservationInterface *implementation)
{
    try
    {
        return implementation->Op_SimulationResultFile();
    }
    catch(const std::runtime_error &ex)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return "";
    }
    catch(...)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return "";
    }
}
