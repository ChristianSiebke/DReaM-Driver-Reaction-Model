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

//-----------------------------------------------------------------------------
//! @file  wordLibrary.h
//! @brief This file contains the internal representation of the library of a
//!        worldInterface.
//-----------------------------------------------------------------------------

#pragma once

#include <QLibrary>
#include "bindings/worldBinding.h"
#include "include/callbackInterface.h"
#include "include/dataBufferInterface.h"

namespace core
{

class WorldLibrary
{
public:
    typedef const std::string &(*WorldInterface_GetVersion)();
    typedef WorldInterface *(*WorldInterface_CreateInstanceType)(
            const CallbackInterface *callbacks, StochasticsInterface* stochastics, DataBufferWriteInterface* dataBuffer);
    typedef void (*WorldInterface_DestroyInstanceType)(WorldInterface *implementation);


    WorldLibrary(const std::string &worldLibraryPath,
                 CallbackInterface *callbacks,
                 StochasticsInterface* stochastics,
                 DataBufferWriteInterface* dataBuffer) :
           worldLibraryPath(worldLibraryPath),
           callbacks(callbacks),
           stochastics(stochastics),
           dataBuffer(dataBuffer)
    {}

    WorldLibrary(const WorldLibrary&) = delete;
    WorldLibrary(WorldLibrary&&) = delete;
    WorldLibrary& operator=(const WorldLibrary&) = delete;
    WorldLibrary& operator=(WorldLibrary&&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor, deletes the stored library (unloads it if necessary)
    //-----------------------------------------------------------------------------
    virtual ~WorldLibrary();

    //-----------------------------------------------------------------------------
    //! Creates a QLibrary based on the path from the constructor and stores function
    //! pointer for getting the library version, creating and destroying instances
    //! and setting the stochasticsInterface item
    //! (see typedefs for corresponding signatures).
    //!
    //! @return                 Null pointer
    //-----------------------------------------------------------------------------
    bool Init();

    //-----------------------------------------------------------------------------
    //! Delete the worldInterface and the library
    //!
    //! @return                         Flag if the release was successful
    //-----------------------------------------------------------------------------
    bool ReleaseWorld();

    //-----------------------------------------------------------------------------
    //! Make sure that the library exists and is loaded, then call the "create instance"
    //! function pointer using the parameters from the stochastics instance to get
    //! a stochastics interface, which is then used to instantiate a stochasticsInterface
    //! which is stored.
    //!
    //! @return                         stochasticsInterface created
    //-----------------------------------------------------------------------------
    WorldInterface *CreateWorld();

private:
    const std::string DllGetVersionId = "OpenPASS_GetVersion";
    const std::string DllCreateInstanceId = "OpenPASS_CreateInstance";
    const std::string DllDestroyInstanceId = "OpenPASS_DestroyInstance";

    const std::string worldLibraryPath;
    WorldInterface *worldInterface = nullptr;
    QLibrary *library = nullptr;
    CallbackInterface *callbacks;
    StochasticsInterface* stochastics;
    DataBufferWriteInterface* dataBuffer;
    WorldInterface_GetVersion getVersionFunc{nullptr};
    WorldInterface_CreateInstanceType createInstanceFunc{nullptr};
    WorldInterface_DestroyInstanceType destroyInstanceFunc{nullptr};
};

} // namespace core


