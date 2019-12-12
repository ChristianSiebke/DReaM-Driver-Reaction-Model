/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <iostream>
#include <algorithm>
#include <QLibrary>
#include <sstream>

#include "agentFactory.h"
#include "CoreFramework/CoreShare/log.h"
#include "observationBinding.h"
#include "spawnPoint.h"
#include "spawnPointLibrary.h"

namespace SimulationSlave
{

bool SpawnPointLibrary::Init()
{
    library = new (std::nothrow) QLibrary(QString::fromStdString(libraryPath));
    if(!library)
    {
        return false;
    }

    if(!library->load())
    {
        LOG_INTERN(LogLevel::Error) << library->errorString().toStdString();
        delete library;
        library = nullptr;
        return false;
    }

    getVersionFunc = (SpawnPointInterface_GetVersion)library->resolve(DllGetVersionId.c_str());
    if(!getVersionFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL";
        return false;
    }

    createInstanceFunc = (SpawnPointInterface_CreateInstanceType)library->resolve(DllCreateInstanceId.c_str());
    if(!createInstanceFunc)
    {
        return false;
    }

    try
    {
        LOG_INTERN(LogLevel::DebugCore) << "loaded spawn point library " << library->fileName().toStdString()
                                        << ", version " << getVersionFunc();
    }
    catch(std::runtime_error const &ex)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL: " << ex.what();
        return false;
    }
    catch(...)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL";
        return false;
    }

    return true;
}

SpawnPointLibrary::~SpawnPointLibrary()
{
    if(library)
    {
        if(library->isLoaded())
        {
            LOG_INTERN(LogLevel::DebugCore) << "unloading library " << libraryPath;
            library->unload();
        }

        delete library;
        library = nullptr;
    }
}

std::unique_ptr<SpawnPoint> SpawnPointLibrary::CreateSpawnPoint(const SpawnPointDependencies& dependencies)
{
    if(!library)
    {
        return nullptr;
    }

    if(!library->isLoaded())
    {
        if(!library->load())
        {
            return nullptr;
        }
    }

    std::unique_ptr<SpawnPointInterface> spawnPoint = nullptr;
    try
    {
        spawnPoint = createInstanceFunc(&dependencies, callbacks);
    }
    catch(std::runtime_error const &ex)
    {
        LOG_INTERN(LogLevel::Error) << "could not create spawn point instance: " << ex.what();
        return nullptr;
    }
    catch(...)
    {
        LOG_INTERN(LogLevel::Error) << "could not create spawn point instance";
        return nullptr;
    }

    if(!spawnPoint)
    {
        return nullptr;
    }

    return std::make_unique<SpawnPoint>(dependencies.agentFactory, std::move(spawnPoint), this);
}
} // namespace SimulationSlave
