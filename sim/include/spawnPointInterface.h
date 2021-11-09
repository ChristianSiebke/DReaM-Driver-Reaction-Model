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
//! @file  SpawnPointInterface.h
//! @brief This file contains the interface of the spawn points to interact
//!        with the framework.
//-----------------------------------------------------------------------------

#pragma once

#include <string>

#include "modelElements/agent.h"
#include "include/parameterInterface.h"
#include "include/callbackInterface.h"
#include "include/agentBlueprintInterface.h"
#include "include/agentInterface.h"

class WorldInterface;

//-----------------------------------------------------------------------------
//! This class represents a spawn point
//-----------------------------------------------------------------------------
class SpawnPointInterface
{
public:    
    SpawnPointInterface(WorldInterface *world,
                        const CallbackInterface *callbacks) :
        world(world),
        callbacks(callbacks)
    {}
    SpawnPointInterface(const SpawnPointInterface&) = delete;
    SpawnPointInterface(SpawnPointInterface&&) = delete;
    SpawnPointInterface& operator=(const SpawnPointInterface&) = delete;
    SpawnPointInterface& operator=(SpawnPointInterface&&) = delete;
    virtual ~SpawnPointInterface() = default;

    using Agents = std::vector<core::Agent*>;
    virtual Agents Trigger(int time) = 0;

protected:
    //-----------------------------------------------------------------------------
    //! Retrieves the world.
    //!
    //! @return                  World (scenery and agents)
    //-----------------------------------------------------------------------------
    WorldInterface *GetWorld() const
    {
        return world;
    }

    //-----------------------------------------------------------------------------
    //! Provides callback to LOG() macro.
    //!
    //! @param[in]     logLevel    Importance of log
    //! @param[in]     file        Name of file where log is called
    //! @param[in]     line        Line within file where log is called
    //! @param[in]     message     Message to log
    //-----------------------------------------------------------------------------
    void Log(CbkLogLevel logLevel,
             const char *file,
             int line,
             const std::string &message) const
    {
        if(callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

private:
    WorldInterface *world;                //!< References the world of the framework
    const CallbackInterface *callbacks;   //!< References the callback functions of the framework    
};


