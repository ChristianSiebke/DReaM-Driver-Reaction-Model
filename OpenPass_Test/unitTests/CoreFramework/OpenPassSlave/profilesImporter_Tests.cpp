/*********************************************************************
* Copyright (c) 2019 in-tech GmbH on behalf of BMW
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/helper/importerHelper.h"

#include "profilesImporter.h"

using ::testing::Eq;
using ::testing::SizeIs;
using ::testing::UnorderedElementsAre;

using namespace Importer;

TEST(ProfilesImporter_ImportAllVehicleComponentsOfVehicleProfile, GivenValidXml_ImportsValues)
{
    QDomElement validXml = documentRootFromString(
                "<root>"
                    "<Components>"
                        "<Component Type=\"ComponentA\">"
                            "<Profiles>"
                                "<Profile Name=\"No warning\" Probability=\"0.5\"/>"
                                "<Profile Name=\"Standard\" Probability=\"0.5\"/>"
                            "</Profiles>"
                            "<SensorLinks>"
                                "<SensorLink SensorId=\"0\" InputId=\"Camera\"/>"
                                "<SensorLink SensorId=\"1\" InputId=\"DrivingCorridor\"/>"
                                "<SensorLink SensorId=\"2\" InputId=\"Camera\"/>"
                            "</SensorLinks>"
                        "</Component>"
                    "</Components>"
                "</root>"
                );

    VehicleProfile profiles;

    EXPECT_NO_THROW(ProfilesImporter::ImportAllVehicleComponentsOfVehicleProfile(validXml, profiles));

    ASSERT_EQ(profiles.vehicleComponents.size(), (size_t )1);

    VehicleComponent resultVehicleComponent = profiles.vehicleComponents.front();

    ASSERT_EQ (resultVehicleComponent.componentProfiles.size(), (size_t) 2);
    ASSERT_EQ (resultVehicleComponent.sensorLinks.size(), (size_t) 3);
}

TEST(ProfilesImporter_ImportAllVehicleComponentsOfVehicleProfile, WithMissingComponentsTag_Throws)
{
    QDomElement missingComponentsTag = documentRootFromString(
                "<root>"
                        "<Component Type=\"ComponentA\">"
                            "<Profiles>"
                                "<Profile Name=\"No warning\" Probability=\"0.5\"/>"
                                "<Profile Name=\"Standard\" Probability=\"0.3\"/>"
                            "</Profiles>"
                            "<SensorLinks>"
                                "<SensorLink SensorId=\"0\" InputId=\"Camera\"/>"
                                "<SensorLink SensorId=\"1\" InputId=\"DrivingCorridor\"/>"
                                "<SensorLink SensorId=\"2\" InputId=\"Camera\"/>"
                            "</SensorLinks>"
                        "</Component>"
                "</root>"
                );

    VehicleProfile profiles;
    EXPECT_THROW(ProfilesImporter::ImportAllVehicleComponentsOfVehicleProfile(missingComponentsTag, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportAllVehicleComponentsOfVehicleProfile, SumOfProbabilityGreatherOne_Throws)
{
    QDomElement sumOfProbabilityGreatherOne = documentRootFromString(
                "<root>"
                    "<Components>"
                        "<Component Type=\"ComponentA\">"
                            "<Profiles>"
                                "<Profile Name=\"No warning\" Probability=\"0.5\"/>"
                                "<Profile Name=\"Standard\" Probability=\"0.8\"/>"
                            "</Profiles>"
                            "<SensorLinks>"
                                "<SensorLink SensorId=\"0\" InputId=\"Camera\"/>"
                                "<SensorLink SensorId=\"1\" InputId=\"DrivingCorridor\"/>"
                                "<SensorLink SensorId=\"2\" InputId=\"Camera\"/>"
                            "</SensorLinks>"
                        "</Component>"
                    "</Components>"
                "</root>"
                );

    VehicleProfile profiles;
    EXPECT_THROW(ProfilesImporter::ImportAllVehicleComponentsOfVehicleProfile(sumOfProbabilityGreatherOne, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportAllSensorsOfVehicleProfile, GivenValidXml_ImportsValues)
{
    QDomElement validXml = documentRootFromString(
                "<root>"
                    "<Sensors>"
                        "<Sensor Id=\"0\">"
                            "<Position Name=\"Somewhere\" Longitudinal=\"1.0\" Lateral=\"2.0\" Height=\"3.0\" Pitch=\"4.0\" Yaw=\"5.0\" Roll=\"6.0\"/>"
                            "<Profile Type=\"Geometric2D\" Name=\"Camera\"/>"
                        "</Sensor>"
                        "<Sensor Id=\"1\">"
                            "<Position Name=\"Somewhere\" Longitudinal=\"0.0\" Lateral=\"0.0\" Height=\"0.5\" Pitch=\"0.0\" Yaw=\"0.0\" Roll=\"0.0\"/>"
                            "<Profile Type=\"DrivingCorridor\" Name=\"Camera\"/>"
                        "</Sensor>"
                    "</Sensors>"
                "</root>"
                );

    VehicleProfile profiles;
    EXPECT_NO_THROW(ProfilesImporter::ImportAllSensorsOfVehicleProfile(validXml, profiles));

    openpass::sensors::Parameters resultSensors = profiles.sensors;

    ASSERT_EQ(resultSensors.size(), (size_t ) 2);

    openpass::sensors::Parameter resultSensorParameter = resultSensors.front();
    openpass::sensors::Position resultSensorPosition = resultSensorParameter.position;
    openpass::sensors::Profile resultSensorProfile = resultSensorParameter.profile;

    ASSERT_EQ (resultSensorPosition.name, "Somewhere");
    ASSERT_EQ (resultSensorPosition.longitudinal, 1.0);
    ASSERT_EQ (resultSensorPosition.lateral, 2.0);
    ASSERT_EQ (resultSensorPosition.height, 3.0);
    ASSERT_EQ (resultSensorPosition.pitch, 4.0);
    ASSERT_EQ (resultSensorPosition.yaw, 5.0);
    ASSERT_EQ (resultSensorPosition.roll, 6.0);

    ASSERT_EQ (resultSensorProfile.type, "Geometric2D");
    ASSERT_EQ (resultSensorProfile.name, "Camera");
}

TEST(ProfilesImporter_ImportAllSensorsOfVehicleProfile, SensorsTagMissing_Throws)
{
    QDomElement sensorsTagMissing = documentRootFromString(
                "<root>"
                        "<Sensor Id=\"0\">"
                            "<Position Name=\"Somewhere\" Longitudinal=\"1.0\" Lateral=\"2.0\" Height=\"3.0\" Pitch=\"4.0\" Yaw=\"5.0\" Roll=\"6.0\"/>"
                            "<Profile Type=\"Geometric2D\" Name=\"Camera\"/>"
                        "</Sensor>"
                        "<Sensor Id=\"1\">"
                            "<Position Name=\"Somewhere\" Longitudinal=\"0.0\" Lateral=\"0.0\" Height=\"0.5\" Pitch=\"0.0\" Yaw=\"0.0\" Roll=\"0.0\"/>"
                            "<Profile Type=\"DrivingCorridor\" Name=\"Camera\"/>"
                        "</Sensor>"
                "</root>"
                );
    VehicleProfile profiles;
    EXPECT_THROW(ProfilesImporter::ImportAllSensorsOfVehicleProfile(sensorsTagMissing, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportAllSensorsOfVehicleProfile, PositionTagMissing_Throws)
{
    QDomElement positionTagMissing = documentRootFromString(
                "<root>"
                    "<Sensors>"
                        "<Sensor Id=\"0\">"
                            "<Profile Type=\"Geometric2D\" Name=\"Camera\"/>"
                        "</Sensor>"
                    "</Sensors>"
                "</root>"
                );
    VehicleProfile profiles;
    EXPECT_THROW(ProfilesImporter::ImportAllSensorsOfVehicleProfile(positionTagMissing, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportAllSensorsOfVehicleProfile, ProfileTagMissing_Throws)
{
    QDomElement profileTagMissing = documentRootFromString(
                "<root>"
                    "<Sensors>"
                        "<Sensor Id=\"0\">"
                            "<Position Name=\"Somewhere\" Longitudinal=\"1.0\" Lateral=\"2.0\" Height=\"3.0\" Pitch=\"4.0\" Yaw=\"5.0\" Roll=\"6.0\"/>"
                        "</Sensor>"
                    "</Sensors>"
                "</root>"
                );
    VehicleProfile profiles;
    EXPECT_THROW(ProfilesImporter::ImportAllSensorsOfVehicleProfile(profileTagMissing, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportSensorProfiles, GivenValidXml_ImportsValues)
{
    QDomElement validXml = documentRootFromString(
                "<root>"
                      "<SensorProfile Name=\"Narrow\" Type=\"Geometric\">"
                            "<Int Key=\"Number1\" Value=\"1\"/>"
                            "<String Key=\"WordA\" Value=\"A\"/>"
                            "<NormalDistribution Key=\"Latency\" Mean=\"0\" SD=\"0\" Min=\"0\" Max=\"0\"/>"
                        "</SensorProfile>"
                        "<SensorProfile Name=\"Wide\" Type=\"Lane\">"
                            "<Int Key=\"Number1\" Value=\"2\"/>"
                            "<String Key=\"WordA\" Value=\"B\"/>"
                            "<NormalDistribution Key=\"Latency\" Mean=\"0\" SD=\"0\" Min=\"0\" Max=\"0\"/>"
                        "</SensorProfile>"
                "</root>"
                );

    openpass::sensors::Profiles profiles;
    EXPECT_NO_THROW(ProfilesImporter::ImportSensorProfiles(validXml, profiles));
    ASSERT_THAT(profiles, SizeIs(2));

    openpass::sensors::Profile& firstProfile = profiles.front();
    ASSERT_EQ(firstProfile.name, "Narrow");
    ASSERT_EQ(firstProfile.type, "Geometric");
    ASSERT_EQ(openpass::parameter::Get<int>(firstProfile.parameter, "Number1").has_value(), true);
    ASSERT_EQ(openpass::parameter::Get<std::string>(firstProfile.parameter, "WordA").has_value(), true);
}


TEST(ProfilesImporter_ImportSensorProfiles, MissingNameTagMissing_Throws)
{
    QDomElement nameTagMissing = documentRootFromString(
                "<root>"
                      "<SensorProfile Type=\"Geometric\">"
                            "<Int Key=\"Number1\" Value=\"1\"/>"
                            "<NormalDistribution Key=\"Latency\" Mean=\"0\" SD=\"0\" Min=\"0\" Max=\"0\"/>"
                      "</SensorProfile>"
                "</root>"
                );
    openpass::sensors::Profiles profiles;
    EXPECT_THROW(ProfilesImporter::ImportSensorProfiles(nameTagMissing, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportSensorProfiles, MissingTypeTagMissing_Throws)
{
    QDomElement typeTagMissing = documentRootFromString(
                "<root>"
                       "<SensorProfile Name=\"Narrow\">"
                            "<Int Key=\"Number1\" Value=\"1\"/>"
                            "<NormalDistribution Key=\"Latency\" Mean=\"0\" SD=\"0\" Min=\"0\" Max=\"0\"/>"
                        "</SensorProfile>"
                "</root>"
                );
    openpass::sensors::Profiles profiles;
    EXPECT_THROW(ProfilesImporter::ImportSensorProfiles(typeTagMissing, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportSensorProfiles, MissingLatencyMissing_Throws)
{
    QDomElement latencyMissing = documentRootFromString(
                "<root>"
                       "<SensorProfile Type=\"Geometric\" Name=\"Narrow\">"
                            "<Int Key=\"Number1\" Value=\"1\"/>"
                        "</SensorProfile>"
                "</root>"
                );
    openpass::sensors::Profiles profiles;
    EXPECT_THROW(ProfilesImporter::ImportSensorProfiles(latencyMissing, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportAgentProfiles, MissingAtLeastOneElement_Throws)
{
    QDomElement missingAtLeastOneElement = documentRootFromString(
                "<root>"
                    "<AgentProfile Name = \"Superhero\">"
                        "<DriverProfiles>"
                            "<DriverProfile Name = \"Regular\" Probability = \"0.5\"/>"
                            "<DriverProfile Name = \"Trajectory\" Probability = \"0.5\"/>"
                        "</DriverProfiles>"
                        "<ADASProfiles>"
                            "<ADASProfile Name = \"ComponentA\" Probability = \"0.5\"/>"
                        "</ADASProfiles>"
                        "<VehicleModels>"
                        "</VehicleModels>"
                    "</AgentProfile>"
                "</root>"
                );
    std::unordered_map<std::string, AgentProfile> profiles {};
    EXPECT_THROW(ProfilesImporter::ImportAgentProfiles(missingAtLeastOneElement, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportAgentProfiles, WrongProbabilities_Throws)
{
    QDomElement wrongProbabilities = documentRootFromString(
                "<root>"
                    "<AgentProfile Name = \"Superhero\">"
                        "<DriverProfiles>"
                            "<DriverProfile Name = \"Regular\" Probability = \"0.5\"/>"
                            "<DriverProfile Name = \"Trajectory\" Probability = \"0.5\"/>"
                        "</DriverProfiles>"
                        "<ADASProfiles>"
                            "<ADASProfile Name = \"ComponentA\" Probability = \"0.5\"/>"
                            "<ADASProfile Name = \"AEB\" Probability = \"1.0\"/>"
                        "</ADASProfiles>"
                        "<VehicleModels>"
                            "<VehicleModel Name = \"Batmobile\" Probability = \"1.5\"/>"
                        "</VehicleModels>"
                    "</AgentProfile>"
                "</root>"
                );
    std::unordered_map<std::string, AgentProfile> profiles {};
    EXPECT_THROW(ProfilesImporter::ImportAgentProfiles(wrongProbabilities, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportAgentProfiles, MissingTag_Throws)
{
    QDomElement missingTag = documentRootFromString(
                "<root>"
                    "<AgentProfile Name = \"Superhero\">"
                        "<ADASProfiles>"
                            "<ADASProfile Name = \"ComponentA\" Probability = \"0.5\"/>"
                            "<ADASProfile Name = \"AEB\" Probability = \"1.0\"/>"
                        "</ADASProfiles>"
                        "<VehicleModels>"
                            "<VehicleModel Name = \"Batmobile\" Probability = \"1.0\"/>"
                        "</VehicleModels>"
                    "</AgentProfile>"
                "</root>"
                );
    std::unordered_map<std::string, AgentProfile> profiles {};
    EXPECT_THROW(ProfilesImporter::ImportAgentProfiles(missingTag, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportDriverProfiles, GivenValidXml_ImportsValues)
{
    QDomElement validXml = documentRootFromString(
                "<root>"
                    "<DriverProfile Name = \"Regular\">"
                        "<String Key=\"Type\" Value=\"MyDriverModule\"/>"
                    "</DriverProfile>"
                    "<DriverProfile Name = \"TrajectoryFollower\">"
                        "<String Key=\"Type\" Value=\"MyDriverModule\"/>"
                        "<String Key=\"TrajectoryFile\" Value=\"Trajectory.csv\"/>"
                    "</DriverProfile>"
                "</root>"
                );

    DriverProfiles profiles;

    EXPECT_NO_THROW(ProfilesImporter::ImportDriverProfiles(validXml, profiles));
    ASSERT_THAT(profiles, SizeIs(2));

    auto resultParameter1 = profiles.at("Regular");
    auto resultParameter2 = profiles.at("TrajectoryFollower");

    EXPECT_THAT(resultParameter1, SizeIs(1));
    EXPECT_THAT(resultParameter2, SizeIs(2));
}

TEST(ProfilesImporter_ImportDriverProfiles, MissingType_Throws)
{
    QDomElement missingType = documentRootFromString(
                "<root>"
                    "<DriverProfile Name = \"Regular\">"
                        "<String Key=\"Type\" Value=\"MyDriverModule\"/>"
                    "</DriverProfile>"
                    "<DriverProfile Name = \"TrajectoryFollower\">"
                        "<String Key=\"TrajectoryFile\" Value=\"Trajectory.csv\"/>"
                    "</DriverProfile>"
                "</root>"
                );
    DriverProfiles profiles;
    EXPECT_THROW(ProfilesImporter::ImportDriverProfiles(missingType, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportDriverProfiles, WithDuplicateNames_Throws)
{
    QDomElement withDuplicateNames = documentRootFromString(
                "<root>"
                    "<DriverProfile Name = \"Regular\">"
                        "<String Key=\"Type\" Value=\"MyDriverModule\"/>"
                    "</DriverProfile>"
                    "<DriverProfile Name = \"Regular\">"
                    "<String Key=\"Type\" Value=\"MyDriverModule\"/>"
                        "<String Key=\"TrajectoryFile\" Value=\"Trajectory.csv\"/>"
                    "</DriverProfile>"
                "</root>"
                );

    DriverProfiles profiles;
    EXPECT_THROW(ProfilesImporter::ImportDriverProfiles(withDuplicateNames, profiles), std::runtime_error);
}

TEST(ProfilesImporter_ImportAllVehicleComponentProfiles, GivenValidXml_ImportsValues)
{
    QDomElement validXml = documentRootFromString(
                "<root>"
                    "<VehicleComponentProfile Type=\"ComponentA\" Name = \"ComponentAProfileOne\">"
                        "<Double Key=\"FakeDoubleOne\" Value=\"0.0\"/>"
                    "</VehicleComponentProfile>"
                    "<VehicleComponentProfile Type=\"ComponentA\" Name = \"ComponentAProfileTwo\">"
                        "<Double Key=\"FakeDoubleOne\" Value=\"0.0\"/>"
                    "</VehicleComponentProfile>"
                    "<VehicleComponentProfile Type=\"ComponentB\" Name = \"ComponentBProfileOne\">"
                        "<Double Key=\"FakeDoubleOne\" Value=\"0.0\"/>"
                    "</VehicleComponentProfile>"
                    "<VehicleComponentProfile Type=\"ComponentB\" Name = \"ComponentBProfileTwo\">"
                        "<Double Key=\"FakeDoubleOne\" Value=\"0.0\"/>"
                    "</VehicleComponentProfile>"
                    "<VehicleComponentProfile Type=\"ComponentC\" Name = \"ComponentCProfileOne\">"
                        "<Double Key=\"FakeDoubleOne\" Value=\"0.0\"/>"
                    "</VehicleComponentProfile>"
                "</root>"
                );

    std::unordered_map<std::string, VehicleComponentProfiles> fakeVehicleComponentProfiles;

    EXPECT_NO_THROW(ProfilesImporter::ImportAllVehicleComponentProfiles(validXml, fakeVehicleComponentProfiles));

    EXPECT_THAT(fakeVehicleComponentProfiles.at("ComponentA"), SizeIs(2));
    EXPECT_THAT(fakeVehicleComponentProfiles.at("ComponentB"), SizeIs(2));
    EXPECT_THAT(fakeVehicleComponentProfiles.at("ComponentC"), SizeIs(1));
}
