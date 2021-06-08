/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2021 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "bindings/observationBinding.h"
#include "include/observationInterface.h"
#include "bindings/observationLibrary.h"

namespace SimulationSlave {

ObservationBinding::ObservationBinding(const openpass::common::RuntimeInformation &runtimeInformation,
                                       CallbackInterface* callbacks) :
    runtimeInformation{runtimeInformation},
    callbacks{callbacks}
{}

ObservationBinding::~ObservationBinding()
{
    Unload();
}

ObservationModule* ObservationBinding::Instantiate(const std::string& libraryPath,
        const openpass::parameter::ParameterSetLevel1 &parameter,
        StochasticsInterface* stochastics,
        WorldInterface* world,
        EventNetworkInterface* eventNetwork,
        DataBufferReadInterface* const dataBuffer)
{
    if (!library)
    {
        library = new (std::nothrow) ObservationLibrary(libraryPath, callbacks);
        if (!library)
        {
            return nullptr;
        }

        if (!library->Init())
        {
            delete library;
            return nullptr;
        }
    }

    return library->CreateObservationModule(runtimeInformation,
                                            parameter,
                                            stochastics,
                                            world,
                                            eventNetwork,
                                            dataBuffer);
}

void ObservationBinding::Unload()
{
    if (library != nullptr)
    {
        delete library;
        library = nullptr;
    }
}

} // namespace SimulationSlave
