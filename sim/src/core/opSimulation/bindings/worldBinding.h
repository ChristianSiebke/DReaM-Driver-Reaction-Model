/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  WorldBinding.h
//! @brief This file contains the interface to the world library.
//-----------------------------------------------------------------------------

#pragma once

#include "common/callbacks.h"
#include "common/opExport.h"
#include "include/worldInterface.h"
#include "include/stochasticsInterface.h"
#include "include/dataBufferInterface.h"

namespace core {

class WorldLibrary;
class FrameworkConfig;

class SIMULATIONCOREEXPORT WorldBinding
{
public:
    WorldBinding(std::string libraryPath,
                 CallbackInterface *callbacks,
                 StochasticsInterface* stochastics,
                 DataBufferWriteInterface* dataBuffer);
    WorldBinding(const WorldBinding&) = delete;
    WorldBinding(WorldBinding&&) = delete;
    WorldBinding& operator=(const WorldBinding&) = delete;
    WorldBinding& operator=(WorldBinding&&) = delete;
    virtual ~WorldBinding();

    //-----------------------------------------------------------------------------
    //! Gets the world instance library and stores it,
    //! then creates a new worldInterface of the library.
    //!
    //! @return                         worldInterface created from the library
    //-----------------------------------------------------------------------------
    WorldInterface *Instantiate();

    //-----------------------------------------------------------------------------
    //! Unloads the stochasticsInterface binding by deleting the library.
    //-----------------------------------------------------------------------------
    void Unload();

private:
    const std::string libraryPath;
    WorldLibrary *library = nullptr;
    CallbackInterface *callbacks;
    StochasticsInterface* stochastics;
    DataBufferWriteInterface* dataBuffer;
};

} // namespace core


