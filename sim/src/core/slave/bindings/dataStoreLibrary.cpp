/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "bindings/dataStoreLibrary.h"

#include "common/log.h"
#include "modelElements/parameters.h"
#include "parameterbuilder.h"


namespace SimulationSlave
{

bool DataStoreLibrary::Init()
{
    library = new (std::nothrow) QLibrary(QString::fromStdString(dataStoreLibraryPath));

    if (!library)
    {
        return false;
    }

    if (!library->load())
    {
        LOG_INTERN(LogLevel::Error) << library->errorString().toStdString();
        delete library;
        library = nullptr;
        return false;
    }

    getVersionFunc = reinterpret_cast<DataStoreInterface_GetVersion>(library->resolve(DllGetVersionId.c_str()));
    if (!getVersionFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL";
        return false;
    }

    createInstanceFunc = reinterpret_cast<DataStoreInterface_CreateInstanceType>(library->resolve(DllCreateInstanceId.c_str()));
    if (!createInstanceFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not create instance from DLL";
        return false;
    }

    destroyInstanceFunc = reinterpret_cast<DataStoreInterface_DestroyInstanceType>(library->resolve(DllDestroyInstanceId.c_str()));
    if (!destroyInstanceFunc)
    {
        LOG_INTERN(LogLevel::Warning) << "dataStore could not be released";
        return false;
    }

    try
    {
        LOG_INTERN(LogLevel::DebugCore) << "loaded dataStore library " << library->fileName().toStdString()
                                        << ", version " << getVersionFunc();
    }
    catch (std::runtime_error const &ex)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL: " << ex.what();
        return false;
    }
    catch (...)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL";
        return false;
    }

    return true;
}

DataStoreLibrary::~DataStoreLibrary()
{
    if (dataStoreInterface)
    {
        LOG_INTERN(LogLevel::Warning) << "unloading library which is still in use";
    }

    if (library)
    {
        if (library->isLoaded())
        {
            LOG_INTERN(LogLevel::DebugCore) << "unloading dataStore library ";
            library->unload();
        }

        delete library;
        library = nullptr;
    }
}

bool DataStoreLibrary::ReleaseDataStore()
{
    if (!dataStoreInterface)
    {
        return true;
    }

    if (!library)
    {
        return false;
    }

    try
    {
        destroyInstanceFunc(dataStoreInterface);
    }
    catch (std::runtime_error const &ex)
    {
        LOG_INTERN(LogLevel::Error) << "dataStore could not be released: " << ex.what();
        return false;
    }
    catch (...)
    {
        LOG_INTERN(LogLevel::Error) << "dataStore could not be released";
        return false;
    }

    dataStoreInterface = nullptr;

    return true;
}

DataStoreInterface* DataStoreLibrary::CreateDataStore(const openpass::common::RuntimeInformation& runtimeInformation)
{
    if (!library)
    {
        return nullptr;
    }

    if (!library->isLoaded())
    {
        if (!library->load())
        {
            return nullptr;
        }
    }

    dataStoreInterface = nullptr;

    try
    {
        dataStoreInterface = createInstanceFunc(&runtimeInformation, callbacks);
    }
    catch (std::runtime_error const &ex)
    {
        LOG_INTERN(LogLevel::Error) << "could not create stochastics instance: " << ex.what();
        return nullptr;
    }
    catch (...)
    {
        LOG_INTERN(LogLevel::Error) << "could not create stochastics instance";
        return nullptr;
    }

    return dataStoreInterface;
}

} // namespace SimulationSlave


