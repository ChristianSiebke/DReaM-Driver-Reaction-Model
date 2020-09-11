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
//! @file  dataStoreBinding.h
//! @brief This file contains the interface to the dataStore library.
//-----------------------------------------------------------------------------

#pragma once

#include "common/opExport.h"
#include "common/runtimeInformation.h"
#include "include/dataStoreInterface.h"

class CallbackInterface;

namespace SimulationSlave {

class DataStoreLibrary;
class FrameworkConfig;

class CORESLAVEEXPORT DataStoreBinding
{
public:
    DataStoreBinding(std::string libraryPath, const openpass::common::RuntimeInformation& runtimeInformation, CallbackInterface* callbacks);
    DataStoreBinding(const DataStoreBinding&) = delete;
    DataStoreBinding(DataStoreBinding&&) = delete;
    DataStoreBinding& operator=(const DataStoreBinding&) = delete;
    DataStoreBinding& operator=(DataStoreBinding&&) = delete;
    virtual ~DataStoreBinding();

    //-----------------------------------------------------------------------------
    //! Gets the dataStore instance library and stores it,
    //! then creates a new dataStoreInterface of the library.
    //!
    //! @return   dataStoreInterface created from the library
    //-----------------------------------------------------------------------------
    DataStoreInterface* Instantiate();

    //-----------------------------------------------------------------------------
    //! Unloads the stochasticsInterface binding by deleting the library.
    //-----------------------------------------------------------------------------
    void Unload();

private:
    const std::string libraryPath;
    DataStoreLibrary* library = nullptr;
    CallbackInterface* callbacks;
    const openpass::common::RuntimeInformation& runtimeInformation;
};

} // namespace SimulationSlave
