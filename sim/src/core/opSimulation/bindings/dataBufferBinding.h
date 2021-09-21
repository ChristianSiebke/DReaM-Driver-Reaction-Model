/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  dataBufferBinding.h
//! @brief This file contains the interface to the dataBuffer library.
//-----------------------------------------------------------------------------

#pragma once

#include "common/opExport.h"
#include "common/runtimeInformation.h"
#include "include/dataBufferInterface.h"

class CallbackInterface;

namespace core {

class DataBufferLibrary;
class FrameworkConfig;

class SIMULATIONCOREEXPORT DataBufferBinding
{
public:
    DataBufferBinding(std::string libraryPath, const openpass::common::RuntimeInformation& runtimeInformation, CallbackInterface* callbacks);
    DataBufferBinding(const DataBufferBinding&) = delete;
    DataBufferBinding(DataBufferBinding&&) = delete;
    DataBufferBinding& operator=(const DataBufferBinding&) = delete;
    DataBufferBinding& operator=(DataBufferBinding&&) = delete;
    virtual ~DataBufferBinding();

    //-----------------------------------------------------------------------------
    //! Gets the dataBuffer instance library and stores it,
    //! then creates a new dataBufferInterface of the library.
    //!
    //! @return   dataBufferInterface created from the library
    //-----------------------------------------------------------------------------
    DataBufferInterface* Instantiate();

    //-----------------------------------------------------------------------------
    //! Unloads the stochasticsInterface binding by deleting the library.
    //-----------------------------------------------------------------------------
    void Unload();

private:
    const std::string libraryPath;
    DataBufferLibrary* library = nullptr;
    CallbackInterface* callbacks;
    const openpass::common::RuntimeInformation& runtimeInformation;
};

} // namespace core
