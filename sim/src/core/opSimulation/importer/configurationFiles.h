/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
* Copyright (c) 2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <string>
#include "directories.h"
#include "common/opExport.h"

/// Container for configuration file related paths
/// \see Directories
struct SIMULATIONCOREEXPORT ConfigurationFiles
{
    ConfigurationFiles(std::string configurationDir,
                       std::string systemConfigBlueprintFile,
                       std::string simulationConfigFile) :
        configurationDir{configurationDir},
        systemConfigBlueprintFile{openpass::core::Directories::Concat(configurationDir, systemConfigBlueprintFile)},
        simulationConfigFile{openpass::core::Directories::Concat(configurationDir, simulationConfigFile)}
    {}

    // This class should not be moved or assigned,
    // as there should be only one instance throughout the system
    ConfigurationFiles() = delete;
    ConfigurationFiles(const openpass::core::Directories&) = delete;
    ConfigurationFiles(openpass::core::Directories&&) = delete;
    ConfigurationFiles& operator=(openpass::core::Directories&&) = delete;

    const std::string configurationDir;            //!< directory of the configuration files
    const std::string systemConfigBlueprintFile;   //!< absolute path of the systemConfigBlueprintFile
    const std::string simulationConfigFile;        //!< absolute path of the simulationConfigFile
};
