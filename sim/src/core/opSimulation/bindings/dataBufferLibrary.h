/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  dataBufferLibrary.h
//! @brief This file contains the internal representation of the library of a
//!        dataBufferInterface.
//-----------------------------------------------------------------------------

#pragma once

#include <QLibrary>

#include "bindings/dataBufferBinding.h"

#include "include/callbackInterface.h"

namespace core
{

class DataBufferLibrary
{
public:
    typedef const std::string &(*DataBufferInterface_GetVersion)();
    typedef DataBufferInterface *(*DataBufferInterface_CreateInstanceType)(
            const openpass::common::RuntimeInformation* runtimeInformation, CallbackInterface* callbacks);
    typedef void (*DataBufferInterface_DestroyInstanceType)(DataBufferInterface *implementation);


    DataBufferLibrary(const std::string &dataBufferLibraryPath, CallbackInterface *callbacks) :
           dataBufferLibraryPath(dataBufferLibraryPath),
           callbacks(callbacks)
    {
    }

    DataBufferLibrary(const DataBufferLibrary&) = delete;
    DataBufferLibrary(DataBufferLibrary&&) = delete;
    DataBufferLibrary& operator=(const DataBufferLibrary&) = delete;
    DataBufferLibrary& operator=(DataBufferLibrary&&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor, deletes the stored library (unloads it if necessary)
    //-----------------------------------------------------------------------------
    virtual ~DataBufferLibrary();

    //-----------------------------------------------------------------------------
    //! Creates a QLibrary based on the path from the constructor and stores function
    //! pointer for getting the library version, creating and destroying instances
    //! and setting the stochasticsInterface item
    //! (see typedefs for corresponding signatures).
    //!
    //! @return   true on successful operation, false otherwise
    //-----------------------------------------------------------------------------
    bool Init();

    //-----------------------------------------------------------------------------
    //! Delete the dataBufferInterface and the library
    //!
    //! @return   Flag if the release was successful
    //-----------------------------------------------------------------------------
    bool ReleaseDataBuffer();

    //-----------------------------------------------------------------------------
    //! Make sure that the library exists and is loaded, then call the "create instance"
    //! function pointer, which instantiates a DataBufferInterface. The created Interface
    //! is stored.
    //!
    //! @param[in]   runtimeInformation   Reference to the simulation runtime information
    //!
    //! @return   DataBufferInterface created
    //-----------------------------------------------------------------------------
    DataBufferInterface* CreateDataBuffer(const openpass::common::RuntimeInformation& runtimeInformation);

private:
    const std::string DllGetVersionId = "OpenPASS_GetVersion";
    const std::string DllCreateInstanceId = "OpenPASS_CreateInstance";
    const std::string DllDestroyInstanceId = "OpenPASS_DestroyInstance";

    const std::string dataBufferLibraryPath;
    DataBufferInterface* dataBufferInterface = nullptr;
    QLibrary* library = nullptr;
    CallbackInterface* callbacks;
    DataBufferInterface_GetVersion getVersionFunc{nullptr};
    DataBufferInterface_CreateInstanceType createInstanceFunc{nullptr};
    DataBufferInterface_DestroyInstanceType destroyInstanceFunc{nullptr};
};

} // namespace core
