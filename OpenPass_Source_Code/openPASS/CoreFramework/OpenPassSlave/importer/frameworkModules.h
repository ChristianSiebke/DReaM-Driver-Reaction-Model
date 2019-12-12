/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <string>
#include <vector>
#include "directories.h"
#include "spawnPointLibraryDefinitions.h"

namespace
{
    SpawnPointLibraryInfoCollection ConcatenateSpawnPointLibraries(const std::string& libraryDir, const SpawnPointLibraryInfoCollection& spawnPointLibraries)
    {
        SpawnPointLibraryInfoCollection libraries = spawnPointLibraries;
        for (auto& libraryInfo : libraries)
        {
            libraryInfo.libraryName = openpass::core::Directories::Concat(libraryDir, libraryInfo.libraryName);
        }

        return libraries;
    }
}

struct FrameworkModules
{
public:
    FrameworkModules(const int logLevel,
                     const std::string& libraryDir,
                     const std::string& eventDetectorLibrary,
                     const std::string& manipulatorLibrary,
                     const std::string& observationLibrary,
                     const std::string& stochasticsLibrary,
                     const std::string& worldLibrary,
                     const SpawnPointLibraryInfoCollection& spawnPointLibraries) :
        logLevel{logLevel},
        libraryDir{libraryDir},
        eventDetectorLibrary{openpass::core::Directories::Concat(libraryDir, eventDetectorLibrary)},
        manipulatorLibrary{openpass::core::Directories::Concat(libraryDir, manipulatorLibrary)},
        observationLibrary{openpass::core::Directories::Concat(libraryDir, observationLibrary)},
        stochasticsLibrary{openpass::core::Directories::Concat(libraryDir, stochasticsLibrary)},
        worldLibrary{openpass::core::Directories::Concat(libraryDir, worldLibrary)},
        spawnPointLibraries{ConcatenateSpawnPointLibraries(libraryDir, spawnPointLibraries)}
    {}

    const int logLevel;
    const std::string libraryDir;
    const std::string eventDetectorLibrary;
    const std::string manipulatorLibrary;
    const std::string observationLibrary;
    const std::string stochasticsLibrary;
    const std::string worldLibrary;
    SpawnPointLibraryInfoCollection spawnPointLibraries;
};
