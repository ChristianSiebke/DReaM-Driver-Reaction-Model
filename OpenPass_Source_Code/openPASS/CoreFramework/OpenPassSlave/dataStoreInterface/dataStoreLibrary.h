/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  dataStoreLibrary.h
//! @brief This file contains the internal representation of the library of a
//!        dataStoreInterface.
//-----------------------------------------------------------------------------

#pragma once

#include <QLibrary>

#include "dataStoreBinding.h"

#include "Interfaces/callbackInterface.h"

namespace SimulationSlave
{

class DataStoreLibrary
{
public:
    typedef const std::string &(*DataStoreInterface_GetVersion)();
    typedef DataStoreInterface *(*DataStoreInterface_CreateInstanceType)(
            const openpass::common::RuntimeInformation* runtimeInformation, CallbackInterface* callbacks);
    typedef void (*DataStoreInterface_DestroyInstanceType)(DataStoreInterface *implementation);


    DataStoreLibrary(const std::string &dataStoreLibraryPath, CallbackInterface *callbacks) :
           dataStoreLibraryPath(dataStoreLibraryPath),
           callbacks(callbacks)
    {
    }

    DataStoreLibrary(const DataStoreLibrary&) = delete;
    DataStoreLibrary(DataStoreLibrary&&) = delete;
    DataStoreLibrary& operator=(const DataStoreLibrary&) = delete;
    DataStoreLibrary& operator=(DataStoreLibrary&&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor, deletes the stored library (unloads it if necessary)
    //-----------------------------------------------------------------------------
    virtual ~DataStoreLibrary();

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
    //! Delete the DataStoreInterface and the library
    //!
    //! @return   Flag if the release was successful
    //-----------------------------------------------------------------------------
    bool ReleaseDataStore();

    //-----------------------------------------------------------------------------
    //! Make sure that the library exists and is loaded, then call the "create instance"
    //! function pointer, which instantiates a DataStoreInterface. The created Interface
    //! is stored.
    //!
    //! @param[in]   runtimeInformation   Reference to the simulation slave runtime information
    //!
    //! @return   DataStoreInterface created
    //-----------------------------------------------------------------------------
    DataStoreInterface* CreateDataStore(const openpass::common::RuntimeInformation& runtimeInformation);

private:
    const std::string DllGetVersionId = "OpenPASS_GetVersion";
    const std::string DllCreateInstanceId = "OpenPASS_CreateInstance";
    const std::string DllDestroyInstanceId = "OpenPASS_DestroyInstance";

    const std::string dataStoreLibraryPath;
    DataStoreInterface* dataStoreInterface = nullptr;
    QLibrary* library = nullptr;
    CallbackInterface* callbacks;
    DataStoreInterface_GetVersion getVersionFunc{nullptr};
    DataStoreInterface_CreateInstanceType createInstanceFunc{nullptr};
    DataStoreInterface_DestroyInstanceType destroyInstanceFunc{nullptr};
};

} // namespace SimulationSlave
