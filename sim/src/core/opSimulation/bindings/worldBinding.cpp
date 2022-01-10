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

#include "bindings/worldBinding.h"
#include "bindings/worldLibrary.h"

namespace core
{

WorldBinding::WorldBinding(std::string libraryPath,
                           CallbackInterface *callbacks,
                           StochasticsInterface* stochastics,
                           DataBufferWriteInterface* dataBuffer) :
        libraryPath{libraryPath},
        callbacks{callbacks},
        stochastics{stochastics},
        dataBuffer{dataBuffer}
{}

WorldBinding::~WorldBinding()
{
    Unload();
}

WorldInterface *WorldBinding::Instantiate()
{
    if(!library)
    {
        library = new (std::nothrow) WorldLibrary(libraryPath,
                                                  callbacks,
                                                  stochastics,
                                                  dataBuffer);
        if(!library)
        {
            return nullptr;
        }

        if(!library->Init())
        {
            delete library;
            library = nullptr;
            return nullptr;
        }
    }

    return library->CreateWorld();
}

void WorldBinding::Unload()
{
    if(library){
        library->ReleaseWorld();
        delete library;
        library = nullptr;
    }
}

} // namespace core
