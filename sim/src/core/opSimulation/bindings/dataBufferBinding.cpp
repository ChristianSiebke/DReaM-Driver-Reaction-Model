/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "bindings/dataBufferBinding.h"
#include "bindings/dataBufferLibrary.h"

namespace core
{

DataBufferBinding::DataBufferBinding(std::string libraryPath,
                                   const openpass::common::RuntimeInformation& runtimeInformation,
                                   CallbackInterface *callbacks) :
        libraryPath{libraryPath},
        callbacks{callbacks},
        runtimeInformation{runtimeInformation}
{
}

DataBufferBinding::~DataBufferBinding()
{
    Unload();
}

DataBufferInterface* DataBufferBinding::Instantiate()
{
    if (!library)
    {
        library = new (std::nothrow) DataBufferLibrary(libraryPath, callbacks);
        
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

    return library->CreateDataBuffer(runtimeInformation);
}

void DataBufferBinding::Unload()
{
    if (library)
    {
        library->ReleaseDataBuffer();
        delete library;
        library = nullptr;
    }
}

} // namespace core
