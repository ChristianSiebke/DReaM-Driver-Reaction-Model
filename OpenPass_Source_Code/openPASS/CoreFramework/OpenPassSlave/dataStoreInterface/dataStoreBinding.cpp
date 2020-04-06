/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "dataStoreBinding.h"
#include "dataStoreLibrary.h"

namespace SimulationSlave
{

DataStoreBinding::DataStoreBinding(std::string libraryPath,
                                   const openpass::common::RuntimeInformation& runtimeInformation,
                                   CallbackInterface *callbacks) :
        libraryPath{libraryPath},
        callbacks{callbacks},
        runtimeInformation{runtimeInformation}
{
}

DataStoreBinding::~DataStoreBinding()
{
    Unload();
}

DataStoreInterface* DataStoreBinding::Instantiate()
{
    if (!library)
    {
        library = new (std::nothrow) DataStoreLibrary(libraryPath, callbacks);
        
        if (!library)
        {
            return nullptr;
        }

        if (!library->Init())
        {
            delete library;
            library = nullptr;
            return nullptr;
        }
    }

    return library->CreateDataStore(runtimeInformation);
}

void DataStoreBinding::Unload()
{
    if (library)
    {
        library->ReleaseDataStore();
        delete library;
        library = nullptr;
    }
}

} // namespace SimulationSlave
