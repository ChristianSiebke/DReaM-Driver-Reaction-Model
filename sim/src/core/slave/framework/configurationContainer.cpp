/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  ConfigurationContainer.cpp */
//-----------------------------------------------------------------------------

#include "directories.h"
#include "configurationContainer.h"

using namespace Importer;

bool ConfigurationContainer::ImportAllConfigurations()
{
    //Import SystemConfigBlueprint
    systemConfigBlueprint = std::make_shared<SystemConfig>();
    if (!SystemConfigImporter::Import(configurationFiles.systemConfigBlueprintFile, systemConfigBlueprint))
    {
        LOG_INTERN(LogLevel::Info) << "could not import systemConfigBlueprint.";
        systemConfigBlueprint = nullptr;
    }

    //Import SlaveConfig
    if (!SlaveConfigImporter::Import(configurationFiles.configurationDir,
                                     configurationFiles.slaveConfigFile,
                                     slaveConfig))
    {
        LOG_INTERN(LogLevel::Error) << "could not import slave configuration '" << configurationFiles.slaveConfigFile << "'";
        return false;
    }

    //Import ProfilesCatalog
    if (!ProfilesImporter::Import(slaveConfig.GetProfilesCatalog(), profiles))
    {
        LOG_INTERN(LogLevel::Error) << "could not import profiles catalog";
        return false;
    }

    //Import Scenario
    if (!ScenarioImporter::Import(slaveConfig.GetScenarioConfig().scenarioPath, &scenario))
    {
        LOG_INTERN(LogLevel::Error) << "could not import scenario";
        return false;
    }

    //Import Scenery
    if (!SceneryImporter::Import(openpass::core::Directories::Concat(configurationFiles.configurationDir, scenario.GetSceneryPath()),
                                 &scenery))
    {
        LOG_INTERN(LogLevel::Error) << "could not import scenery";
        return false;
    }

    //Import VehicleModels
    std::string vehicleCatalogPath = "";
    if (!scenario.GetVehicleCatalogPath().empty())
    {
        vehicleCatalogPath = openpass::core::Directories::Concat(configurationFiles.configurationDir, scenario.GetVehicleCatalogPath());
    }
    std::string pedestrianCatalogPath = "";
    if (!scenario.GetPedestrianCatalogPath().empty())
    {
        pedestrianCatalogPath = openpass::core::Directories::Concat(configurationFiles.configurationDir, scenario.GetPedestrianCatalogPath());
    }
    if (!VehicleModelsImporter::Import(vehicleCatalogPath,pedestrianCatalogPath,vehicleModels))
    {
        LOG_INTERN(LogLevel::Error) << "could not import vehicle models";
        return false;
    }

    std::set<std::string> systemConfigFiles;
    std::transform(profiles.GetAgentProfiles().begin(), profiles.GetAgentProfiles().end(), std::inserter(systemConfigFiles, systemConfigFiles.begin()),
                   [](const auto& agentProfile){return agentProfile.second.systemConfigFile;});

    //Import SystemConfigs
    for (const auto& systemConfigFile : systemConfigFiles)
    {
        if (systemConfigFile == "")
        {
            continue;
        }
        auto systemConfig = std::make_shared<SystemConfig>();

        if(!SystemConfigImporter::Import(openpass::core::Directories::Concat(configurationFiles.configurationDir, systemConfigFile), systemConfig))
        {
            LOG_INTERN(LogLevel::Error) << "could not import system configs";
            return false;
        }
        systemConfigs.insert(std::make_pair(systemConfigFile, systemConfig));
    }

    return true;
}

std::shared_ptr<SystemConfigInterface> ConfigurationContainer::GetSystemConfigBlueprint()
{
    return systemConfigBlueprint;
}

SlaveConfigInterface* ConfigurationContainer::GetSlaveConfig()
{
    return &slaveConfig;
}

ProfilesInterface* ConfigurationContainer::GetProfiles()
{
    return &profiles;
}

SceneryInterface* ConfigurationContainer::GetScenery()
{
    return &scenery;
}

ScenarioInterface* ConfigurationContainer::GetScenario()
{
    return &scenario;
}

std::map<std::string, std::shared_ptr<SystemConfigInterface>>& ConfigurationContainer::GetSystemConfigs()
{
    return systemConfigs;
}

VehicleModelsInterface* ConfigurationContainer::GetVehicleModels()
{
    return &vehicleModels;
}

const openpass::common::RuntimeInformation& ConfigurationContainer::GetRuntimeInformation() const
{
    return runtimeInformation;
}
