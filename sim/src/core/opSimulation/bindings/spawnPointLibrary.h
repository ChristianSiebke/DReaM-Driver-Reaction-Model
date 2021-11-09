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

//-----------------------------------------------------------------------------
//! @file  SpawnPointLibrary.h
//! @brief This file contains the internal representation of the library of a
//!        spawn point.
//-----------------------------------------------------------------------------

#pragma once

#include <list>
#include <string>
#include <QLibrary>
#include "include/agentFactoryInterface.h"
#include "include/spawnPointInterface.h"
#include "include/callbackInterface.h"
#include "include/agentBlueprintInterface.h"
#include "include/agentBlueprintProviderInterface.h"
#include "include/scenarioInterface.h"
#include "common/spawnPointLibraryDefinitions.h"

namespace core
{

class SpawnPointLibrary
{
public:    
    typedef const std::string &(*SpawnPointInterface_GetVersion)();
    typedef std::unique_ptr<SpawnPointInterface> (*SpawnPointInterface_CreateInstanceType)(const SpawnPointDependencies* dependencies,
                                                                                           const CallbackInterface* callbacks);

    SpawnPointLibrary(const std::string &libraryPath,
                      CallbackInterface *callbacks) :
        libraryPath(libraryPath),
        callbacks(callbacks)
    {}
    SpawnPointLibrary(const SpawnPointLibrary&) = delete;
    SpawnPointLibrary(SpawnPointLibrary&&) = delete;
    SpawnPointLibrary& operator=(const SpawnPointLibrary&) = delete;
    SpawnPointLibrary& operator=(SpawnPointLibrary&&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor, deletes the stored library (unloads it if necessary), if the list
    //! of stored spawn points is empty.
    //-----------------------------------------------------------------------------
    virtual ~SpawnPointLibrary();

    //-----------------------------------------------------------------------------
    //! Creates a QLibrary based on the path from the constructor and stores function
    //! pointer for getting the library version, creating and destroying instances
    //! and setting the spawn item (see typedefs for corresponding signatures).
    //!
    //! @return                 Null pointer
    //-----------------------------------------------------------------------------l
    bool Init();

    //-----------------------------------------------------------------------------
    //! Make sure that the library exists and is loaded, then call the "create instance"
    //! function pointer using the parameters from the spawn point instance to get
    //! a spawn point interface, which is then used to instantiate a spawn point
    //! which is also stored in the list of spawn points.
    //!
    //! @param[in]  world                   World instance
    //! @param[in]  parameters              Spawn point parameters
    //! @param[in]  agentFactory            Factory for the agents
    //! @param[in]  agentBlueprintProvider  AgentBluePrintProvider
    //! @param[in]  sampler                 Sampler instance
    //! @param[in]  scenario                Scenario from configs
    //! @return                         Spawn point created
    //-----------------------------------------------------------------------------
    std::unique_ptr<SpawnPoint> CreateSpawnPoint(const SpawnPointDependencies& dependencies);

private:    
    const std::string DllGetVersionId = "OpenPASS_GetVersion";
    const std::string DllCreateInstanceId = "OpenPASS_CreateInstance";

    std::string libraryPath;
    QLibrary *library = nullptr;
    CallbackInterface *callbacks;
    SpawnPointInterface_GetVersion getVersionFunc{nullptr};
    SpawnPointInterface_CreateInstanceType createInstanceFunc{nullptr};
};

} // namespace core

