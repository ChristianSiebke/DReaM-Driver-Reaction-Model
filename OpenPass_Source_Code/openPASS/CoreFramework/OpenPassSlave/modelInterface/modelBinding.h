/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ModelBinding.h
//! @brief This file contains the interface to the model libraries.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <string>

#include "Interfaces/modelInterface.h"
#include "Interfaces/componentInterface.h"

#include "Common/opExport.h"
#include "Common/runtimeInformation.h"
#include "CoreFramework/CoreShare/callbacks.h"
#include "CoreFramework/CoreShare/log.h"

class PublisherInterface;

namespace SimulationSlave
{

class ModelLibrary;
class Agent;
class ComponentType;
class ObservationNetworkInterface;

class CORESLAVEEXPORT ModelBinding
{
public:
    ModelBinding(const std::string libraryPath,
                 const openpass::common::RuntimeInformation& runtimeInformation,
                 CallbackInterface *callbacks);
    ModelBinding(const ModelBinding&) = delete;
    ModelBinding(ModelBinding&&) = delete;
    ModelBinding& operator=(const ModelBinding&) = delete;
    ModelBinding& operator=(ModelBinding&&) = delete;
    virtual ~ModelBinding();

    //-----------------------------------------------------------------------------
    //! @brief Creates a new component from the given parameters using the respective
    //!         model library
    //!
    //! Gets the model library from the component type (instantiating and initializing
    //! it, if not already done), then creates the new component from the given
    //! parameters using this library.
    //!
    //! @param[in]  componentType       Type of the component to instantiate
    //! @param[in]  componentName       Name of the component to instantiate
    //! @param[in]  stochastics         Stochastics interface
    //! @param[in]  world               World representation
    //! @param[in]  observationNetwork  Network of the observation modules
    //! @param[in]  agent               Agent that the component type is a part of
    //! @param[in]  eventNetwork        Instance of the internal event logic
    //! @param[in]  publisher           Publisher instance
    //!
    //! @return                         The instantiated component
    //-----------------------------------------------------------------------------
    ComponentInterface *Instantiate(std::shared_ptr<ComponentType> componentType,
                                    std::string componentName,
                                    StochasticsInterface *stochastics,
                                    WorldInterface *world,
                                    ObservationNetworkInterface *observationNetwork,
                                    Agent *agent,
                                    EventNetworkInterface *eventNetwork,
                                    PublisherInterface *publisher);

    //-----------------------------------------------------------------------------
    //! Unloads the model library by deleting the stored libraries
    //-----------------------------------------------------------------------------
    void Unload();

private:
    const std::string libraryPath;
    const openpass::common::RuntimeInformation &runtimeInformation;
    CallbackInterface *callbacks {nullptr};

    std::map<std::string, ModelLibrary*> modelLibraries;
};

} // namespace SimulationSlave


