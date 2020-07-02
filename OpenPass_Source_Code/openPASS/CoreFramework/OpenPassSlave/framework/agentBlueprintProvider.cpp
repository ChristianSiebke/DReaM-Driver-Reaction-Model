/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "agentBlueprintProvider.h"
#include "systemConfigImporter.h"
#include "dynamicProfileSampler.h"
#include "dynamicParametersSampler.h"
#include "agentBlueprint.h"

AgentBlueprintProvider::AgentBlueprintProvider(ConfigurationContainerInterface *configurationContainer, StochasticsInterface& stochastics) :
    stochastics(stochastics),
    profiles(configurationContainer->GetProfiles()),
    agentProfiles(configurationContainer->GetProfiles()->GetAgentProfiles()),
    vehicleModels(configurationContainer->GetVehicleModels()),
    systemConfigBlueprint(configurationContainer->GetSystemConfigBlueprint()),
    systemConfigs(configurationContainer->GetSystemConfigs())
{
}

AgentBlueprint AgentBlueprintProvider::SampleAgent(const std::string& agentProfileName, const openScenario::Parameters& assignedParameters) const
{
    AgentBlueprint agentBlueprint;

    try
    {
        auto agentProfile = agentProfiles.at(agentProfileName);
        if (agentProfile.type == AgentProfileType::Dynamic)
        {
            if(systemConfigBlueprint == nullptr)
            {
                LogErrorAndThrow("Couldn't instantiate AgentProfile:" + agentProfileName + ". SystemConfigBlueprint is either missing or invalid.");
            }

            SampledProfiles sampledProfiles = SampledProfiles::make(agentProfileName, stochastics, profiles)
                    .SampleDriverProfile()
                    .SampleVehicleProfile()
                    .SampleVehicleComponentProfiles();
            DynamicParameters dynamicParameters = DynamicParameters::make(stochastics, sampledProfiles.vehicleProfileName, profiles)
                    .SampleSensorLatencies();
            AgentBuildInformation agentBuildInformation = AgentBuildInformation::make(sampledProfiles, dynamicParameters, systemConfigBlueprint, profiles, vehicleModels)
                    .SetVehicleModelParameters(assignedParameters)
                    .GatherBasicComponents()
                    .GatherDriverComponents()
                    .GatherVehicleComponents()
                    .GatherSensors();
            GenerateDynamicAgentBlueprint(agentBlueprint, agentBuildInformation, sampledProfiles.driverProfileName);

            return agentBlueprint;
        }
        else
        {
            GenerateStaticAgentBlueprint(agentBlueprint, agentProfile.systemConfigFile, agentProfile.systemId, agentProfile.vehicleModel);

            return agentBlueprint;
        }
    }
    catch (const std::out_of_range& e)
    {
        std::string message{e.what()};
        message += ": Tried to sample undefined AgentProfile '" + agentProfileName + "'";
        LOG_INTERN(LogLevel::Error) << message;
        throw std::runtime_error(message);
    }
}

void AgentBlueprintProvider::GenerateDynamicAgentBlueprint(AgentBlueprintInterface &agentBlueprint, AgentBuildInformation agentBuildInformation, std::string &driverProfileName) const
{
    agentBlueprint.SetVehicleModelName(agentBuildInformation.vehicleModelName);
    agentBlueprint.SetVehicleModelParameters(agentBuildInformation.vehicleModelParameters);
    agentBlueprint.SetDriverProfileName(driverProfileName);
    agentBlueprint.SetAgentType(agentBuildInformation.agentType);

    for(const auto& sensor : agentBuildInformation.sensorParameters)
    {
        agentBlueprint.AddSensor(sensor);
    }
}

void AgentBlueprintProvider::GenerateStaticAgentBlueprint(AgentBlueprintInterface &agentBlueprint, std::string systemConfigName, int systemId, std::string vehicleModelName) const
{
    try
    {
        auto& systems = systemConfigs.at(systemConfigName)->GetSystems();
        agentBlueprint.SetAgentType(systems.at(systemId));
    }
    catch (const std::out_of_range& e)
    {
        const std::string message{"No system for specified id found in imported systemConfig: " + std::string(e.what())};
        LOG_INTERN(LogLevel::Error) << message;
        throw std::runtime_error(message);
    }

    agentBlueprint.SetVehicleModelName(vehicleModelName);
    auto vehicleModelParameters = vehicleModels->GetVehicleModel(vehicleModelName);
    agentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
}
