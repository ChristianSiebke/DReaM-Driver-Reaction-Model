/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  world.cpp
//! @brief This file contains the DLL wrapper.
//-----------------------------------------------------------------------------

#include "World.h"
#include "core/opSimulation/modules/World_OSI/WorldImplementation.h"
#include "include/worldInterface.h"

const std::string Version = "0.0.1";
static const CallbackInterface *Callbacks = nullptr;

extern "C" WORLD_SHARED_EXPORT const std::string &OpenPASS_GetVersion()
{
    return Version;
}

extern "C" WORLD_SHARED_EXPORT WorldInterface *OpenPASS_CreateInstance(
        CallbackInterface *callbacks, StochasticsInterface* stochastics, DataBufferWriteInterface* dataBuffer)
{
    Callbacks = callbacks;

    try
    {
        return (WorldInterface*)(new (std::nothrow) WorldImplementation(callbacks, stochastics, dataBuffer));
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

extern "C" WORLD_SHARED_EXPORT void OpenPASS_DestroyInstance(WorldInterface *implementation)
{
    delete implementation;
}
