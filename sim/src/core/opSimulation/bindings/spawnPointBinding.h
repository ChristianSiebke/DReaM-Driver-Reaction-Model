/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SpawnPointBinding.h
//! @brief This file contains the interface to the spawn point libraries.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <string>
#include "include/spawnPointInterface.h"
#include "common/callbacks.h"
#include "common/spawnPointLibraryDefinitions.h"
#include "bindings/spawnPointLibrary.h"
#include "spawnPoint.h"

namespace core {

class SpawnPointBinding
{
public:
    SpawnPointBinding(CallbackInterface* callbacks);
    SpawnPointBinding(SpawnPointBinding&&) noexcept = default;
    virtual ~SpawnPointBinding() = default;

    SpawnPointBinding(const SpawnPointBinding&) = delete;
    SpawnPointBinding& operator=(const SpawnPointBinding&) = delete;
    SpawnPointBinding& operator=(SpawnPointBinding&&) = delete;

    //-----------------------------------------------------------------------------
    //! Gets the spawn point instance library either from the already stored libraries
    //! or create a new instance (which is then also stored), then create a new spawn
    //! point using the provided parameters.
    //!
    //! @param[in]  libraryPath             Path of the spawnpoint library
    //! @param[in]  deps                    Spawn point dependencies
    //! @return                          Spawn point created from the library
    //-----------------------------------------------------------------------------
    std::unique_ptr<SpawnPoint> Instantiate(const std::string& libraryPath,
                                            const SpawnPointDependencies& dependencies);

    //-----------------------------------------------------------------------------
    /// Transfers ownership of parameters to this specific binding
    /// \param parameter Parameters
    //-----------------------------------------------------------------------------
    void SetParameter(std::unique_ptr<ParameterInterface> parameter)
    {
        this->parameter = std::move(parameter);
    }

    //-----------------------------------------------------------------------------
    //! Unloads the spawn point binding by deleting all stored spawn point libraries.
    //-----------------------------------------------------------------------------
    void Unload();

private:
    std::unique_ptr<ParameterInterface> parameter {nullptr};
    std::unique_ptr<SpawnPointLibrary> library {nullptr};
    CallbackInterface* callbacks;
    SpawnPointDependencies spawnPointDependencies;
};

} // namespace core

