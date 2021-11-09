/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
* Copyright (c) 2020 HLRS, University of Stuttgart.
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

#include "common/log.h"
#include "manipulator.h"
#include "bindings/manipulatorLibrary.h"
#include "bindings/observationBinding.h"

namespace core
{

bool ManipulatorLibrary::Init()
{
    std::string suffix = DEBUG_POSTFIX;
    library = new (std::nothrow) QLibrary(QString::fromStdString(libraryPath+suffix));
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

    getVersionFunc = (ManipulatorInterface_GetVersion)library->resolve(DllGetVersionId.c_str());
    if(!getVersionFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL";
        return false;
    }

    createInstanceFunc = (ManipulatorInterface_CreateInstanceType)library->resolve(DllCreateInstanceId.c_str());
    if(!createInstanceFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not instantiate class from DLL";
        return false;
    }

    createDefaultInstanceFunc = (ManipulatorInterface_CreateDefaultInstanceType)library->resolve(DllCreateDefaultInstanceId.c_str());
    if(!createDefaultInstanceFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not instantiate class from DLL";
        return false;
    }

    destroyInstanceFunc = (ManipulatorInterface_DestroyInstanceType)library->resolve(DllDestroyInstanceId.c_str());
    if(!destroyInstanceFunc)
    {
        LOG_INTERN(LogLevel::Warning) << "manipulator could not be released";
        return false;
    }

    try
    {
        LOG_INTERN(LogLevel::DebugCore) << "loaded manipulator library " << library->fileName().toStdString()
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

ManipulatorLibrary::~ManipulatorLibrary()
{
    if(!(manipulators.empty()))
    {
        LOG_INTERN(LogLevel::Warning) << "unloading library which is still in use";
    }

    if(library)
    {
        if(library->isLoaded())
        {
            LOG_INTERN(LogLevel::DebugCore) << "unloading manipulator library ";
            library->unload();
        }

        delete library;
        library = nullptr;
    }
}

bool ManipulatorLibrary::ReleaseManipulator(Manipulator *manipulator)
{
    if(!library)
    {
        return false;
    }

    auto findIter = std::find(manipulators.begin(), manipulators.end(), manipulator);
    if(manipulators.end() == findIter)
    {
        LOG_INTERN(LogLevel::Warning) << "manipulator doesn't belong to library";
        return false;
    }

    try
    {
        destroyInstanceFunc(manipulator->GetImplementation());
    }
    catch(std::runtime_error const &ex)
    {
        LOG_INTERN(LogLevel::Error) << "manipulator could not be released: " << ex.what();
        return false;
    }
    catch(...)
    {
        LOG_INTERN(LogLevel::Error) << "manipulator could not be released";
        return false;
    }

    manipulators.erase(findIter);

    return true;
}

Manipulator *ManipulatorLibrary::CreateManipulator(const openScenario::ManipulatorInformation manipulatorInformation,
                                                   EventNetworkInterface* eventNetwork,
                                                   WorldInterface* world)
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

    auto manipulatorInterface = createInstanceFunc(world,
                                                   manipulatorInformation,
                                                   eventNetwork,
                                                   callbacks);

    Manipulator *manipulator = new Manipulator(manipulatorInterface,
                                               this);
    manipulators.push_back(manipulator);
    return manipulator;
}

Manipulator *ManipulatorLibrary::CreateManipulator(const std::string& manipulatorType,
                                                   EventNetworkInterface* eventNetwork,
                                                   WorldInterface* world,
                                                   PublisherInterface *publisher)
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

    auto manipulatorInterface = createDefaultInstanceFunc(world,
                                                   manipulatorType,
                                                   eventNetwork,
                                                   callbacks,
                                                   publisher);

    Manipulator *manipulator = new Manipulator(manipulatorInterface,
                                               this);
    manipulators.push_back(manipulator);
    return manipulator;
}

} // namespace core
