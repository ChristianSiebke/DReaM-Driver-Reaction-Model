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
/** \file  basicDataStore.cpp */
//-----------------------------------------------------------------------------

#include "basicDataStore.h"
#include "basicDataStoreImplementation.h"

const std::string Version = "1.0.0";    //!< The version of the current module - has to be incremented manually

//-----------------------------------------------------------------------------
//! dll-function to obtain the version of the current module
//!
//! @return                       Version of the current module
//-----------------------------------------------------------------------------
extern "C" BASIC_DATASTORE_SHARED_EXPORT const std::string& OpenPASS_GetVersion()
{
    return Version;
}

//-----------------------------------------------------------------------------
//! dll-function to create an instance of the module
//!
//! @param[in]     parameters     Pointer to the parameters of the module
//! @param[in]     callbacks      Pointer to the callbacks
//!
//! @return                       A pointer to the created module instance
//-----------------------------------------------------------------------------
extern "C" BASIC_DATASTORE_SHARED_EXPORT DataStoreInterface* OpenPASS_CreateInstance(const openpass::common::RuntimeInformation* runtimeInformation, const CallbackInterface* callbacks)
{
    try
    {
        return (DataStoreInterface*)(new (std::nothrow) BasicDataStoreImplementation(runtimeInformation, callbacks));
    }
    catch (const std::runtime_error& ex)
    {
        if (callbacks != nullptr)
        {
            callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return nullptr;
    }
    catch (...)
    {
        if (callbacks != nullptr)
        {
            callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return nullptr;
    }
}

//-----------------------------------------------------------------------------
//! dll-function to destroy/delete an instance of the module
//!
//! @param[in]     implementation    The instance that should be freed
//-----------------------------------------------------------------------------
extern "C" BASIC_DATASTORE_SHARED_EXPORT void OpenPASS_DestroyInstance(DataStoreInterface* implementation)
{
    delete implementation;
}
