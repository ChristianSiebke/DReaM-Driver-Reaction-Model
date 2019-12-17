/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "dynamicAgentTypeGenerator.h"

DynamicAgentTypeGenerator::DynamicAgentTypeGenerator(SampledProfiles& sampledProfiles,
        DynamicParameters& dynamicParameters, std::shared_ptr<SystemConfigInterface> systemConfigBlueprint, ProfilesInterface* profiles,
        VehicleModelsInterface* vehicleModels) :
    sampledProfiles(sampledProfiles),
    dynamicParameters(dynamicParameters),
    systemConfigBlueprint(systemConfigBlueprint),
    profiles(profiles),
    vehicleModels(vehicleModels)
{
}

DynamicAgentTypeGenerator& DynamicAgentTypeGenerator::GatherBasicComponents()
{
    for (const auto& component : defaultComponents.basicComponentNames)
    {
        GatherComponent(component, agentBuildInformation.agentType);
    }

    return *this;
}

DynamicAgentTypeGenerator& DynamicAgentTypeGenerator::GatherDriverComponents()
{
    auto driverParameters = profiles->GetDriverProfiles().at(sampledProfiles.driverProfileName);
    auto driverModule = openpass::parameter::Get<std::string>(driverParameters, "Type"); // Existence checked by ProfilesImporter

    if (auto driverParameterModule = openpass::parameter::Get<std::string>(driverParameters, "ParametersModule"))
    {
        GatherComponentWithParameters(driverParameterModule.value(), agentBuildInformation.agentType, driverParameters);
        GatherComponent(driverModule.value(), agentBuildInformation.agentType);
    }
    else
    {
        GatherComponentWithParameters(driverModule.value(), agentBuildInformation.agentType, driverParameters);
    }

    auto sensorDriverModule = openpass::parameter::Get<std::string>(driverParameters, "SensorDriverModule");
    GatherComponent(sensorDriverModule.value_or("Sensor_Driver"), agentBuildInformation.agentType);

    auto algorithmLateralModule = openpass::parameter::Get<std::string>(driverParameters, "AlgorithmLateralModule");
    GatherComponent(algorithmLateralModule.value_or("AlgorithmLateralDriver"), agentBuildInformation.agentType);

    auto algorithmLongitudinalModule = openpass::parameter::Get<std::string>(driverParameters, "AlgorithmLongitudinalModule");
    GatherComponent(algorithmLongitudinalModule.value_or("AlgorithmLongitudinalDriver"), agentBuildInformation.agentType);


    for (const auto& componentName : defaultComponents.driverComponentNames)
    {
        GatherComponent(componentName, agentBuildInformation.agentType);
    }

    return *this;
}

DynamicAgentTypeGenerator& DynamicAgentTypeGenerator::GatherVehicleComponents()
{
    std::shared_ptr<SimulationSlave::AgentType> agentType = agentBuildInformation.agentType;
    VehicleComponentProfileNames vehicleComponentProfileNames = sampledProfiles.vehicleComponentProfileNames;
    auto possibleVehicleComponentProfiles = profiles->GetVehicleComponentProfiles();

    if (!vehicleComponentProfileNames.empty())
    {
        for (std::string componentName : defaultComponents.vehicleComponentNames)
        {
            GatherComponent(componentName, agentType);
        }
    }

    for (auto vehicleComponentProfile : vehicleComponentProfileNames)
    {
        if (possibleVehicleComponentProfiles.count(vehicleComponentProfile.first) == 0)
        {
            throw std::logic_error("No vehicle component profile of type " + vehicleComponentProfile.first);
        }
        if (possibleVehicleComponentProfiles.at(vehicleComponentProfile.first).count(vehicleComponentProfile.second) == 0)
        {
            throw std::logic_error("No vehicle component profile of type " + vehicleComponentProfile.first + " with name " +
                                   vehicleComponentProfile.second);
        }
        auto parameters = possibleVehicleComponentProfiles.at(vehicleComponentProfile.first).at(
                              vehicleComponentProfile.second);

        if (profiles->GetVehicleProfiles().count(sampledProfiles.vehicleProfileName) == 0)
        {
            throw std::logic_error("No vehicle profile of type " + sampledProfiles.vehicleProfileName);
        }
        auto vehicleProfile = profiles->GetVehicleProfiles().at(sampledProfiles.vehicleProfileName);

        auto matchedComponent = std::find_if(vehicleProfile.vehicleComponents.begin(),
                                             vehicleProfile.vehicleComponents.end(), [vehicleComponentProfile](VehicleComponent curComponent)
        { return curComponent.type == vehicleComponentProfile.first; });

        openpass::parameter::internal::ParameterList parameterList;
        for (auto link : matchedComponent->sensorLinks)
        {
            openpass::parameter::internal::ParameterSet param;
            param.emplace_back("SensorId", link.sensorId);
            param.emplace_back("InputId", link.inputId);
            parameterList.emplace_back(param);
        }
        parameters.emplace_back("SensorLinks", parameterList);

        GatherComponentWithParameters(vehicleComponentProfile.first, agentType, parameters);
    }

    return *this;
}

DynamicAgentTypeGenerator& DynamicAgentTypeGenerator::GatherSensors()
{
    const std::string sensorFusionModulName = "SensorFusion";

    GatherComponent(sensorFusionModulName, agentBuildInformation.agentType);

    int inputIdSensorFusion = systemConfigBlueprint->GetSystems().at(0)->GetComponents().at(sensorFusionModulName)->GetInputLinks().at(0);
    int sensorNumber = 0;
    auto vehicleProfile = profiles->GetVehicleProfiles().at(sampledProfiles.vehicleProfileName);

    const auto sensorProfilesFromConfig = profiles->GetSensorProfiles();

    for (const auto& sensor : vehicleProfile.sensors)
    {
        const auto matchedSensorProfile = std::find_if(
            sensorProfilesFromConfig.begin(), sensorProfilesFromConfig.end(),
            [sensor](const auto& profile)
            {
              return profile.name == sensor.profile.name && profile.type == sensor.profile.type;
            });

        if (matchedSensorProfile == sensorProfilesFromConfig.end())
        {
            throw std::runtime_error("Could not find SensorProfile");
        }

        // clone first from profile
        auto sensorParam = openpass::parameter::Container(matchedSensorProfile->parameter);

        // add specific information
        sensorParam.emplace_back("Name", matchedSensorProfile->name);
        sensorParam.emplace_back("Type", matchedSensorProfile->type);
        sensorParam.emplace_back("Id", sensor.id);
        sensorParam.emplace_back("Longitudinal", sensor.position.longitudinal);
        sensorParam.emplace_back("Lateral", sensor.position.lateral);
        sensorParam.emplace_back("Height", sensor.position.height);
        sensorParam.emplace_back("Pitch", sensor.position.pitch);
        sensorParam.emplace_back("Yaw", sensor.position.yaw);
        sensorParam.emplace_back("Roll", sensor.position.roll);
        sensorParam.emplace_back("Latency", dynamicParameters.sensorLatencies.at(sensor.id));

        const std::string uniqueSensorName =  "Sensor_" + std::to_string(sensor.id);
        GatherComponentWithParameters(uniqueSensorName, agentBuildInformation.agentType, sensorParam,
                                      "SensorObjectDetector", sensorNumber);

        if (sensorNumber > 0)
        {
            agentBuildInformation.agentType->GetComponents().at(sensorFusionModulName)->AddInputLink(sensorNumber,
                    inputIdSensorFusion + sensorNumber);
        }

        // clone sensor and set specific parameters
        openpass::sensors::Parameter clonedSensor(sensor);
        clonedSensor.profile.parameter = sensorParam;
        agentBuildInformation.sensorParameters.push_back(clonedSensor);

        sensorNumber++;
    }

    return *this;
}

DynamicAgentTypeGenerator& DynamicAgentTypeGenerator::SetVehicleModelParameters()
{
    VehicleProfile vehicleProfile = profiles->GetVehicleProfiles().at(sampledProfiles.vehicleProfileName);
    agentBuildInformation.vehicleModelName = vehicleProfile.vehicleModel;
    agentBuildInformation.vehicleModelParameters = vehicleModels->GetVehicleModel(vehicleProfile.vehicleModel);

    return *this;
}

void DynamicAgentTypeGenerator::GatherComponent(const std::string componentName,
        std::shared_ptr<SimulationSlave::AgentType> agentType)
{
    auto componentType = std::make_shared<SimulationSlave::ComponentType>(*systemConfigBlueprint->GetSystems().at(
                             0)->GetComponents().at(componentName));

    for (const auto& componentOutput : componentType->GetOutputLinks())
    {
        agentType->AddChannel(componentOutput.second);
    }

    if (!agentType->AddComponent(componentType))
    {
        throw std::runtime_error("Could not add component to agentType");
    }
}

void DynamicAgentTypeGenerator::GatherComponentWithParameters(std::string componentName,
        std::shared_ptr<SimulationSlave::AgentType> agentType,
        const openpass::parameter::Container &parameters)
{
    GatherComponentWithParameters(componentName, agentType, parameters, componentName, 0);
}

void DynamicAgentTypeGenerator::GatherComponentWithParameters(std::string componentName,
        std::shared_ptr<SimulationSlave::AgentType> agentType,
        const openpass::parameter::Container &parameters, std::string componentNameInSystemConfigBlueprint, int channelOffset)
{
    if (systemConfigBlueprint->GetSystems().at(0)->GetComponents().count(componentNameInSystemConfigBlueprint) == 0)
    {
        throw std::logic_error("Could not find compenent " + componentNameInSystemConfigBlueprint + " in systemConfigBlueprint");
    }

    auto componentTypeFromSystemConfigBlueprint = systemConfigBlueprint->GetSystems().at(0)->GetComponents().at(componentNameInSystemConfigBlueprint);

    auto componentType = std::make_shared<SimulationSlave::ComponentType>(componentName,
                         componentTypeFromSystemConfigBlueprint->GetInit(),
                         componentTypeFromSystemConfigBlueprint->GetPriority(),
                         componentTypeFromSystemConfigBlueprint->GetOffsetTime(),
                         componentTypeFromSystemConfigBlueprint->GetResponseTime(),
                         componentTypeFromSystemConfigBlueprint->GetCycleTime(),
                         componentTypeFromSystemConfigBlueprint->GetModelLibrary());

    for (const auto& componentOutput : componentTypeFromSystemConfigBlueprint->GetOutputLinks())
    {
        componentType->AddOutputLink(componentOutput.first, componentOutput.second + channelOffset);
        agentType->AddChannel(componentOutput.second + channelOffset);
    }
    for (const auto& componentInput : componentTypeFromSystemConfigBlueprint->GetInputLinks())
    {
        componentType->AddInputLink(componentInput.first, componentInput.second + channelOffset);
    }

    componentType->SetModelParameter(parameters);

    if (!agentType->AddComponent(componentType))
    {
        throw std::runtime_error("Could not add component to agentType");
    }
}

DynamicAgentTypeGenerator AgentBuildInformation::make(SampledProfiles& sampledProfiles,
        DynamicParameters& dynamicParameters,
        std::shared_ptr<SystemConfigInterface> systemConfigBlueprint,
        ProfilesInterface* profiles,
        VehicleModelsInterface* vehicleModels)
{
    return DynamicAgentTypeGenerator(sampledProfiles, dynamicParameters, systemConfigBlueprint, profiles, vehicleModels);
}
