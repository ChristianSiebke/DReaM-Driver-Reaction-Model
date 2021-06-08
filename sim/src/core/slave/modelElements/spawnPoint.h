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
//! @file  SpawnPoint.h
//! @brief This file contains the internal representation of the a spawn point.
//-----------------------------------------------------------------------------

#pragma once

#include <list>

#include "modelElements/parameters.h"
#include "include/spawnPointInterface.h"
#include "bindings/spawnPointLibrary.h"
#include "include/worldInterface.h"
#include "agentBlueprint.h"
#include "common/log.h"

namespace SimulationSlave
{

class SpawnPoint
{
public:    
    SpawnPoint(AgentFactoryInterface *agentFactory,
               std::unique_ptr<SpawnPointInterface>&& implementation,
               SpawnPointLibrary *library) :
        library(library),
        implementation(std::move(implementation)),
        agentFactory(agentFactory)
    {
        LOG_INTERN(LogLevel::DebugCore) << "created spawn point " << id;
    }
    SpawnPoint(const SpawnPoint&) = delete;
    SpawnPoint(SpawnPoint&&) = delete;
    SpawnPoint& operator=(const SpawnPoint&) = delete;
    SpawnPoint& operator=(SpawnPoint&&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor, releases the spawn point from the stored library.
    //-----------------------------------------------------------------------------
    virtual ~SpawnPoint() = default;

    //-----------------------------------------------------------------------------
    //! Return the ID of the respective spawn point instance.
    //!
    //! @return                         ID of the respective spawn point instance
    //-----------------------------------------------------------------------------
    int GetId() const
    {
        return id;
    }

    //-----------------------------------------------------------------------------
    //! Returns the agent factory.
    //!
    //! @return                         Agent factory
    //-----------------------------------------------------------------------------
    AgentFactoryInterface *GetAgentFactory()
    {
        return agentFactory;
    }

    //-----------------------------------------------------------------------------
    //! Returns the spawn point interface.
    //!
    //! @return                         Spawn point interface
    //-----------------------------------------------------------------------------
    SpawnPointInterface* GetImplementation()
    {
        return implementation.get();
    }

    //-----------------------------------------------------------------------------
    //! Returns the spawn point library.
    //!
    //! @return                         Spawn point library
    //-----------------------------------------------------------------------------
    SpawnPointLibrary *GetLibrary()
    {
        return library;
    }

private:
    SpawnPointLibrary *library;
    std::unique_ptr<SpawnPointInterface> implementation;
    AgentFactoryInterface *agentFactory;
    int id = 0;
};

} // namespace SimulationSlave


