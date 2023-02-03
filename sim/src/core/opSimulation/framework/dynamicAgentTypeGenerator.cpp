/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "dynamicAgentTypeGenerator.h"

constexpr char DRIVER[] = "Driver";

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
    auto driverParameters = profiles->GetProfile(DRIVER, sampledProfiles.driverProfileName);
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

    auto globalObserverModule = openpass::parameter::Get<std::string>(driverParameters, "GlobalObserver");
    GatherComponent(globalObserverModule.value(), agentBuildInformation.agentType);

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
    std::shared_ptr<core::AgentType> agentType = agentBuildInformation.agentType;
    VehicleComponentProfileNames vehicleComponentProfileNames = sampledProfiles.vehicleComponentProfileNames;

    if (!vehicleComponentProfileNames.empty())
    {
        for (std::string componentName : defaultComponents.vehicleComponentNames)
        {
            GatherComponent(componentName, agentType);
        }
    }

    for (auto vehicleComponentProfile : vehicleComponentProfileNames)
    {
        auto parameters = profiles->GetProfile(vehicleComponentProfile.first, vehicleComponentProfile.second);

        if (profiles->GetVehicleProfiles().count(sampledProfiles.vehicleProfileName) == 0)
        {
            throw std::logic_error("No vehicle profile of type " + sampledProfiles.vehicleProfileName);
        }
        auto vehicleProfile = profiles->GetVehicleProfiles().at(sampledProfiles.vehicleProfileName); // Existence checked by DynamicProfileSampler

        auto matchedComponent = std::find_if(vehicleProfile.vehicleComponents.begin(),
                                             vehicleProfile.vehicleComponents.end(), [vehicleComponentProfile](VehicleComponent curComponent)
        { return curComponent.type == vehicleComponentProfile.first; });

        openpass::parameter::internal::ParameterListLevel2 parameterList;
        for (auto link : matchedComponent->sensorLinks)
        {
            openpass::parameter::internal::ParameterSetLevel3 param;
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
    const std::string sensorAggregationModulName = "SensorAggregation";

    GatherComponent(sensorAggregationModulName, agentBuildInformation.agentType);

    int inputIdSensorAggregation = systemConfigBlueprint->GetSystems().at(0)->GetComponents().at(sensorAggregationModulName)->GetInputLinks().at(0);
    int sensorNumber = 0;
    auto vehicleProfile = profiles->GetVehicleProfiles().at(sampledProfiles.vehicleProfileName); // Existence checked by DynamicProfileSampler

    for (const auto& sensor : vehicleProfile.sensors)
    {
        const auto matchedSensorProfile = profiles->GetProfile(sensor.profile.type, sensor.profile.name);

        // clone first from profile
        auto sensorParam = matchedSensorProfile;

        // add specific information
        sensorParam.emplace_back("Name", sensor.profile.name);
        sensorParam.emplace_back("Type", sensor.profile.type);
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
            agentBuildInformation.agentType->GetComponents().at(sensorAggregationModulName)->AddInputLink(sensorNumber,
                    inputIdSensorAggregation + sensorNumber);
        }

        // clone sensor and set specific parameters
        openpass::sensors::Parameter clonedSensor(sensor);
        clonedSensor.profile.parameter = sensorParam;
        agentBuildInformation.sensorParameters.push_back(clonedSensor);

        sensorNumber++;
    }

    return *this;
}

DynamicAgentTypeGenerator& DynamicAgentTypeGenerator::SetVehicleModelParameters(const openScenario::Parameters& assignedParameters)
{
    VehicleProfile vehicleProfile = profiles->GetVehicleProfiles().at(sampledProfiles.vehicleProfileName); // Existence checked by DynamicProfileSampler
    agentBuildInformation.vehicleModelName = vehicleProfile.vehicleModel;
    agentBuildInformation.vehicleModelParameters = vehicleModels->GetVehicleModel(vehicleProfile.vehicleModel, assignedParameters);

    return *this;
}

void DynamicAgentTypeGenerator::GatherComponent(const std::string componentName,
        std::shared_ptr<core::AgentType> agentType)
{
    auto& systemComponents = systemConfigBlueprint->GetSystems().at(0)->GetComponents();
    auto find_result = systemComponents.find(componentName);
    if (find_result == systemComponents.end())
    {
        throw std::runtime_error("Could not find component " + componentName + " in systemConfigBlueprint");
    }
    auto componentType = std::make_shared<core::ComponentType>(*(find_result->second));

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
        std::shared_ptr<core::AgentType> agentType,
        const openpass::parameter::ParameterSetLevel1 &parameters)
{
    GatherComponentWithParameters(componentName, agentType, parameters, componentName, 0);
}

void DynamicAgentTypeGenerator::GatherComponentWithParameters(std::string componentName,
        std::shared_ptr<core::AgentType> agentType,
        const openpass::parameter::ParameterSetLevel1 &parameters, std::string componentNameInSystemConfigBlueprint, int channelOffset)
{
    auto& systemComponents = systemConfigBlueprint->GetSystems().at(0)->GetComponents();
    auto find_result = systemComponents.find(componentNameInSystemConfigBlueprint);
    if (find_result == systemComponents.end())
    {
        throw std::runtime_error("Could not find component " + componentNameInSystemConfigBlueprint + " in systemConfigBlueprint");
    }
    auto componentTypeFromSystemConfigBlueprint = std::make_shared<core::ComponentType>(*(find_result->second));

    auto componentType = std::make_shared<core::ComponentType>(componentName,
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
