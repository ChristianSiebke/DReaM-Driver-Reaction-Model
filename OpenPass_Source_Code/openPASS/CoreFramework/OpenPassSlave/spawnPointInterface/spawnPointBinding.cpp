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

#include <memory>
#include "spawnPointBinding.h"
#include "log.h"

namespace SimulationSlave {

SpawnPointBinding::SpawnPointBinding(CallbackInterface* callbacks) :
    callbacks{callbacks}
{}

std::unique_ptr<SpawnPoint> SpawnPointBinding::Instantiate(const std::string& libraryPath,
                                                           const SpawnPointDependencies& dependencies)
{
    spawnPointDependencies = dependencies; // take ownership by copy
    if (!library)
    {
        library = std::make_unique<SpawnPointLibrary>(libraryPath, callbacks);
        ThrowIfFalse(library && library->Init(), "Unable to create SpawnPointLibrary: " + libraryPath);
    }

    return library->CreateSpawnPoint(spawnPointDependencies);
}

} // namespace SimulationSlave
