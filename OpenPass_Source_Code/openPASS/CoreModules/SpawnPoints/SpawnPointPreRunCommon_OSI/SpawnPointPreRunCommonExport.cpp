/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "SpawnPointPreRunCommonExport.h"
#include "SpawnPointPreRunCommon.h"

const std::string Version = "0.0.1";
static const CallbackInterface *Callbacks = nullptr;

extern "C" SPAWNPOINT_SHARED_EXPORT const std::string &OpenPASS_GetVersion()
{
    return Version;
}

extern "C" SPAWNPOINT_SHARED_EXPORT std::unique_ptr<SpawnPointInterface> OpenPASS_CreateInstance(const SpawnPointDependencies* dependencies,
                                                                                                 const CallbackInterface* callbacks)
{
    Callbacks = callbacks;

    try
    {
        return std::make_unique<SpawnPointPreRunCommon>(dependencies,
                                                        callbacks);
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

extern "C" SPAWNPOINT_SHARED_EXPORT SpawnPointInterface::Agents OpenPASS_Trigger(SpawnPointInterface *implementation, int time)
{
    return implementation->Trigger(time);
}
