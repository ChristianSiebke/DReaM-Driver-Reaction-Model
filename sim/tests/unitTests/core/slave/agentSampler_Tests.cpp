/*******************************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeAgentBlueprint.h"
#include "fakeSlaveConfig.h"
#include "fakeProfiles.h"
#include "fakeConfigurationContainer.h"
#include "fakeStochastics.h"
#include "fakeVehicleModels.h"
#include "fakeParameter.h"
#include "fakeScenario.h"
#include "fakeAgentType.h"
#include "fakeSystemConfig.h"

#include "profiles.h"
#include "dynamicProfileSampler.h"
#include "dynamicAgentTypeGenerator.h"
#include "dynamicParametersSampler.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::NiceMock;

using namespace std::string_literals;

namespace op = openpass::parameter;



TEST(DynamicProfileSampler, SampleDriverProfile)
{
    NiceMock<FakeStochastics> fakeStochastics;
    NiceMock<FakeProfiles> fakeProfiles;
    StringProbabilities driverProbabilities {{"SomeDriver", 1.0}};
    ON_CALL(fakeProfiles, GetDriverProbabilities("SomeAgentProfile")).WillByDefault(ReturnRef(driverProbabilities));
    ON_CALL(fakeStochastics, GetUniformDistributed(_,_)).WillByDefault(Return(0));

    SampledProfiles sampledProfiles = SampledProfiles::make("SomeAgentProfile", fakeStochastics, &fakeProfiles)
            .SampleDriverProfile();

    ASSERT_THAT(sampledProfiles.driverProfileName, "SomeDriver");
}

TEST(DynamicProfileSampler, SampleVehicleProfile)
{
    NiceMock<FakeStochastics> fakeStochastics;
    NiceMock<FakeProfiles> fakeProfiles;
    StringProbabilities vehicleProbabilities {{"SomeVehicle", 1.0}};
    ON_CALL(fakeProfiles, GetVehicleProfileProbabilities("SomeAgentProfile")).WillByDefault(ReturnRef(vehicleProbabilities));
    ON_CALL(fakeStochastics, GetUniformDistributed(_,_)).WillByDefault(Return(0));

    SampledProfiles sampledProfiles = SampledProfiles::make("SomeAgentProfile", fakeStochastics, &fakeProfiles)
            .SampleVehicleProfile();

    ASSERT_THAT(sampledProfiles.vehicleProfileName, "SomeVehicle");
}

TEST(DynamicProfileSampler, SampleVehicleComponentProfiles)
{
    NiceMock<FakeStochastics> fakeStochastics;
    NiceMock<FakeProfiles> fakeProfiles;

    VehicleComponent someComponent;
    StringProbabilities probabilities = {{"SomeProfile", 1.0}};
    someComponent.type = "SomeComponent";
    someComponent.componentProfiles = probabilities;

    VehicleProfile someVehicleProfile;
    someVehicleProfile.vehicleComponents = {{someComponent}};
    std::unordered_map<std::string, VehicleProfile> vehicleProfiles {{"SomeVehicle", someVehicleProfile}};
    ON_CALL(fakeProfiles, GetVehicleProfiles()).WillByDefault(ReturnRef(vehicleProfiles));
    ON_CALL(fakeStochastics, GetUniformDistributed(_,_)).WillByDefault(Return(0));

    StringProbabilities vehicleProbabilities {{"SomeVehicle", 1.0}};
    ON_CALL(fakeProfiles, GetVehicleProfileProbabilities("SomeAgentProfile")).WillByDefault(ReturnRef(vehicleProbabilities));

    SampledProfiles sampledProfiles = SampledProfiles::make("SomeAgentProfile", fakeStochastics, &fakeProfiles)
            .SampleVehicleProfile()
            .SampleVehicleComponentProfiles();

    ASSERT_THAT(sampledProfiles.vehicleComponentProfileNames.at("SomeComponent"), "SomeProfile");
}

TEST(DynamicAgentTypeGenerator, GatherBasicComponents)
{
    NiceMock<FakeStochastics> fakeStochastics;
    SampledProfiles sampledProfiles = SampledProfiles::make("", fakeStochastics, nullptr);
    auto systemConfigBlueprint = std::make_shared<NiceMock<FakeSystemConfig>>();
    NiceMock<FakeProfiles> profiles;
    NiceMock<FakeVehicleModels> vehicleModels;
    DefaultComponents defaultComponents;
    DynamicParameters dynamicParameters = DynamicParameters::empty();

    auto fakeAgentType = std::make_shared<NiceMock<SimulationSlave::FakeAgentType>>();
    std::map<int, std::shared_ptr< SimulationSlave::AgentTypeInterface>> systems = {{0, fakeAgentType}};
    std::map<std::string, std::shared_ptr<SimulationSlave::ComponentType>> components{};

    for (const auto& component : defaultComponents.basicComponentNames)
    {
        components.insert(std::make_pair(component, std::make_shared<SimulationSlave::ComponentType>(component, false, 0, 0, 0, 0, "")));
    }

    ON_CALL(*fakeAgentType, GetComponents()).WillByDefault(ReturnRef(components));

    ON_CALL(*systemConfigBlueprint, GetSystems()).WillByDefault(ReturnRef(systems));

    AgentBuildInformation agentBuildInformation = AgentBuildInformation::make(sampledProfiles, dynamicParameters, systemConfigBlueprint, &profiles, &vehicleModels)
            .GatherBasicComponents();

    ASSERT_THAT(agentBuildInformation.agentType->GetComponents().size(), Eq(defaultComponents.basicComponentNames.size()));
    for (const auto& component : defaultComponents.basicComponentNames)
    {
        ASSERT_THAT(agentBuildInformation.agentType->GetComponents().count(component), Eq(1));
    }
}

TEST(DynamicAgentTypeGenerator, GatherDriverComponents)
{
    NiceMock<FakeStochastics> fakeStochastics;
    SampledProfiles sampledProfiles = SampledProfiles::make("", fakeStochastics, nullptr);
    auto systemConfigBlueprint = std::make_shared<NiceMock<FakeSystemConfig>>();
    NiceMock<FakeProfiles> profiles;
    NiceMock<FakeVehicleModels> vehicleModels;
    DefaultComponents defaultComponents;
    DynamicParameters dynamicParameters = DynamicParameters::empty();

    sampledProfiles.driverProfileName = "SomeDriverProfile";

    op::ParameterSetLevel1 driverProfile{
        {"Type"s, "SomeDriverModule"s},
        {"ParametersModule"s, "SomeParameters"s},
        {"SensorDriverModule"s, "SomeSensorDriver"s},
        {"AlgorithmLateralModule"s, "SomeAlgorithmLateral"s},
        {"AlgorithmLongitudinalModule"s, "SomeAlgorithmLongitudinal"s}
    };

    ON_CALL(profiles, GetProfile("Driver", "SomeDriverProfile")).WillByDefault(Return(driverProfile));

    auto fakeAgentType = std::make_shared<NiceMock<SimulationSlave::FakeAgentType>>();
    std::map<int, std::shared_ptr< SimulationSlave::AgentTypeInterface>> systems = {{0, fakeAgentType}};
    std::map<std::string, std::shared_ptr<SimulationSlave::ComponentType>> components{};

    for (const auto& component : defaultComponents.driverComponentNames)
    {
        components.insert(std::make_pair(component, std::make_shared<SimulationSlave::ComponentType>(component, false, 0, 0, 0, 0, "")));
    }
    components.insert(std::make_pair("SomeParameters", std::make_shared<SimulationSlave::ComponentType>("SomeParameters", false, 0, 0, 0, 0, "")));
    components.insert(std::make_pair("SomeSensorDriver", std::make_shared<SimulationSlave::ComponentType>("SomeSensorDriver", false, 0, 0, 0, 0, "")));
    components.insert(std::make_pair("SomeAlgorithmLateral", std::make_shared<SimulationSlave::ComponentType>("SomeAlgorithmLateral", false, 0, 0, 0, 0, "")));
    components.insert(std::make_pair("SomeAlgorithmLongitudinal", std::make_shared<SimulationSlave::ComponentType>("SomeAlgorithmLongitudinal", false, 0, 0, 0, 0, "")));
    components.insert(std::make_pair("SomeDriverModule", std::make_shared<SimulationSlave::ComponentType>("SomeDriverModule", false, 0, 0, 0, 0, "")));

    ON_CALL(*fakeAgentType, GetComponents()).WillByDefault(ReturnRef(components));

    ON_CALL(*systemConfigBlueprint, GetSystems()).WillByDefault(ReturnRef(systems));

    AgentBuildInformation agentBuildInformation = AgentBuildInformation::make(sampledProfiles, dynamicParameters, systemConfigBlueprint, &profiles, &vehicleModels)
            .GatherDriverComponents();

    ASSERT_THAT(agentBuildInformation.agentType->GetComponents().size(), Eq(defaultComponents.driverComponentNames.size() + 5));
    for (const auto& component : defaultComponents.driverComponentNames)
    {
        ASSERT_THAT(agentBuildInformation.agentType->GetComponents().count(component), Eq(1));
    }
    ASSERT_THAT(agentBuildInformation.agentType->GetComponents().count("SomeParameters"), Eq(1));
    ASSERT_THAT(agentBuildInformation.agentType->GetComponents().count("SomeSensorDriver"), Eq(1));
    ASSERT_THAT(agentBuildInformation.agentType->GetComponents().count("SomeAlgorithmLateral"), Eq(1));
    ASSERT_THAT(agentBuildInformation.agentType->GetComponents().count("SomeAlgorithmLongitudinal"), Eq(1));
    ASSERT_THAT(agentBuildInformation.agentType->GetComponents().count("SomeDriverModule"), Eq(1));
}

TEST(DynamicAgentTypeGenerator, GatherVehicleComponents)
{
    NiceMock<FakeStochastics> fakeStochastics;
    SampledProfiles sampledProfiles = SampledProfiles::make("", fakeStochastics, nullptr);
    auto systemConfigBlueprint = std::make_shared<NiceMock<FakeSystemConfig>>();
    NiceMock<FakeProfiles> profiles;
    NiceMock<FakeVehicleModels> vehicleModels;
    DefaultComponents defaultComponents;
    DynamicParameters dynamicParameters = DynamicParameters::empty();

    sampledProfiles.vehicleProfileName = "SomeVehicle";
    sampledProfiles.vehicleComponentProfileNames = {{"VehicleComponentA", "ProfileA"}, {"VehicleComponentB", "ProfileB"}};

    op::ParameterSetLevel1 parametersAA {{"aa", 0}};
    op::ParameterSetLevel1 parametersAB {{"ab", 0}};
    op::ParameterSetLevel1 parametersBA {{"ba", 0}};
    op::ParameterSetLevel1 parametersBB {{"bb", 0}};

    ON_CALL(profiles, GetProfile("VehicleComponentA", "ProfileA")).WillByDefault(Return(parametersAA));
    ON_CALL(profiles, GetProfile("VehicleComponentA", "ProfileB")).WillByDefault(Return(parametersAB));
    ON_CALL(profiles, GetProfile("VehicleComponentB", "ProfileA")).WillByDefault(Return(parametersBA));
    ON_CALL(profiles, GetProfile("VehicleComponentB", "ProfileB")).WillByDefault(Return(parametersBB));

    VehicleComponent vehicleComponentA;
    vehicleComponentA.type = "VehicleComponentA";

    VehicleComponent vehicleComponentB;
    vehicleComponentB.type = "VehicleComponentB";

    VehicleProfile vehicleProfile;
    vehicleProfile.vehicleComponents = {{vehicleComponentA, vehicleComponentB}};

    std::unordered_map<std::string, VehicleProfile> vehicleProfiles {{"SomeVehicle", vehicleProfile}};
    ON_CALL(profiles, GetVehicleProfiles()).WillByDefault(ReturnRef(vehicleProfiles));

    auto fakeAgentType = std::make_shared<NiceMock<SimulationSlave::FakeAgentType>>();
    std::map<int, std::shared_ptr< SimulationSlave::AgentTypeInterface>> systems = {{0, fakeAgentType}};
    std::map<std::string, std::shared_ptr<SimulationSlave::ComponentType>> components{};

    for (const auto& component : defaultComponents.vehicleComponentNames)
    {
        components.insert(std::make_pair(component, std::make_shared<SimulationSlave::ComponentType>(component, false, 0, 0, 0, 0, "")));
    }
    components.insert(std::make_pair("VehicleComponentA", std::make_shared<SimulationSlave::ComponentType>("VehicleComponentA", false, 0, 0, 0, 0, "")));
    components.insert(std::make_pair("VehicleComponentB", std::make_shared<SimulationSlave::ComponentType>("VehicleComponentB", false, 0, 0, 0, 0, "")));

    ON_CALL(*fakeAgentType, GetComponents()).WillByDefault(ReturnRef(components));
    ON_CALL(*systemConfigBlueprint, GetSystems()).WillByDefault(ReturnRef(systems));

    AgentBuildInformation agentBuildInformation = AgentBuildInformation::make(sampledProfiles, dynamicParameters, systemConfigBlueprint, &profiles, &vehicleModels)
            .GatherVehicleComponents();

    ASSERT_THAT(agentBuildInformation.agentType->GetComponents().size(), Eq(defaultComponents.vehicleComponentNames.size() + 2));
    for (const auto& component : defaultComponents.vehicleComponentNames)
    {
        ASSERT_THAT(agentBuildInformation.agentType->GetComponents().count(component), Eq(1));
    }
    ASSERT_THAT(agentBuildInformation.agentType->GetComponents().count("VehicleComponentA"), Eq(1));
    auto parameterA = agentBuildInformation.agentType->GetComponents().at("VehicleComponentA")->GetModelParameters();
    EXPECT_THAT(op::Get<int>(parameterA, "aa").has_value(), true);

    ASSERT_THAT(agentBuildInformation.agentType->GetComponents().count("VehicleComponentB"), Eq(1));
    auto parameterB = agentBuildInformation.agentType->GetComponents().at("VehicleComponentB")->GetModelParameters();
    EXPECT_THAT(op::Get<int>(parameterB, "bb").has_value(), true);
}

TEST(DynamicAgentTypeGenerator, GatherSensors)
{
    NiceMock<FakeStochastics> fakeStochastics;
    SampledProfiles sampledProfiles = SampledProfiles::make("", fakeStochastics, nullptr);
    auto systemConfigBlueprint = std::make_shared<NiceMock<FakeSystemConfig>>();
    NiceMock<FakeProfiles> profiles;
    NiceMock<FakeVehicleModels> vehicleModels;
    DefaultComponents defaultComponents;
    DynamicParameters dynamicParameters = DynamicParameters::empty();
    dynamicParameters.sensorLatencies = {{5, 1.0}, {7, 2.0}};

    sampledProfiles.vehicleProfileName = "SomeVehicle";

    openpass::sensors::Profile sensorProfileA;
    sensorProfileA.name = "ProfileA";
    sensorProfileA.type = "SensorTypeA";

    openpass::sensors::Profile sensorProfileB;
    sensorProfileB.name = "ProfileB";
    sensorProfileB.type = "SensorTypeB";

    openpass::parameter::ParameterSetLevel1 parameter;

    ON_CALL(profiles, GetProfile("SensorTypeA", "ProfileA")).WillByDefault(Return(parameter));
    ON_CALL(profiles, GetProfile("SensorTypeB", "ProfileB")).WillByDefault(Return(parameter));

    VehicleProfile vehicleProfile;

    openpass::sensors::Parameter sensorA;
    sensorA.id = 5;
    sensorA.profile = sensorProfileA;

    openpass::sensors::Position positionA;
    positionA.longitudinal = 2.0;
    sensorA.position = positionA;

    openpass::sensors::Parameter sensorB;
    sensorB.id = 7;
    sensorB.profile = sensorProfileB;

    openpass::sensors::Position positionB;
    positionB.longitudinal = 3.0;
    sensorB.position = positionB;
    vehicleProfile.sensors = {sensorA, sensorB};

    std::unordered_map<std::string, VehicleProfile> vehicleProfiles {{"SomeVehicle", vehicleProfile}};
    ON_CALL(profiles, GetVehicleProfiles()).WillByDefault(ReturnRef(vehicleProfiles));

    auto fakeAgentType = std::make_shared<NiceMock<SimulationSlave::FakeAgentType>>();
    std::map<int, std::shared_ptr<SimulationSlave::AgentTypeInterface>> systems = {{0, fakeAgentType}};
    std::map<std::string, std::shared_ptr<SimulationSlave::ComponentType>> components{};

    auto sensorAggregation = std::make_shared<SimulationSlave::ComponentType>("SensorAggregation", false, 0, 0, 0, 0, "SensorAggregation");
    sensorAggregation->AddInputLink(0, 100);
    components.insert(std::make_pair("SensorAggregation", sensorAggregation));

    auto sensorObjectDetector = std::make_shared<SimulationSlave::ComponentType>("SensorObjectDetector", false, 0, 0, 0, 0, "SensorObjectDetector");
    sensorObjectDetector->AddOutputLink(3, 100);
    components.insert(std::make_pair("SensorObjectDetector", sensorObjectDetector));

    ON_CALL(*fakeAgentType, GetComponents()).WillByDefault(ReturnRef(components));
    ON_CALL(*systemConfigBlueprint, GetSystems()).WillByDefault(ReturnRef(systems));

    AgentBuildInformation agentBuildInformation = AgentBuildInformation::make(sampledProfiles, dynamicParameters, systemConfigBlueprint, &profiles, &vehicleModels)
            .GatherSensors();

    const auto& gatheredComponents = agentBuildInformation.agentType->GetComponents();

    ASSERT_THAT(gatheredComponents.size(), Eq(3));
    ASSERT_THAT(gatheredComponents.count("SensorAggregation"), Eq(1));
    ASSERT_THAT(gatheredComponents.count("Sensor_5"), Eq(1));
    ASSERT_THAT(gatheredComponents.count("Sensor_7"), Eq(1));

    ASSERT_THAT(gatheredComponents.at("SensorAggregation")->GetInputLinks().size(), Eq(2));
    ASSERT_THAT(gatheredComponents.at("SensorAggregation")->GetModelLibrary(), Eq("SensorAggregation"));
    ASSERT_THAT(gatheredComponents.at("SensorAggregation")->GetInputLinks().at(0), Eq(100));
    ASSERT_THAT(gatheredComponents.at("SensorAggregation")->GetInputLinks().at(1), Eq(101));

    ASSERT_THAT(gatheredComponents.at("Sensor_5")->GetModelLibrary(), Eq("SensorObjectDetector"));
    ASSERT_THAT(gatheredComponents.at("Sensor_5")->GetOutputLinks().at(3), Eq(100));

    ASSERT_THAT(gatheredComponents.at("Sensor_7")->GetModelLibrary(), Eq("SensorObjectDetector"));
    ASSERT_THAT(gatheredComponents.at("Sensor_7")->GetOutputLinks().at(3), Eq(101));
}

TEST(DynamicAgentTypeGenerator, SetVehicleModelParameters)
{
    NiceMock<FakeStochastics> fakeStochastics;
    SampledProfiles sampledProfiles = SampledProfiles::make("", fakeStochastics, nullptr);
    auto systemConfigBlueprint = std::make_shared<NiceMock<FakeSystemConfig>>();
    NiceMock<FakeProfiles> profiles;
    NiceMock<FakeVehicleModels> vehicleModels;
    DynamicParameters dynamicParameters = DynamicParameters::empty();

    sampledProfiles.vehicleProfileName = "SomeVehicle";

    VehicleProfile vehicleProfile;
    vehicleProfile.vehicleModel = "SomeVehicleModel";
    std::unordered_map<std::string, VehicleProfile> vehicleProfiles{{"SomeVehicle", vehicleProfile}};
    ON_CALL(profiles, GetVehicleProfiles()).WillByDefault(ReturnRef(vehicleProfiles));

    VehicleModelParameters vehicleModelParameters;
    vehicleModelParameters.boundingBoxDimensions.length = 5.0;
    vehicleModelParameters.boundingBoxDimensions.width = 2.0;
    ON_CALL(vehicleModels, GetVehicleModel("SomeVehicleModel", _)).WillByDefault(Return(vehicleModelParameters));
    openScenario::Parameters assignedParameters;

    AgentBuildInformation agentBuildInformation =
            AgentBuildInformation::make(sampledProfiles, dynamicParameters, systemConfigBlueprint, &profiles, &vehicleModels)
            .SetVehicleModelParameters(assignedParameters);

    ASSERT_THAT(agentBuildInformation.vehicleModelName, Eq("SomeVehicleModel"));
    ASSERT_THAT(agentBuildInformation.vehicleModelParameters.boundingBoxDimensions.length, Eq(5.0));
    ASSERT_THAT(agentBuildInformation.vehicleModelParameters.boundingBoxDimensions.width, Eq(2.0));
}

TEST(DynamicParametersSampler, SampleSensorLatencies)
{
    NiceMock<FakeStochastics> fakeStochastics;
    ON_CALL(fakeStochastics, GetNormalDistributed(_,_)).WillByDefault(Return(10));

    std::string vehicleProfileName ="SomeVehicle";
    openpass::sensors::Parameter sensorParameter;
    sensorParameter.id = 5;
    sensorParameter.profile.name = "SomeProfile";
    sensorParameter.profile.type = "SomeSensorType";

    VehicleProfile vehicleProfile;
    vehicleProfile.sensors = {{sensorParameter}};

    Profiles profiles;
    profiles.GetVehicleProfiles() = {{vehicleProfileName, vehicleProfile}};

    openpass::parameter::ParameterSetLevel1 parameter{{"Latency", op::NormalDistribution{10.0, 0.0, 5.0, 15.0}}};
    ProfileGroup profileGroup{{"SomeProfile", parameter}};
    ProfileGroups profileGroups{{"SomeSensorType", profileGroup}};

    profiles.GetProfileGroups() = profileGroups;

    DynamicParameters dynamicParameters = DynamicParameters::make(fakeStochastics, vehicleProfileName, &profiles).SampleSensorLatencies();

    ASSERT_THAT(dynamicParameters.sensorLatencies.at(5), DoubleEq(10.0));
}
