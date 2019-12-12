/*********************************************************************
* Copyright (c) 2018 - 2019 in-tech GmbH on behalf of BMW
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeAgentBlueprint.h"
#include "fakeSlaveConfig.h"
#include "fakeProfiles.h"
#include "fakeConfigurationContainer.h"
#include "fakeSampler.h"
#include "fakeVehicleModels.h"
#include "fakeParameter.h"
#include "fakeScenario.h"
#include "fakeAgentType.h"
#include "fakeSystemConfig.h"

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
    NiceMock<FakeSampler> fakeSampler;
    NiceMock<FakeProfiles> fakeProfiles;
    std::unordered_map<std::string, double> driverProbabilities {{"SomeDriver", 1.0}};
    ON_CALL(fakeProfiles, GetDriverProbabilities("SomeAgentProfile")).WillByDefault(ReturnRef(driverProbabilities));
    ON_CALL(fakeSampler, SampleStringProbability(driverProbabilities)).WillByDefault(Return("SomeDriver"));

    SampledProfiles sampledProfiles = SampledProfiles::make("SomeAgentProfile", fakeSampler, &fakeProfiles)
            .SampleDriverProfile();

    ASSERT_THAT(sampledProfiles.driverProfileName, "SomeDriver");
}

TEST(DynamicProfileSampler, SampleVehicleProfile)
{
    NiceMock<FakeSampler> fakeSampler;
    NiceMock<FakeProfiles> fakeProfiles;
    std::unordered_map<std::string, double> vehicleProbabilities {{"SomeVehicle", 1.0}};
    ON_CALL(fakeProfiles, GetVehicleProfileProbabilities("SomeAgentProfile")).WillByDefault(ReturnRef(vehicleProbabilities));
    ON_CALL(fakeSampler, SampleStringProbability(vehicleProbabilities)).WillByDefault(Return("SomeVehicle"));

    SampledProfiles sampledProfiles = SampledProfiles::make("SomeAgentProfile", fakeSampler, &fakeProfiles)
            .SampleVehicleProfile();

    ASSERT_THAT(sampledProfiles.vehicleProfileName, "SomeVehicle");
}

TEST(DynamicProfileSampler, SampleVehicleComponentProfiles)
{
    NiceMock<FakeSampler> fakeSampler;
    NiceMock<FakeProfiles> fakeProfiles;

    VehicleComponent someComponent;
    std::unordered_map<std::string, double> probabilities = {{"SomeProfile", 1.0}};
    someComponent.type = "SomeComponent";
    someComponent.componentProfiles = probabilities;

    VehicleProfile someVehicleProfile;
    someVehicleProfile.vehicleComponents = {{someComponent}};
    std::unordered_map<std::string, VehicleProfile> vehicleProfiles {{"SomeVehicle", someVehicleProfile}};
    ON_CALL(fakeProfiles, GetVehicleProfiles()).WillByDefault(ReturnRef(vehicleProfiles));
    ON_CALL(fakeSampler, SampleStringProbability(probabilities)).WillByDefault(Return("SomeProfile"));

    std::unordered_map<std::string, double> vehicleProbabilities {{"SomeVehicle", 1.0}};
    ON_CALL(fakeProfiles, GetVehicleProfileProbabilities("SomeAgentProfile")).WillByDefault(ReturnRef(vehicleProbabilities));
    ON_CALL(fakeSampler, SampleStringProbability(vehicleProbabilities)).WillByDefault(Return("SomeVehicle"));

    SampledProfiles sampledProfiles = SampledProfiles::make("SomeAgentProfile", fakeSampler, &fakeProfiles)
            .SampleVehicleProfile()
            .SampleVehicleComponentProfiles();

    ASSERT_THAT(sampledProfiles.vehicleComponentProfileNames.at("SomeComponent"), "SomeProfile");
}

TEST(DynamicAgentTypeGenerator, GatherBasicComponents)
{
    NiceMock<FakeSampler> fakeSampler;
    SampledProfiles sampledProfiles = SampledProfiles::make("", fakeSampler, nullptr);
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
    NiceMock<FakeSampler> fakeSampler;
    SampledProfiles sampledProfiles = SampledProfiles::make("", fakeSampler, nullptr);
    auto systemConfigBlueprint = std::make_shared<NiceMock<FakeSystemConfig>>();
    NiceMock<FakeProfiles> profiles;
    NiceMock<FakeVehicleModels> vehicleModels;
    DefaultComponents defaultComponents;
    DynamicParameters dynamicParameters = DynamicParameters::empty();

    sampledProfiles.driverProfileName = "SomeDriverProfile";

    op::Container driverProfile{
        {"Type"s, "SomeDriverModule"s},
        {"ParametersModule"s, "SomeParameters"s},
        {"SensorDriverModule"s, "SomeSensorDriver"s},
        {"AlgorithmLateralModule"s, "SomeAlgorithmLateral"s},
        {"AlgorithmLongitudinalModule"s, "SomeAlgorithmLongitudinal"s}
    };

    DriverProfiles driverProfiles{{"SomeDriverProfile", driverProfile}};
    ON_CALL(profiles, GetDriverProfiles()).WillByDefault(ReturnRef(driverProfiles));

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
    NiceMock<FakeSampler> fakeSampler;
    SampledProfiles sampledProfiles = SampledProfiles::make("", fakeSampler, nullptr);
    auto systemConfigBlueprint = std::make_shared<NiceMock<FakeSystemConfig>>();
    NiceMock<FakeProfiles> profiles;
    NiceMock<FakeVehicleModels> vehicleModels;
    DefaultComponents defaultComponents;
    DynamicParameters dynamicParameters = DynamicParameters::empty();

    sampledProfiles.vehicleProfileName = "SomeVehicle";
    sampledProfiles.vehicleComponentProfileNames = {{"VehicleComponentA", "ProfileA"}, {"VehicleComponentB", "ProfileB"}};

    op::Container parametersAA {{"aa", 0}};
    op::Container parametersAB {{"ab", 0}};
    op::Container parametersBA {{"ba", 0}};
    op::Container parametersBB {{"bb", 0}};

    VehicleComponentProfiles profilesComponentA {
        {"ProfileA", parametersAA},
        {"ProfileB", parametersAB}
    };
    VehicleComponentProfiles profilesComponentB {
        {"ProfileA", parametersBA},
        {"ProfileB", parametersBB}
    };

    std::unordered_map<std::string, VehicleComponentProfiles> vehicleComponentProfiles {
        {"VehicleComponentA", profilesComponentA},
        {"VehicleComponentB", profilesComponentB}
    };

    ON_CALL(profiles, GetVehicleComponentProfiles()).WillByDefault(ReturnRef(vehicleComponentProfiles));

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
    NiceMock<FakeSampler> fakeSampler;
    SampledProfiles sampledProfiles = SampledProfiles::make("", fakeSampler, nullptr);
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

    openpass::sensors::Profiles sensorProfiles {{sensorProfileA, sensorProfileB}};
    ON_CALL(profiles, GetSensorProfiles()).WillByDefault(ReturnRef(sensorProfiles));

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

    auto sensorFusion = std::make_shared<SimulationSlave::ComponentType>("SensorFusion", false, 0, 0, 0, 0, "SensorFusion");
    sensorFusion->AddInputLink(0, 100);
    components.insert(std::make_pair("SensorFusion", sensorFusion));

    auto sensorObjectDetector = std::make_shared<SimulationSlave::ComponentType>("SensorObjectDetector", false, 0, 0, 0, 0, "SensorObjectDetector");
    sensorObjectDetector->AddOutputLink(3, 100);
    components.insert(std::make_pair("SensorObjectDetector", sensorObjectDetector));

    ON_CALL(*fakeAgentType, GetComponents()).WillByDefault(ReturnRef(components));
    ON_CALL(*systemConfigBlueprint, GetSystems()).WillByDefault(ReturnRef(systems));

    AgentBuildInformation agentBuildInformation = AgentBuildInformation::make(sampledProfiles, dynamicParameters, systemConfigBlueprint, &profiles, &vehicleModels)
            .GatherSensors();

    const auto& gatheredComponents = agentBuildInformation.agentType->GetComponents();

    ASSERT_THAT(gatheredComponents.size(), Eq(3));
    ASSERT_THAT(gatheredComponents.count("SensorFusion"), Eq(1));
    ASSERT_THAT(gatheredComponents.count("Sensor_5"), Eq(1));
    ASSERT_THAT(gatheredComponents.count("Sensor_7"), Eq(1));

    ASSERT_THAT(gatheredComponents.at("SensorFusion")->GetInputLinks().size(), Eq(2));
    ASSERT_THAT(gatheredComponents.at("SensorFusion")->GetModelLibrary(), Eq("SensorFusion"));
    ASSERT_THAT(gatheredComponents.at("SensorFusion")->GetInputLinks().at(0), Eq(100));
    ASSERT_THAT(gatheredComponents.at("SensorFusion")->GetInputLinks().at(1), Eq(101));

    ASSERT_THAT(gatheredComponents.at("Sensor_5")->GetModelLibrary(), Eq("SensorObjectDetector"));
    ASSERT_THAT(gatheredComponents.at("Sensor_5")->GetOutputLinks().at(3), Eq(100));

    ASSERT_THAT(gatheredComponents.at("Sensor_7")->GetModelLibrary(), Eq("SensorObjectDetector"));
    ASSERT_THAT(gatheredComponents.at("Sensor_7")->GetOutputLinks().at(3), Eq(101));
}

TEST(DynamicAgentTypeGenerator, SetVehicleModelParameters)
{
    NiceMock<FakeSampler> fakeSampler;
    SampledProfiles sampledProfiles = SampledProfiles::make("", fakeSampler, nullptr);
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
    vehicleModelParameters.length = 5.0;
    vehicleModelParameters.width = 2.0;
    ON_CALL(vehicleModels, GetVehicleModel("SomeVehicleModel")).WillByDefault(Return(vehicleModelParameters));

    AgentBuildInformation agentBuildInformation =
            AgentBuildInformation::make(sampledProfiles, dynamicParameters, systemConfigBlueprint, &profiles, &vehicleModels)
            .SetVehicleModelParameters();

    ASSERT_THAT(agentBuildInformation.vehicleModelName, Eq("SomeVehicleModel"));
    ASSERT_THAT(agentBuildInformation.vehicleModelParameters.length, Eq(5.0));
    ASSERT_THAT(agentBuildInformation.vehicleModelParameters.width, Eq(2.0));
}

TEST(DynamicParametersSampler, SampleSensorLatencies)
{
    NiceMock<FakeSampler> sampler;
    ON_CALL(sampler, RollForStochasticAttribute(DoubleEq(10.0),_,_,_)).WillByDefault(Return(10.0));

    std::string vehicleProfileName ="SomeVehicle";
    openpass::sensors::Parameter sensorParameter;
    sensorParameter.id = 5;
    sensorParameter.profile.name = "SomeProfile";
    sensorParameter.profile.type = "SomeSensorType";
    VehicleProfile vehicleProfile;
    vehicleProfile.sensors = {{sensorParameter}};
    std::unordered_map<std::string, VehicleProfile> vehicleProfiles{{vehicleProfileName, vehicleProfile}};

    openpass::sensors::Profile sensorProfile;
    sensorProfile.name = "SomeProfile";
    sensorProfile.type = "SomeSensorType";
    sensorProfile.parameter = op::Container{ {"Latency", op::NormalDistribution{10.0, 0.0, 0.0, 0.0}} };
    openpass::sensors::Profiles sensorProfiles{sensorProfile};

    DynamicParameters dynamicParameters = DynamicParameters::make(sampler, vehicleProfileName, vehicleProfiles, sensorProfiles)
            .SampleSensorLatencies();

    ASSERT_THAT(dynamicParameters.sensorLatencies.at(5), DoubleEq(10.0));
}
